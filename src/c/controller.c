#include <pebble.h>

#include "model.h"
#include "view.h"
#include "utils.h"

static Layer *bg_canvas_layer, *fg_canvas_layer;
static Window *s_main_window;

static void update_clock(struct tm newtime);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static int lastcheck = 70;
static bool js_ready = false;

void require_update_timezone(struct tm *tm)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Requiring suntimes");

  char tz_buf2[6];

  DictionaryIterator *iter;

  app_message_outbox_begin(&iter);

  strftime(tz_buf2, 6, "%z", tm);
  dict_write_cstring(iter, MESSAGE_KEY_DELTAUTC, tz_buf2);
  app_message_outbox_send();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  Tuple *data_rd = dict_find(iterator, MESSAGE_KEY_JSREADY);
  if (data_rd)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "JS is now ready");
    js_ready = true;
  }

  Tuple *data_sun = dict_find(iterator, MESSAGE_KEY_SUNTIMES);
  if (data_sun)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Receiving new suntimes");
    int values[18];
    for (int s_index = 0; s_index < 18; s_index++)
    {
      Tuple *data_t = dict_find(iterator, MESSAGE_KEY_SUNTIMES + s_index);
      if (data_t)
      {
        // APP_LOG(APP_LOG_LEVEL_INFO, "Received %d", data_t->value->int32);
        values[s_index] = data_t->value->int32;
      }
    }
    model_set_suntimes(values, model_gtm.tm_yday, model_gtm.tm_gmtoff);
    layer_mark_dirty(bg_canvas_layer);
  }

  Tuple *data_sm = dict_find(iterator, MESSAGE_KEY_SHOW_MODE);
  if (data_sm)
  {
    model_show_mode = data_sm->value->int16;
    APP_LOG(APP_LOG_LEVEL_INFO, "Demo Mode %s", model_show_mode ? "enabled" : "disabled");
    layer_mark_dirty(bg_canvas_layer);
    layer_mark_dirty(fg_canvas_layer);
    lastcheck = 70;
  }

  Tuple *data_ct = dict_find(iterator, MESSAGE_KEY_CUSTOMTIME);
  if (data_ct && model_show_mode)
  {
    int custtime = data_ct->value->int32;
    struct tm custtime_tm = model_gtm;
    custtime_tm.tm_hour = custtime / 100;
    custtime_tm.tm_min = (custtime % 100) * 60 / 100;
    custtime_tm.tm_sec = 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "Custom time received %d:%02d", custtime_tm.tm_hour, custtime_tm.tm_min);
    update_clock(custtime_tm);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static bool last_day = -1;
static bool tz_update_pending = false;

static void update_clock(struct tm newtime)
{
  model_gtm = newtime;
  if (model_current_suntimes.yday != model_gtm.tm_yday)
    tz_update_pending = true;
  if (tz_update_pending && js_ready)
  {
    tz_update_pending = false;
    require_update_timezone(&model_gtm);
  }

  bool new_day = model_is_day();
  if (last_day != new_day)
  {
    last_day = new_day;
    layer_mark_dirty(bg_canvas_layer);
  }

  layer_mark_dirty(fg_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  if (model_current_suntimes.deltautc != tick_time->tm_gmtoff)
    tz_update_pending = true;
  if (tz_update_pending || tick_time->tm_sec <= lastcheck)
  {
    lastcheck = tick_time->tm_sec;
    if (!model_show_mode)
    {
      time_t rawTime;
      time(&rawTime);
      struct tm newtime = *localtime(&rawTime);
      APP_LOG(APP_LOG_LEVEL_INFO, "Updating clock");
      update_clock(newtime);
    }
  }
}

static void main_window_load(Window *window)
{

  // get window data
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  view_set_bounds(bounds);

  // create background layer
  bg_canvas_layer = layer_create(bounds);
  layer_set_update_proc(bg_canvas_layer, view_redraw_background);
  layer_mark_dirty(bg_canvas_layer);

  // create foreground layer
  fg_canvas_layer = layer_create(bounds);
  layer_set_update_proc(fg_canvas_layer, view_redraw_foreground);
  layer_mark_dirty(fg_canvas_layer);

  // Add to Window
  layer_add_child(window_get_root_layer(window), bg_canvas_layer);
  layer_add_child(window_get_root_layer(window), fg_canvas_layer);
}

static void main_window_unload(Window *window)
{
}

static void init()
{

  // create window
  model_init();
  view_init();

  time_t rawTime;
  time(&rawTime);
  model_gtm = *localtime(&rawTime);

  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorWhite);

  // handlers
  window_set_window_handlers(s_main_window, (WindowHandlers){
                                                .load = main_window_load,
                                                .unload = main_window_unload});

  // register ticktimerservice
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  // register phone communication
  app_message_register_inbox_received(inbox_received_callback);
  // Open AppMessage
  app_message_open(256, 128);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // show window with animation
  window_stack_push(s_main_window, true);
}

static void deinit()
{
  view_deinit();
  model_deinit();
  window_destroy(s_main_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}