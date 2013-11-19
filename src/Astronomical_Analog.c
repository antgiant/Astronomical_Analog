#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "suncalc.h"

/*   ------- Config Secion -------     */
#define SHOW_SECONDS false
#define SHOW_DATE true
#define SHOW_RING true
//LOW_RES_TIME means only updating when needed (better for battery assuming pebble is smart enough to only paint changed pixels)
#define LOW_RES_TIME false
#define INVERTED false
#define EAST_TO_WEST_ORB_ROTATION true //as opposed to clockwise
/*   ----- End Config Secion -----     */
	
	
#define MY_UUID { 0xE3, 0x2A, 0x8B, 0xDA, 0xE8, 0x2D, 0x45, 0x17, 0xA6, 0x9D, 0xA9, 0x29, 0x63, 0xFA, 0x84, 0x20 }

PBL_APP_INFO(MY_UUID,
             "Astronomical Analog", "Antgiant's Watches",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#if !INVERTED
const GColor BackgroundColor = GColorBlack;
const GColor ForegroundColor = GColorWhite;
#else
const GColor BackgroundColor = GColorWhite;
const GColor ForegroundColor = GColorBlack;
#endif
	
	static const GPathInfo HOUR_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {{-5, 14}, {-5, 0}, {0, -48}, {5, 0}, {5, 14}}
};
static const GPathInfo MINUTE_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {{-5, 14}, {-5, 0}, {0, -65}, {5, 0}, {5, 14}}
};
static const GPathInfo SECOND_HAND_POINTS = {
	.num_points = 2,
	.points = (GPoint []) {{-1, 14}, {-1, -65}}
};


Window window;
Layer watch_face_layer;
Layer sunlight_layer;
Layer hour_layer;
Layer minute_layer;
Layer second_layer;
Layer hand_pin_layer;
Layer orbiting_body_layer;
GPath hour_hand;
GPath minute_hand;
GPath second_hand;
TextLayer date_layer;
times suntimes;

//Provides the ability to move something in a circle by degrees (0 = Top center)
GPoint move_by_degrees(GPoint origin, int radius, int degrees) {
	GPoint newOrigin = origin;
	newOrigin.y = (-cos_lookup(TRIG_MAX_ANGLE * degrees / 360) * radius / TRIG_MAX_RATIO) + origin.y;
	newOrigin.x = (sin_lookup(TRIG_MAX_ANGLE * degrees / 360) * radius / TRIG_MAX_RATIO) + origin.x;
	return newOrigin;
}

void draw_watch_face(Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color(ctx, ForegroundColor);
	graphics_context_set_stroke_color(ctx, BackgroundColor);

	//Main circle for watch face
	GRect layer_rect = layer_get_bounds(layer);
	int layer_radius = 70;
	
	//Do different things if we have a sunset than if we do not.
	if (suntimes.sun_declin_deg <= -990.0) {
		//Draw Watch Background
		graphics_fill_circle(ctx, grect_center_point(&layer_rect), layer_radius);
#if SHOW_RING
		if (layer_radius > 2) {
			graphics_draw_circle(ctx, grect_center_point(&layer_rect), (layer_radius - 2));
		}
#endif
	}
	else {
	}
}

void draw_date() {
    PblTm t;
    get_time(&t);
	
	static char dom_text[] = "00";
	string_format_time(dom_text, sizeof(dom_text), "%e", &t);	
  
	text_layer_set_text(&date_layer, dom_text);
}

void draw_orbiting_body(Layer *layer, GContext *ctx) {
    PblTm t;
	int hour, angle;

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorBlack);

        get_time(&t);
        //Offset by 12 hours so top half is day and bottom half is night.
	hour = (t.tm_hour + 12)%24;

	//Get info about main circle for watch face
	GRect layer_rect = layer_get_bounds(layer);
	GPoint center_point = grect_center_point(&layer_rect);
	int layer_radius, body_radius;

	//This is the radius of watchface.
	layer_radius = 70;
	
	//Draw Orbiting Body
	body_radius = 7;
	
#if LOW_RES_TIME
	//Rotate orbiting body to proper spot (15 degrees per hour)
	angle = (15*hour);
#else
	int minute;
	minute = t.tm_min;
	//Rotate orbiting body to proper spot (15 degrees per hour + 1 degree per 4 minutes)
	angle = (15*hour) + (minute/4);
#endif
#if EAST_TO_WEST_ORB_ROTATION
        angle = -(angle - 360);
#endif	
	graphics_fill_circle(ctx, move_by_degrees(center_point, layer_radius - body_radius - 10, angle), body_radius);
	graphics_draw_circle(ctx, move_by_degrees(center_point, layer_radius - body_radius - 10, angle), body_radius);
}

void draw_hand_pin(Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

	//Main circle for watch face
	GRect layer_rect = layer_get_bounds(layer);
	GPoint center_point = grect_center_point(&layer_rect);
	int layer_radius;
	
	//Draw Hand Pin
	center_point.y = center_point.y - 14;
	layer_radius = 2;
	graphics_draw_circle(ctx, center_point, layer_radius);
}

void draw_hour_hand(Layer *layer, GContext *ctx) {
    PblTm t;
	int hour;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

    get_time(&t);
	hour = t.tm_hour%12;

#if LOW_RES_TIME
	//Rotate hour hand to to proper spot (30 degrees per hour)
	gpath_rotate_to(&hour_hand, (TRIG_MAX_ANGLE / 360) * (30*hour));
#else
	int minute;
	minute = t.tm_min;
	//Rotate hour hand to to proper spot (30 degrees per hour + 1 degree per 2 minutes)
	gpath_rotate_to(&hour_hand, (TRIG_MAX_ANGLE / 360) * ((30*hour) + (minute/2)));
#endif
	
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
	

#if LOW_RES_TIME || !SHOW_SECONDS
	//Rotate minute hand to to proper spot (6 degrees per minute)
	gpath_rotate_to(&minute_hand, (TRIG_MAX_ANGLE / 360) * (6*minute));
#else
	int second;
	second = t.tm_sec;
	//Rotate minute hand to to proper spot (6 degrees per minute + 1 degree per 10 seconds)
	gpath_rotate_to(&minute_hand, (TRIG_MAX_ANGLE / 360) * ((6*minute) + (second/10)));
#endif
	
	gpath_draw_filled(ctx, &minute_hand);
	gpath_draw_outline(ctx, &minute_hand);
}

void draw_second_hand(Layer *layer, GContext *ctx) {
    PblTm t;
	int second;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, BackgroundColor);

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
#if SHOW_DATE
	if (tickE->units_changed == 0 || tickE->units_changed & DAY_UNIT) {
        draw_date();
	}
#endif
	
//No need to seperatly update hour hand if done by another hand
#if LOW_RES_TIME
  	if (tickE->units_changed == 0 || tickE->units_changed & HOUR_UNIT) {
        layer_mark_dirty(&hour_layer);
		layer_mark_dirty(&orbiting_body_layer);
	}
#endif
#if LOW_RES_TIME || !SHOW_SECONDS
	if (tickE->units_changed == 0 || tickE->units_changed & MINUTE_UNIT) {
        layer_mark_dirty(&minute_layer);
  #if !LOW_RES_TIME
		//Only move hour hand every two minutes (due to 2 minutes per degree rotation).
		if (tickE->units_changed == 0 || tickE->tick_time->tm_min%2 == 0) {
        	layer_mark_dirty(&hour_layer);
	  		//Only move orb every 4 minutes (due to 4 minutes per degree rotation)
			if (tickE->tick_time->tm_min%4 == 0) {
				layer_mark_dirty(&orbiting_body_layer);
			}
		}
  #endif
	}
#endif
#if SHOW_SECONDS
  	if (tickE->units_changed == 0 || tickE->units_changed & SECOND_UNIT) {
        layer_mark_dirty(&second_layer);
  #if !LOW_RES_TIME
		//Only move minute hand every two seconds (due to 10 seconds per degree rotation).
		if (tickE->units_changed == 0 || tickE->tick_time->tm_sec%10 == 0) {
        	layer_mark_dirty(&minute_layer);
		}
	  	//Only move hour hand every two minutes (due to 2 minutes per degree rotation).
		if (tickE->units_changed == 0 || tickE->tick_time->tm_min%2 == 0) {
        	layer_mark_dirty(&hour_layer);
	  		//Only move orb every 4 minutes (due to 4 minutes per degree rotation)
			if (tickE->tick_time->tm_min%4 == 0) {
				layer_mark_dirty(&orbiting_body_layer);
			}
		}
  #endif
	}
#endif
	//Always redraw the hand pin
	layer_mark_dirty(&hand_pin_layer);
}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	suntimes.sun_declin_deg = -1000.0;
	
	window_init(&window, "Astronomical Analog");
	window_stack_push(&window, true /* Animated */);
	
#if !INVERTED
        window_set_background_color(&window, BackgroundColor);
#else
        window_set_background_color(&window, ForegroundColor);
#endif
	
	/* Main Watch Face */
	layer_init(&watch_face_layer, GRect(0, 14, 144, 144));
	watch_face_layer.update_proc = draw_watch_face;
	layer_add_child(&window.layer, &watch_face_layer);

#if SHOW_DATE
	/* Date */
	text_layer_init(&date_layer, GRect((int)(144/2 - 15), (int)(144/2 + 20), 30, 30));
	text_layer_set_font(&date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
	text_layer_set_text_color(&date_layer, BackgroundColor);
	text_layer_set_background_color(&date_layer, GColorClear);
	layer_add_child(&watch_face_layer, &date_layer.layer);	
	draw_date();
#endif
	
	/* Orbiting Body (aka 24 hour analog clock) */
	layer_init(&orbiting_body_layer, watch_face_layer.frame);
	orbiting_body_layer.update_proc = draw_orbiting_body;
	layer_add_child(&watch_face_layer, &orbiting_body_layer);
	layer_set_frame(&orbiting_body_layer, GRect(0, 0, 144, 144));

	/* Time (aka Clock Hands) */
#if SHOW_SECONDS
	//Second Hand
	layer_init(&second_layer, watch_face_layer.frame);
	second_layer.update_proc = draw_second_hand;
	layer_add_child(&watch_face_layer, &second_layer);
	gpath_init(&second_hand, &SECOND_HAND_POINTS);
	gpath_move_to(&second_hand, GPoint(72, 58));
#endif

	//Minute Hand
	layer_init(&minute_layer, watch_face_layer.frame);
	minute_layer.update_proc = draw_minute_hand;
	layer_add_child(&watch_face_layer, &minute_layer);
	gpath_init(&minute_hand, &MINUTE_HAND_POINTS);
	gpath_move_to(&minute_hand, GPoint(72, 58));
	
	//Hour Hand
	layer_init(&hour_layer, watch_face_layer.frame);
	hour_layer.update_proc = draw_hour_hand;
	layer_add_child(&watch_face_layer, &hour_layer);
	gpath_init(&hour_hand, &HOUR_HAND_POINTS);
	gpath_move_to(&hour_hand, GPoint(72, 58));
	
	//Hand Pin
	layer_init(&hand_pin_layer, watch_face_layer.frame);
	hand_pin_layer.update_proc = draw_hand_pin;
	layer_add_child(&watch_face_layer, &hand_pin_layer);

//   my_suntimes(float lat, float lon, PblTm time, float timezone, double angle);
//	PblTm t;
//    get_time(&t);
//	suntimes = my_suntimes(0.0, 0.0, t, -5, 90.833);
	
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.tick_info = {
			.tick_handler = &handle_tick,
#if SHOW_SECONDS && SHOW_DATE
			.tick_units = SECOND_UNIT | DAY_UNIT
#elif SHOW_SECONDS
			.tick_units = SECOND_UNIT
#elif SHOW_DATE
			.tick_units = MINUTE_UNIT | DAY_UNIT
#else
			.tick_units = MINUTE_UNIT
#endif
		}
	};
	app_event_loop(params, &handlers);
}