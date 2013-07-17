#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xE3, 0x2A, 0x8B, 0xDA, 0xE8, 0x2D, 0x45, 0x17, 0xA6, 0x9D, 0xA9, 0x29, 0x63, 0xFA, 0x84, 0x20 }

PBL_APP_INFO(MY_UUID,
             "Astronomical Analog", "Antgiant's Watches",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

const GColor BackgroundColor = GColorBlack;
const GColor ForegroundColor = GColorWhite;
static const GPathInfo HOUR_HAND_POINTS = {
	.num_points = 3,
	.points = (GPoint []) {{(int)-144*.05, 0}, {0, (int)-144/3}, {(int)144*.05, 0}}
};
static const GPathInfo MINUTE_HAND_POINTS = {
	.num_points = 3,
	.points = (GPoint []) {{(int)-144*.05, 0}, {0, (int)-144/2.2}, {(int)144*.05, 0}}
};
static const GPathInfo SECOND_HAND_POINTS = {
	.num_points = 3,
	.points = (GPoint []) {{-1.5, 0}, {-1.5, (int)-144/2.2}, {1.5, (int)-144/2.2}, {1.5, 0}}
};


Window window;
Layer watch_face_layer;
Layer hour_layer;
Layer minute_layer;
Layer second_layer;
GPath hour_hand;
GPath minute_hand;
GPath second_hand;

void draw_watch_face(Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color(ctx, ForegroundColor);
	graphics_context_set_stroke_color(ctx, BackgroundColor);

	//Main circle for watch face
	GRect layer_rect = layer_get_bounds(layer);
	int layer_radius;
	if (layer_rect.size.w < layer_rect.size.w) {
		layer_radius = (layer_rect.size.w/2) - 2;
	} else {
		layer_radius = (layer_rect.size.w/2) - 2;
	}
	
	//Draw Watch Background
	graphics_fill_circle(ctx, grect_center_point(&layer_rect), layer_radius);
	if (layer_radius > 2) {
		graphics_draw_circle(ctx, grect_center_point(&layer_rect), (layer_radius - 2));
	}
}

void draw_hour_hand(Layer *layer, GContext *ctx) {
    PblTm t;
	int hour;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

    get_time(&t);
	hour = t.tm_hour;

	if(!clock_is_24h_style()) {
		hour = hour%12;
		if(hour == 0) hour=12;
	}
	
	//Rotate hour hand to to proper spot (30 degrees per hour)
	gpath_rotate_to(&hour_hand, (TRIG_MAX_ANGLE / 360) * 30 * hour);
	
	gpath_draw_filled(ctx, &hour_hand);
	gpath_draw_outline(ctx, &hour_hand);
}

void draw_minute_hand(Layer *layer, GContext *ctx) {
    PblTm t;
	int minute;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

    get_time(&t);
	minute = t.tm_min;
	
	//Rotate minute hand to to proper spot (6 degrees per minute)
	gpath_rotate_to(&minute_hand, (TRIG_MAX_ANGLE / 360) * 6 * minute);
	
	gpath_draw_filled(ctx, &minute_hand);
	gpath_draw_outline(ctx, &minute_hand);
}

void draw_second_hand(Layer *layer, GContext *ctx) {
    PblTm t;
	int second;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

    get_time(&t);
	second = t.tm_sec;
	
	//Rotate second hand to to proper spot (6 degrees per second)
	gpath_rotate_to(&second_hand, (TRIG_MAX_ANGLE / 360) * 6 * second);
	
	gpath_draw_filled(ctx, &second_hand);
	gpath_draw_outline(ctx, &second_hand);
}

/* handle_tick is called at every time change. It updates 
   things appropriately*/
void handle_tick(AppContextRef ctx, PebbleTickEvent *tickE) {

	//NOTE: This is a Bit Mask Check not a and &&
	//Secondary Note: tickE->units_changed == 0 catches initialzation tick
  	if (tickE->units_changed == 0 || tickE->units_changed & DAY_UNIT) {
	  	// Update Day Layers
	}
  	if (tickE->units_changed == 0 || tickE->units_changed & HOUR_UNIT) {
        layer_mark_dirty(&hour_layer);
	}
  	if (tickE->units_changed == 0 || tickE->units_changed & MINUTE_UNIT) {
        layer_mark_dirty(&minute_layer);
	}
  	if (tickE->units_changed == 0 || tickE->units_changed & SECOND_UNIT) {
        layer_mark_dirty(&second_layer);
	}
}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "Astronomical Analog");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, BackgroundColor);

	/* Main Watch Face */
	layer_init(&watch_face_layer, GRect(0, 14, 144, 144));
	watch_face_layer.update_proc = draw_watch_face;
	layer_add_child(&window.layer, &watch_face_layer);
	
	/* Time (aka Clock Hands) */
	//Hour Hand
	layer_init(&hour_layer, watch_face_layer.frame);
	hour_layer.update_proc = draw_hour_hand;
	layer_add_child(&watch_face_layer, &hour_layer);
	gpath_init(&hour_hand, &HOUR_HAND_POINTS);
	gpath_move_to(&hour_hand, GPoint((int)(144/2), (int)(144/2) - 14));
	
	//Minute Hand
	layer_init(&minute_layer, watch_face_layer.frame);
	minute_layer.update_proc = draw_minute_hand;
	layer_add_child(&watch_face_layer, &minute_layer);
	gpath_init(&minute_hand, &MINUTE_HAND_POINTS);
	gpath_move_to(&minute_hand, GPoint((int)(144/2), (int)(144/2) - 14));
	
	//Second Hand
	layer_init(&second_layer, watch_face_layer.frame);
	second_layer.update_proc = draw_second_hand;
	layer_add_child(&watch_face_layer, &second_layer);
	gpath_init(&second_hand, &SECOND_HAND_POINTS);
	gpath_move_to(&second_hand, GPoint((int)(144/2), (int)(144/2) - 14));

}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.tick_info = {
			.tick_handler = &handle_tick,
			.tick_units = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT
		}
	};
	app_event_loop(params, &handlers);
}