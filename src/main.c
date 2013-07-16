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
Layer time_layer;
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

void draw_time_layer(Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color(ctx, ForegroundColor);
	graphics_context_set_stroke_color(ctx, BackgroundColor);

	gpath_init(&hour_hand, &HOUR_HAND_POINTS);
	gpath_move_to(&hour_hand, GPoint((int)(144/2), (int)(144/2)));
gpath_rotate_to(&hour_hand, (TRIG_MAX_ANGLE / 360) * 100);
	gpath_draw_filled(ctx, &hour_hand);
	gpath_draw_outline(ctx, &hour_hand);
	
	gpath_init(&minute_hand, &MINUTE_HAND_POINTS);
	gpath_move_to(&minute_hand, GPoint((int)(144/2), (int)(144/2)));
gpath_rotate_to(&minute_hand, (TRIG_MAX_ANGLE / 360) * 180);
	gpath_draw_filled(ctx, &minute_hand);
	gpath_draw_outline(ctx, &minute_hand);
	
	gpath_init(&second_hand, &SECOND_HAND_POINTS);
	gpath_move_to(&second_hand, GPoint((int)(144/2), (int)(144/2)));
gpath_rotate_to(&second_hand, (TRIG_MAX_ANGLE / 360) * 250);
	gpath_draw_filled(ctx, &second_hand);
	gpath_draw_outline(ctx, &second_hand);

}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "Astronomical Analog");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, BackgroundColor);

	/* Main Watch Face */
	layer_init(&watch_face_layer, GRect(0, 0, 144, 168));
	watch_face_layer.update_proc = draw_watch_face;
	layer_add_child(&window.layer, &watch_face_layer);
	
	/* Time (aka Clock Hands) */
	layer_init(&time_layer, watch_face_layer.frame);
	time_layer.update_proc = draw_time_layer;
	layer_add_child(&watch_face_layer, &time_layer);
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init
	};
	app_event_loop(params, &handlers);
}