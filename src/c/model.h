#ifndef MODEL_H
#define MODEL_H

#define PERSIST_KEY_SUNTIMES 0
// #define PERSIST_KEY_DAYOFTHEYEAR 1
// #define PERSIST_KEY_DELTAUTC 2

#include <pebble.h>

// ------------------------- TYPEDEFS -------------------------

typedef struct model_small_time
{
  int h;
  int m;
  int tot_m;
  int angle;
} model_small_time;

typedef struct model_suntimes
{
  int yday;
  int deltautc;
  model_small_time s_day;
  model_small_time e_day;
  model_small_time s_civ;
  model_small_time e_civ;
  model_small_time s_nau;
  model_small_time e_nau;
  model_small_time s_ast;
  model_small_time e_ast;
  model_small_time noon;
} model_suntimes;

// ------------------------------------------------------------

// -------------------------- GLOBALS -------------------------

extern bool model_show_mode;
extern struct tm model_gtm;
extern model_suntimes model_current_suntimes;

// ------------------------------------------------------------

// ---------------------------- FNS ---------------------------

void model_init();
void model_deinit();
void model_update_totals_angles();
void model_set_suntimes(int *values, int yday, int deltautc);
int model_small_time_to_m(model_small_time st);
int model_small_time_to_angle(model_small_time st);
bool model_is_day();

// ------------------------------------------------------------

#endif