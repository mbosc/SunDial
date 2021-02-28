#ifndef VIEW_H
#define VIEW_H

#include <pebble.h>
#include "model.h"

// -------------------------- GLOBALS -------------------------

extern GPoint view_centre;
extern int view_crown_ray;
extern int view_inner_crown_ray;
extern int view_inner_circle_ray;
extern GColor view_colday;
extern GColor view_colnit;
extern GColor view_col1;
extern GColor view_col2;
extern GColor view_col3;
extern GColor view_col4;
extern GFont view_dial_font;
extern GRect view_window_bounds;

// ------------------------------------------------------------

// ---------------------------- FNS ---------------------------

void view_init();
void view_deinit();
void view_redraw_foreground(Layer *layer, GContext *ctx);
void view_redraw_background(Layer *layer, GContext *ctx);
void view_set_bounds(GRect bounds);

// ------------------------------------------------------------

#endif