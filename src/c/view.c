#include "view.h"

GPoint view_centre;
int view_crown_ray;
int view_inner_crown_ray;
int view_inner_circle_ray;

GColor view_colday;
GColor view_colnit;
GColor view_col1;
GColor view_col2;
GColor view_col3;
GColor view_col4;
GFont view_dial_font;
GFont view_main_font;
GRect view_window_bounds;
// GBitmap *view_bitmap_bg_day, *view_bitmap_bg_nite;

void view_init()
{
  view_crown_ray = PBL_IF_ROUND_ELSE(89, 70);
  view_inner_crown_ray = PBL_IF_ROUND_ELSE(67, 50);
  view_inner_circle_ray = PBL_IF_ROUND_ELSE(50, 34);
  view_centre = PBL_IF_ROUND_ELSE(GPoint(89, 89), GPoint(71, 84));

  view_dial_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_THREE_7));
  view_main_font = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_VIC_30, RESOURCE_ID_FONT_VIC_20)));
  view_colday = GColorCeleste;
  view_colnit = GColorDukeBlue;
  view_col1 = GColorBabyBlueEyes;
  view_col2 = GColorBlueMoon;
  view_col3 = GColorBlue;
  view_col4 = GColorOxfordBlue;
  // view_bitmap_bg_day = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_DAY);
  // view_bitmap_bg_nite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_NITE);
}

void view_set_bounds(GRect bounds)
{
  view_window_bounds = bounds;
}

void graphics_fill_radial_force(GContext *ctx, GRect rect, GOvalScaleMode scale_mode, uint16_t inset_thickness, int32_t angle_start, int32_t angle_end)
{
  if (TRIGANGLE_TO_DEG(angle_start) > TRIGANGLE_TO_DEG(angle_end))
    graphics_fill_radial(ctx, rect, scale_mode, inset_thickness, DEG_TO_TRIGANGLE(TRIGANGLE_TO_DEG(angle_start) - 360), angle_end);
  else
    graphics_fill_radial(ctx, rect, scale_mode, inset_thickness, angle_start, angle_end);
}

void view_deinit()
{
  // gbitmap_destroy(view_bitmap_bg_day);
  // gbitmap_destroy(view_bitmap_bg_nite);
  fonts_unload_custom_font(view_dial_font);
}

void view_redraw_foreground(Layer *layer, GContext *ctx)
{

  int time_tt = model_gtm.tm_sec + model_gtm.tm_min * 60 + model_gtm.tm_hour * 3600;

  bool is_day = model_is_day();

  graphics_context_set_antialiased(ctx, false);

  int angle_p = DEG_TO_TRIGANGLE(180 + time_tt / 240);

  GPoint end_p = GPoint(sin_lookup(angle_p) * (view_crown_ray - 4) / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * (view_crown_ray - 4) / TRIG_MAX_RATIO + view_centre.y);
  int sun_ray = 5;

  GPoint sun_p = GPoint(sin_lookup(angle_p) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  if (is_day)
  {
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 2);
    // draw time hand
    graphics_draw_line(ctx, view_centre, end_p);
    graphics_context_set_stroke_width(ctx, 1);
    // here comes the sun
    graphics_fill_circle(ctx, sun_p, sun_ray);
    graphics_draw_circle(ctx, sun_p, sun_ray);
    // draw inner circle
    graphics_fill_circle(ctx, view_centre, view_inner_circle_ray);
    graphics_draw_circle(ctx, view_centre, view_inner_circle_ray);
    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_context_set_stroke_color(ctx, GColorWhite);
  }
  else
  {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 2);
    // draw time hand
    graphics_draw_line(ctx, view_centre, end_p);
    graphics_context_set_stroke_width(ctx, 1);
    // here comes the sun
    graphics_fill_circle(ctx, sun_p, sun_ray);
    graphics_draw_circle(ctx, sun_p, sun_ray);
    // draw inner circle
    graphics_fill_circle(ctx, view_centre, view_inner_circle_ray);
    graphics_draw_circle(ctx, view_centre, view_inner_circle_ray);
    graphics_context_set_text_color(ctx, GColorWhite);
  }

  // print time
  GRect font_bounds_sq = GRect(view_centre.x - view_inner_crown_ray, view_centre.y - 13, 2 * view_inner_crown_ray + 1, 20);
  GRect font_bounds_rd = GRect(view_centre.x - view_inner_crown_ray, view_centre.y - 20, 2 * view_inner_crown_ray + 1, 20);
  GRect font_bounds = PBL_IF_ROUND_ELSE(font_bounds_rd, font_bounds_sq);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", &model_gtm);
  graphics_draw_text(ctx, s_buffer, view_main_font, font_bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void view_redraw_background(Layer *layer, GContext *ctx)
{

  graphics_context_set_stroke_color(ctx, GColorRed);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_antialiased(ctx, false);

  bool is_day = model_is_day();

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, view_window_bounds, 0, GCornerNone);

  // draw sky
  if (is_day)
  {
    // graphics_draw_bitmap_in_rect(ctx, view_bitmap_bg_day, view_window_bounds);
    graphics_context_set_fill_color(ctx, view_colday);
    graphics_fill_circle(ctx, view_centre, view_crown_ray);
  }
  else
  {
    // graphics_draw_bitmap_in_rect(ctx, view_bitmap_bg_nite, view_window_bounds);
    graphics_context_set_fill_color(ctx, view_colnit);
    graphics_fill_circle(ctx, view_centre, view_crown_ray);
  }

  // draw civil twilight zone (sunset --> dawn)
  graphics_context_set_fill_color(ctx, view_col1);
  graphics_fill_radial_force(ctx, GRect(view_centre.x - view_crown_ray, view_centre.y - view_crown_ray, view_crown_ray * 2 + 1, view_crown_ray * 2 + 1), GOvalScaleModeFitCircle,
                             view_crown_ray, model_current_suntimes.e_day.angle, model_current_suntimes.s_day.angle);

  // draw nautical twilight zone (civil sunset --> civil dawn)
  graphics_context_set_fill_color(ctx, view_col2);
  graphics_fill_radial_force(ctx, GRect(view_centre.x - view_crown_ray, view_centre.y - view_crown_ray, view_crown_ray * 2 + 1, view_crown_ray * 2 + 1), GOvalScaleModeFitCircle,
                             view_crown_ray, model_current_suntimes.e_civ.angle, model_current_suntimes.s_civ.angle);

  // draw astronomical twilight zone (nautical sunset --> nautical dawn)
  graphics_context_set_fill_color(ctx, view_col3);
  graphics_fill_radial_force(ctx, GRect(view_centre.x - view_crown_ray, view_centre.y - view_crown_ray, view_crown_ray * 2 + 1, view_crown_ray * 2 + 1), GOvalScaleModeFitCircle,
                             view_crown_ray, model_current_suntimes.e_nau.angle, model_current_suntimes.s_nau.angle);

  // draw night zone (astronomical sunset --> astronomical dawn)
  graphics_context_set_fill_color(ctx, view_col4);
  graphics_fill_radial_force(ctx, GRect(view_centre.x - view_crown_ray, view_centre.y - view_crown_ray, view_crown_ray * 2 + 1, view_crown_ray * 2 + 1), GOvalScaleModeFitCircle,
                             view_crown_ray, model_current_suntimes.e_ast.angle, model_current_suntimes.s_ast.angle);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);

  // draw sun circle
  graphics_fill_radial(ctx, GRect(view_centre.x - view_inner_crown_ray, view_centre.y - view_inner_crown_ray, view_inner_crown_ray * 2 + 1, view_inner_crown_ray * 2 + 1), GOvalScaleModeFitCircle,
                       1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  if (is_day)
  {
    // make sun circle black in day zone
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_radial(ctx, GRect(view_centre.x - view_inner_crown_ray, view_centre.y - view_inner_crown_ray, view_inner_crown_ray * 2 + 1, view_inner_crown_ray * 2 + 1), GOvalScaleModeFitCircle,
                         1, DEG_TO_TRIGANGLE(180 + model_current_suntimes.s_day.tot_m * 360 / 1440), DEG_TO_TRIGANGLE(180 + model_current_suntimes.e_day.tot_m * 360 / 1440));
  }

  int small_circle_ray = 2;
  int big_circle_ray = 3;

  // draw position markers on sun circle
  GPoint noon_p = GPoint(sin_lookup(model_current_suntimes.noon.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.noon.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, noon_p, big_circle_ray);
  graphics_context_set_fill_color(ctx, GColorWhite);
  GPoint s_twi_p = GPoint(sin_lookup(model_current_suntimes.s_day.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.s_day.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, s_twi_p, small_circle_ray);
  GPoint e_twi_p = GPoint(sin_lookup(model_current_suntimes.e_day.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.e_day.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, e_twi_p, small_circle_ray);
  GPoint s_civ_p = GPoint(sin_lookup(model_current_suntimes.s_civ.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.s_civ.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, s_civ_p, small_circle_ray);
  GPoint e_civ_p = GPoint(sin_lookup(model_current_suntimes.e_civ.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.e_civ.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, e_civ_p, small_circle_ray);
  GPoint s_nau_p = GPoint(sin_lookup(model_current_suntimes.s_nau.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.s_nau.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, s_nau_p, small_circle_ray);
  GPoint e_nau_p = GPoint(sin_lookup(model_current_suntimes.e_nau.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.e_nau.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, e_nau_p, small_circle_ray);
  GPoint s_ast_p = GPoint(sin_lookup(model_current_suntimes.s_ast.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.s_ast.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, s_ast_p, small_circle_ray);
  GPoint e_ast_p = GPoint(sin_lookup(model_current_suntimes.e_ast.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(model_current_suntimes.e_ast.angle) * view_inner_crown_ray / TRIG_MAX_RATIO + view_centre.y);
  graphics_fill_circle(ctx, e_ast_p, small_circle_ray);

  graphics_context_set_stroke_width(ctx, 2);

  // draw hour/half-hour markers
  for (int hh = 0; hh < 48; hh++)
  {
    int is_day_hh = ((hh / 2) * 60 + (hh % 2) * 30) > model_current_suntimes.s_civ.tot_m && ((hh / 2) * 60 + (hh % 2) * 30) < model_current_suntimes.e_civ.tot_m;

    if (is_day_hh && is_day)
    {
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_text_color(ctx, GColorBlack);
      int angle_p = DEG_TO_TRIGANGLE(180 + hh * 360 / 48);
      GPoint sta_p = GPoint(sin_lookup(angle_p) * (view_crown_ray - (hh % 2 ? 3 : 5)) / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * (view_crown_ray - (hh % 2 ? 3 : 5)) / TRIG_MAX_RATIO + view_centre.y);
      GPoint end_p = GPoint(sin_lookup(angle_p) * view_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * view_crown_ray / TRIG_MAX_RATIO + view_centre.y);
      graphics_draw_line(ctx, sta_p, end_p);
      if (hh % 4 == 0)
      {
        GPoint font_p = GPoint(sin_lookup(angle_p) * (view_crown_ray - 11) / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * (view_crown_ray - 11) / TRIG_MAX_RATIO + view_centre.y);
        GRect font_bounds = GRect(font_p.x - 4, font_p.y - 4, 10, 8);
        char lebuff[3];
        snprintf(lebuff, 3, "%02d", hh == 0 ? 24 : (hh / 2));
        graphics_draw_text(ctx, lebuff, view_dial_font, font_bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      graphics_context_set_text_color(ctx, GColorWhite);
      graphics_context_set_stroke_color(ctx, GColorWhite);
    }
    else
    {

      int angle_p = DEG_TO_TRIGANGLE(180 + hh * 360 / 48);
      GPoint sta_p = GPoint(sin_lookup(angle_p) * (view_crown_ray - (hh % 2 ? 3 : 5)) / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * (view_crown_ray - (hh % 2 ? 3 : 5)) / TRIG_MAX_RATIO + view_centre.y);
      GPoint end_p = GPoint(sin_lookup(angle_p) * view_crown_ray / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * view_crown_ray / TRIG_MAX_RATIO + view_centre.y);
      graphics_draw_line(ctx, sta_p, end_p);
      if (hh % 4 == 0)
      {
        GPoint font_p = GPoint(sin_lookup(angle_p) * (view_crown_ray - 11) / TRIG_MAX_RATIO + view_centre.x, -cos_lookup(angle_p) * (view_crown_ray - 11) / TRIG_MAX_RATIO + view_centre.y);
        GRect font_bounds = GRect(font_p.x - 4, font_p.y - 4, 10, 8);
        char lebuff[3];
        snprintf(lebuff, 3, "%02d", hh == 0 ? 24 : (hh / 2));
        graphics_draw_text(ctx, lebuff, view_dial_font, font_bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }
  }

  // draw outer crown
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_circle(ctx, view_centre, view_crown_ray);
}