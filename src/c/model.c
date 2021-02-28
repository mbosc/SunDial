#include "model.h"

bool model_show_mode = 0;

struct tm model_gtm;
model_suntimes model_current_suntimes = {
    -1,
    0,
    {6, 1, 0, 0},
    {16, 58, 0, 0},
    {5, 32, 0, 0},
    {17, 27, 0, 0},
    {4, 58, 0, 0},
    {18, 1, 0, 0},
    {4, 24, 0, 0},
    {18, 35, 0, 0},
    {11, 29, 0, 0}};

void model_init()
{
  if (persist_exists(PERSIST_KEY_SUNTIMES))
    persist_read_data(PERSIST_KEY_SUNTIMES, &model_current_suntimes, sizeof(model_current_suntimes));
  model_update_totals_angles();
}

void model_deinit()
{
  persist_write_data(PERSIST_KEY_SUNTIMES, &model_current_suntimes, sizeof(model_current_suntimes));
}

void model_set_suntimes(int *values, int yday, int deltautc)
{
  model_current_suntimes.yday = yday;
  model_current_suntimes.deltautc = deltautc;
  model_current_suntimes.s_day.h = values[0];
  model_current_suntimes.s_day.m = values[1];
  model_current_suntimes.e_day.h = values[2];
  model_current_suntimes.e_day.m = values[3];
  model_current_suntimes.s_civ.h = values[4];
  model_current_suntimes.s_civ.m = values[5];
  model_current_suntimes.e_civ.h = values[6];
  model_current_suntimes.e_civ.m = values[7];
  model_current_suntimes.s_nau.h = values[8];
  model_current_suntimes.s_nau.m = values[9];
  model_current_suntimes.e_nau.h = values[10];
  model_current_suntimes.e_nau.m = values[11];
  model_current_suntimes.s_ast.h = values[12];
  model_current_suntimes.s_ast.m = values[13];
  model_current_suntimes.e_ast.h = values[14];
  model_current_suntimes.e_ast.m = values[15];
  model_current_suntimes.noon.h = values[16];
  model_current_suntimes.noon.m = values[17];
  model_update_totals_angles();
}

void model_update_totals_angles()
{
  model_current_suntimes.s_day.angle = model_small_time_to_angle(model_current_suntimes.s_day);
  model_current_suntimes.s_day.tot_m = model_small_time_to_m(model_current_suntimes.s_day);
  model_current_suntimes.e_day.angle = model_small_time_to_angle(model_current_suntimes.e_day);
  model_current_suntimes.e_day.tot_m = model_small_time_to_m(model_current_suntimes.e_day);
  model_current_suntimes.s_civ.angle = model_small_time_to_angle(model_current_suntimes.s_civ);
  model_current_suntimes.s_civ.tot_m = model_small_time_to_m(model_current_suntimes.s_civ);
  model_current_suntimes.e_civ.angle = model_small_time_to_angle(model_current_suntimes.e_civ);
  model_current_suntimes.e_civ.tot_m = model_small_time_to_m(model_current_suntimes.e_civ);
  model_current_suntimes.s_nau.angle = model_small_time_to_angle(model_current_suntimes.s_nau);
  model_current_suntimes.s_nau.tot_m = model_small_time_to_m(model_current_suntimes.s_nau);
  model_current_suntimes.e_nau.angle = model_small_time_to_angle(model_current_suntimes.e_nau);
  model_current_suntimes.e_nau.tot_m = model_small_time_to_m(model_current_suntimes.e_nau);
  model_current_suntimes.s_ast.angle = model_small_time_to_angle(model_current_suntimes.s_ast);
  model_current_suntimes.s_ast.tot_m = model_small_time_to_m(model_current_suntimes.s_ast);
  model_current_suntimes.e_ast.angle = model_small_time_to_angle(model_current_suntimes.e_ast);
  model_current_suntimes.e_ast.tot_m = model_small_time_to_m(model_current_suntimes.e_ast);
  model_current_suntimes.noon.angle = model_small_time_to_angle(model_current_suntimes.noon);
  model_current_suntimes.noon.tot_m = model_small_time_to_m(model_current_suntimes.noon);
}

int model_small_time_to_m(model_small_time st)
{
  return st.h * 60 + st.m;
}

int model_small_time_to_angle(model_small_time st)
{
  int ang = 180 + model_small_time_to_m(st) * 360 / 1440;
  return DEG_TO_TRIGANGLE(ang % 360);
}

bool model_is_day()
{
  int tt = model_gtm.tm_sec + model_gtm.tm_min * 60 + model_gtm.tm_hour * 3600;
  return tt > model_current_suntimes.s_day.tot_m * 60 && tt < model_current_suntimes.e_day.tot_m * 60;
}