#include <pebble.h>
#include "suncalc.h"

//Last number is the max # of integers to be passed
#define INBOX_SIZE  (1 + (7+4) * 6)
#define OUTBOX_SIZE (1 + (7+4) * 6)

/*   ------- Config Secion -------     */
#define SHOW_SECONDS_OLD true
#define SHOW_DATE_OLD true
#define SHOW_RING_OLD false
//LOW_RES_TIME_OLD means only updating when needed (better for battery assuming pebble is smart enough to only paint changed pixels)
#define LOW_RES_TIME_OLD false
#define INVERTED_OLD false
#define EAST_TO_WEST_ORB_ROTATION_OLD true //as opposed to clockwise
/*   ----- End Config Secion -----     */
	
GColor BackgroundColor, ForegroundColor;
	
static GPathInfo HOUR_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {{-5, 14}, {-5, 0}, {0, -48}, {5, 0}, {5, 14}}
};
static GPathInfo MINUTE_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {{-5, 14}, {-5, 0}, {0, -65}, {5, 0}, {5, 14}}
};
static GPathInfo SECOND_HAND_POINTS_BACKGROUND = {
	.num_points = 2,
	.points = (GPoint []) {{-2, 14}, {-2, -65}}
};
static GPathInfo SECOND_HAND_POINTS_FOREGROUND = {
	.num_points = 2,
	.points = (GPoint []) {{2, 14}, {2, -65}}
};
GPathInfo night_pattern_points = {
	.num_points = 7,
	.points = (GPoint []) {{0, 0}, {1, 0}, {2, 2}, {3, 0}, {4, 0}, {4, 4}, {0, 4}}
};

Window *window;
Layer *watch_face_layer;
Layer *hour_layer;
Layer *minute_layer;
Layer *second_layer;
Layer *hand_pin_layer;
Layer *orbiting_body_layer;
TextLayer *date_layer;
TextLayer *date_layer_shadow;
TextLayer *sunup_layer;
TextLayer *sundown_layer;
GPath *hour_hand;
GPath *minute_hand;
GPath *second_hand_foreground;
GPath *second_hand_background;
GPath *night_pattern;
times suntimes;
float latitude, longitude, timezone;
double sun_angle = 90.833; //This is the official angle of the sun for sunrise/sunset
bool have_gps_fix = false;
time_t current_time;
struct tm *t;
enum ConfigOptions {
	CONFIG_VERSION,
	SHOW_SECONDS,
	SHOW_DATE,
	SHOW_RING,
	LOW_RES_TIME,
	INVERTED,
	EAST_TO_WEST_ORB_ROTATION,
};
bool show_seconds = true;
bool show_date = true;
bool show_ring = false;
bool low_res_time = false;
bool inverted = false;
bool east_to_west_orb_rotation = true;
bool first_run = true;

//Function declaration so that next function can use it.
void handle_tick(struct tm *tickE, TimeUnits units_changed);

void update_screen(){
		tick_timer_service_unsubscribe();
		tick_timer_service_subscribe(HOUR_UNIT|MINUTE_UNIT|SECOND_UNIT, handle_tick);
	
	if (first_run) {
		first_run = false;
	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Set Watch Style");
}

void handle_appmessage_receive(DictionaryIterator *iter, void *context) {
	// Read show seconds preference
	Tuple *tuple = dict_find(iter, MESSAGE_KEY_show_seconds);
	if(tuple) {
		show_seconds = tuple->value->int32;
		persist_write_int(MESSAGE_KEY_show_seconds, show_seconds);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved new show seconds preference (%i).", show_seconds);
		update_screen();
	} else if (persist_exists(MESSAGE_KEY_show_seconds)) {
		show_seconds = persist_read_int(MESSAGE_KEY_show_seconds); 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded show seconds preference from watch storage. (%i)", show_seconds);
		update_screen();
	} else {
		show_seconds = true; //Default to true 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded default show seconds preference (%i). (No saved settings).", show_seconds);
	}
	
	// Read show date preference
	tuple = dict_find(iter, MESSAGE_KEY_show_date);
	if(tuple) {
		show_date = tuple->value->int32;
		persist_write_int(MESSAGE_KEY_show_date, show_date);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved new show date preference (%i).", show_date);
		update_screen();
	} else if (persist_exists(MESSAGE_KEY_show_date)) {
		show_seconds = persist_read_int(MESSAGE_KEY_show_date); 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded show date preference from watch storage. (%i)", show_date);
		update_screen();
	} else {
		show_date = true; //Default to true 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded default show date preference (%i). (No saved settings).", show_date);
	}
	
	// Read show ring preference
	tuple = dict_find(iter, MESSAGE_KEY_show_ring);
	if(tuple) {
		show_ring = tuple->value->int32;
		persist_write_int(MESSAGE_KEY_show_ring, show_ring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved new show ring preference (%i).", show_ring);
		update_screen();
	} else if (persist_exists(MESSAGE_KEY_show_ring)) {
		show_ring = persist_read_int(MESSAGE_KEY_show_ring); 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded show ring preference from watch storage. (%i)", show_ring);
		update_screen();
	} else {
		show_ring = false; //Default to false 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded default show false preference (%i). (No saved settings).", show_ring);
	}
	
	// Read low resolution time preference
	tuple = dict_find(iter, MESSAGE_KEY_low_res_time);
	if(tuple) {
		low_res_time = tuple->value->int32;
		persist_write_int(MESSAGE_KEY_low_res_time, low_res_time);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved new low resolution time preference (%i).", low_res_time);
		update_screen();
	} else if (persist_exists(MESSAGE_KEY_low_res_time)) {
		low_res_time = persist_read_int(MESSAGE_KEY_low_res_time); 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded low resolution time preference from watch storage. (%i)", low_res_time);
		update_screen();
	} else {
		low_res_time = false; //Default to false 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded default low resolution time preference (%i). (No saved settings).", low_res_time);
	}
	
	// Read inverted colors preference
	tuple = dict_find(iter, MESSAGE_KEY_inverted);
	if(tuple) {
		inverted = tuple->value->int32;
		persist_write_int(MESSAGE_KEY_inverted, inverted);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved new inverted colors preference (%i).", inverted);
		update_screen();
	} else if (persist_exists(MESSAGE_KEY_inverted)) {
		inverted = persist_read_int(MESSAGE_KEY_inverted); 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded use inverted colors preference from watch storage. (%i)", inverted);
		update_screen();
	} else {
		inverted = true; //Default to true 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded default use inverted colors preference (%i). (No saved settings).", inverted);
	}
	
	// Read rotate orb properly preference
	tuple = dict_find(iter, MESSAGE_KEY_east_to_west_orb_rotation);
	if(tuple) {
		east_to_west_orb_rotation = tuple->value->int32;
		persist_write_int(MESSAGE_KEY_east_to_west_orb_rotation, east_to_west_orb_rotation);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved new east to west orb rotation preference (%i).", east_to_west_orb_rotation);
		update_screen();
	} else if (persist_exists(MESSAGE_KEY_east_to_west_orb_rotation)) {
		east_to_west_orb_rotation = persist_read_int(MESSAGE_KEY_east_to_west_orb_rotation); 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded east to west orb rotation preference from watch storage. (%i)", east_to_west_orb_rotation);
		update_screen();
	} else {
		east_to_west_orb_rotation = true; //Default to true 
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded east to west orb rotation preference (%i). (No saved settings).", east_to_west_orb_rotation);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Completed processing config data");
}

void load_saved_config_options() {
	int current_version = 1;
	int saved_version;
	if (persist_exists(MESSAGE_KEY_config_version)) {
		saved_version = persist_read_int(MESSAGE_KEY_config_version);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded Config Version (%i)", saved_version);	
	} else {
		saved_version = current_version;
		persist_write_int(MESSAGE_KEY_config_version, saved_version);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved config version to persistent storage.");
	}
	if (saved_version < current_version) {
		//Deal with old data
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "Upgrading Config data");	
		
//		persist_write_int(MESSAGE_KEY_config_version, current_version);
		
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "Upgrade of Config data complete");
	}
	else if (saved_version > current_version) {
		show_seconds = true;
		show_date = true;
		show_ring = false;
		low_res_time = false;
		inverted = false;
		east_to_west_orb_rotation = true;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Using defaults as config data is from the future");	
	}
	
	//Config is now up to date use handle_appmessage_receive to load the default values
	DictionaryIterator iter;
	uint8_t buffer[INBOX_SIZE];
	dict_write_begin(&iter, buffer, sizeof(buffer));
	void *context = NULL;
	handle_appmessage_receive(&iter, context);
	
}

//Provides the ability to move something in a circle clockwise by degrees (0 = Top center)
GPoint move_by_degrees(GPoint origin, int radius, int degrees) {
	GPoint newOrigin = origin;
	newOrigin.y = (-cos_lookup(TRIG_MAX_ANGLE * degrees / 360) * radius / TRIG_MAX_RATIO) + origin.y;
	newOrigin.x = (sin_lookup(TRIG_MAX_ANGLE * degrees / 360) * radius / TRIG_MAX_RATIO) + origin.x;
	return newOrigin;
}

//Provides the ability to return a point moving around a rectangle clockwise by degrees (0 = Top center)
GPoint move_by_degrees_rectangle(GRect rect, int degrees) {
	GPoint point;
	degrees = degrees%360;
	if (degrees < 0) {
		degrees = 360 + degrees;
	}
	if (degrees <= 45 || degrees >= 315 || (degrees >= 135 && degrees <= 225)) {
		if (degrees >= 135 && degrees <= 225) {
			point.y = rect.size.h;
			point.x = rect.size.w - ((rect.size.h*((sin_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0)/(cos_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0)))/2 + rect.size.w/2);
		}
		else {
			point.y = 0;
			point.x = (rect.size.h*((sin_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0)/(cos_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0)))/2 + rect.size.w/2;
		}
	}
	else { 
		if (degrees > 225 && degrees < 315) {
			point.x = 0;
			point.y = (rect.size.w/(2.0*(sin_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0)/(cos_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0))) + rect.size.h/2;
		}
		else {
			point.x = rect.size.w;
			point.y = rect.size.h - ((rect.size.w/(2.0*(sin_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0)/(cos_lookup(TRIG_MAX_ANGLE * degrees / 360)*1.0))) + rect.size.h/2);
		}
	}
	point.x = point.x - (rect.size.w/2);
	point.y = point.y - (rect.size.h/2);
	return point;
}

//Given a rectangle and a point on the rectangle return the "next" corner (Point must have coordinates relative to rect center.)
int next_rectangle_corner(GRect rect, GPoint point) {
	//Default to "first" corner
	int corner = 0;
	if (point.x == -rect.size.w/2) {
		corner = 3;
	}
	else if (point.x == rect.size.w/2) {
		corner = 1;
	}
	else if (point.y == rect.size.h/2) {
		corner = 2;
	}
#if EAST_TO_WEST_ORB_ROTATION_OLD
	if (corner == 1 || corner == 3) {
		corner = (corner + 2)%4;
	}
#endif
	return corner;
}

//Called if Httpebble is installed on phone.
void have_time(int32_t dst_offset, bool is_dst, uint32_t unixtime, const char* tz_name, void* context) {
  if (!is_dst) {
    timezone = dst_offset/3600.0;
  }
  else {
    timezone = (dst_offset/3600.0) - 1;
  }
 
  //Now that we have timezone get location
  //http_location_request();	
}

//Called if Httpebble is installed on phone.
void have_location(float lat, float lon, float altitude, float accuracy, void* context) {
	latitude = lat;
	longitude = lon;  
  	have_gps_fix = true;
  //Mark sunrise/set layer in need of updating.
  layer_mark_dirty(watch_face_layer);
}

void draw_night_path(GRect rect, GContext *ctx) {

		GPathInfo corner_points = {
			.num_points = 4,
#if EAST_TO_WEST_ORB_ROTATION_OLD
			.points = (GPoint []) {{-rect.size.w/2, -rect.size.h/2}, {-rect.size.w/2, rect.size.h/2}, {rect.size.w/2, rect.size.h/2}, {rect.size.w/2, -rect.size.h/2}}
#else
			.points = (GPoint []) {{rect.size.w/2, -rect.size.h/2}, {rect.size.w/2, rect.size.h/2}, {-rect.size.w/2, rect.size.h/2}, {-rect.size.w/2, -rect.size.h/2}}
#endif
		};

		//Calculate sunrise/sunset
        suntimes = my_suntimes(latitude, longitude, t, timezone, sun_angle);

		int angle_sunup, angle_sundown, angle_diff;

        //Offset by 12 hours so top half is day and bottom half is night.
		//Then get angle (15 degrees per hour + 1 degree per 4 minutes)
		angle_sunup = (15*((suntimes.sunup->tm_hour + 12)%24)) + (suntimes.sunup->tm_min/4);
		angle_sundown = (15*((suntimes.sundown->tm_hour + 12)%24)) + (suntimes.sundown->tm_min/4);
	
#if EAST_TO_WEST_ORB_ROTATION_OLD
        angle_sunup = (360 - angle_sunup);
        angle_sundown = (360 - angle_sundown);
		angle_diff = angle_sundown - angle_sunup;
#else
		angle_diff = angle_sunup - angle_sundown;
#endif	
	
		if (angle_diff < 0) { angle_diff += 360; }
		if (angle_diff > 360) { angle_diff = angle_diff%360; }
	
		night_pattern_points.points[0] = move_by_degrees_rectangle(rect, angle_sunup);
		night_pattern_points.points[1].x = (int16_t)(0);
		night_pattern_points.points[1].y = (int16_t)(0);
		night_pattern_points.points[2] = move_by_degrees_rectangle(rect, angle_sundown);

		//On the same line
		if (angle_diff < 180 && (night_pattern_points.points[0].x == night_pattern_points.points[2].x || night_pattern_points.points[0].y == night_pattern_points.points[2].y)) {
			//set remaining points to existing points
			night_pattern_points.points[3] = move_by_degrees_rectangle(rect, angle_sundown);
			night_pattern_points.points[4] = move_by_degrees_rectangle(rect, angle_sundown);
			night_pattern_points.points[5] = move_by_degrees_rectangle(rect, angle_sunup);
			night_pattern_points.points[6] = move_by_degrees_rectangle(rect, angle_sunup);
		}
		//2 Corners between sunup and sundown
		else if (-night_pattern_points.points[0].x == night_pattern_points.points[2].x || -night_pattern_points.points[0].y == night_pattern_points.points[2].y) {
			int corner = next_rectangle_corner(rect, move_by_degrees_rectangle(rect, angle_sundown));
			night_pattern_points.points[3] = corner_points.points[corner];
			night_pattern_points.points[4] = corner_points.points[corner];
			night_pattern_points.points[5] = corner_points.points[(corner + 1)%4];
			night_pattern_points.points[6] = corner_points.points[(corner + 1)%4];
		}
		//1 Corner between sunup and sundown
		else if (angle_diff < 180) {
			int corner = next_rectangle_corner(rect, night_pattern_points.points[2]);
			night_pattern_points.points[3] = corner_points.points[corner];
			night_pattern_points.points[4] = corner_points.points[corner];
			night_pattern_points.points[5] = corner_points.points[corner];
			night_pattern_points.points[6] = corner_points.points[corner];
		}
		//3 Corners between sunup and sundown
		else {
			int corner = next_rectangle_corner(rect, night_pattern_points.points[2]);
			night_pattern_points.points[3] = corner_points.points[corner];
			night_pattern_points.points[4] = corner_points.points[(corner + 1)%4];
			night_pattern_points.points[5] = corner_points.points[(corner + 2)%4];
			night_pattern_points.points[6] = corner_points.points[(corner + 3)%4];
		}

		night_pattern = gpath_create(&night_pattern_points);
		gpath_move_to(night_pattern, grect_center_point(&rect));
		graphics_context_set_fill_color(ctx, BackgroundColor);
		gpath_draw_filled(ctx, night_pattern);

  		static char sunup_text[] = "00:00";
  		static char sundown_text[] = "00:00";
	    char *time_format;

	    if (clock_is_24h_style()) 
		{
			time_format = "%R";
		}
		else 
		{
			time_format = "%l:%M";
		}
	
		strftime(sunup_text, sizeof(sunup_text), time_format, suntimes.sunup);	
		strftime(sundown_text, sizeof(sundown_text), time_format, suntimes.sundown);	
  
		text_layer_set_text(sunup_layer, sunup_text);
		text_layer_set_text(sundown_layer, sundown_text);
}

void draw_watch_face(Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color(ctx, ForegroundColor);
	graphics_context_set_stroke_color(ctx, BackgroundColor);

	//Main circle for watch face
	GRect layer_rect = layer_get_bounds(layer);
	int layer_radius = 70;
	
	//Only draw Sunlight layer if GPS fix exists
	if (have_gps_fix) {
		//Draw Watch Background
		graphics_fill_circle(ctx, grect_center_point(&layer_rect), layer_radius);

		draw_night_path(layer_rect, ctx);
		graphics_context_set_stroke_color(ctx, ForegroundColor);
		graphics_draw_circle(ctx, grect_center_point(&layer_rect), (layer_radius - 1));
		graphics_draw_circle(ctx, grect_center_point(&layer_rect), (layer_radius));
		graphics_context_set_stroke_color(ctx, BackgroundColor);
	}

#if SHOW_RING_OLD
	int offset = 2;
	if (!have_gps_fix) {
		graphics_context_set_stroke_color(ctx, ForegroundColor);
		offset = 0;
	}
	if (layer_radius > offset) {
		graphics_draw_circle(ctx, grect_center_point(&layer_rect), (layer_radius - offset));
	}
#endif
}
#if SHOW_DATE_OLD
void draw_date() {
	
	static char dom_text[] = "00";
	strftime(dom_text, sizeof(dom_text), "%e", t);	
  
	text_layer_set_text(date_layer, dom_text);
	text_layer_set_text(date_layer_shadow, dom_text);
}
#endif
void draw_orbiting_body(Layer *layer, GContext *ctx) {
	int hour, angle;

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorBlack);

    //Offset by 12 hours so top half is day and bottom half is night.
	hour = (t->tm_hour + 12)%24;

	//Get info about main circle for watch face
	GRect layer_rect = layer_get_bounds(layer);
	GPoint center_point = grect_center_point(&layer_rect);
	int layer_radius, body_radius;

	//This is the radius of watchface.
	layer_radius = 70;
	
	//Draw Orbiting Body
	body_radius = 7;
	
#if LOW_RES_TIME_OLD
	//Rotate orbiting body to proper spot (15 degrees per hour)
	angle = (15*hour);
#else
	int minute;
	minute = t->tm_min;
	//Rotate orbiting body to proper spot (15 degrees per hour + 1 degree per 4 minutes)
	angle = (15*hour) + (minute/4);
#endif
#if EAST_TO_WEST_ORB_ROTATION_OLD
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
	int hour;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

	hour = t->tm_hour%12;

#if LOW_RES_TIME_OLD
	//Rotate hour hand to to proper spot (30 degrees per hour)
	gpath_rotate_to(hour_hand, (TRIG_MAX_ANGLE / 360) * (30*hour));
#else
	int minute;
	minute = t->tm_min;
	//Rotate hour hand to to proper spot (30 degrees per hour + 1 degree per 2 minutes)
	gpath_rotate_to(hour_hand, (TRIG_MAX_ANGLE / 360) * ((30*hour) + (minute/2)));
#endif
	
	gpath_draw_filled(ctx, hour_hand);
	gpath_draw_outline(ctx, hour_hand);
}

void draw_minute_hand(Layer *layer, GContext *ctx) {
	int minute;

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

	minute = t->tm_min;
	

#if LOW_RES_TIME_OLD || !SHOW_SECONDS_OLD
	//Rotate minute hand to to proper spot (6 degrees per minute)
	gpath_rotate_to(minute_hand, (TRIG_MAX_ANGLE / 360) * (6*minute));
#else
	int second;
	second = t->tm_sec;
	//Rotate minute hand to to proper spot (6 degrees per minute + 1 degree per 10 seconds)
	gpath_rotate_to(minute_hand, (TRIG_MAX_ANGLE / 360) * ((6*minute) + (second/10)));
#endif
	
	gpath_draw_filled(ctx, minute_hand);
	gpath_draw_outline(ctx, minute_hand);
}
#if SHOW_SECONDS_OLD
void draw_second_hand(Layer *layer, GContext *ctx) {
	int second;

	graphics_context_set_fill_color(ctx, ForegroundColor);
	graphics_context_set_stroke_color(ctx, ForegroundColor);

	second = t->tm_sec;
	
	//Rotate second hand to to proper spot (6 degrees per second)
	gpath_rotate_to(second_hand_foreground, (TRIG_MAX_ANGLE / 360) * 6 * second);
	
	gpath_draw_filled(ctx, second_hand_foreground);
	gpath_draw_outline(ctx, second_hand_foreground);

	graphics_context_set_fill_color(ctx, BackgroundColor);
	graphics_context_set_stroke_color(ctx, BackgroundColor);

	//Rotate second hand to to proper spot (6 degrees per second)
	gpath_rotate_to(second_hand_background, (TRIG_MAX_ANGLE / 360) * 6 * second);
	
	gpath_draw_filled(ctx, second_hand_background);
	gpath_draw_outline(ctx, second_hand_background);
}
#endif
/* handle_tick is called at every time change. It updates 
   things appropriately*/
void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Handling Tick Event");
	current_time = time(NULL);
	t = localtime(&current_time);
	
	//NOTE: This is a Bit Mask Check not a and &&
	//Secondary Note: units_changed == 0 catches initialzation tick
#if SHOW_DATE_OLD
	if (units_changed == 0 || units_changed & DAY_UNIT) {
        draw_date();
	}
#endif
	
  	if (units_changed == 0 || units_changed & HOUR_UNIT) {
		//http_time_request(); //Update Sunrise/set location & data

//No need to seperatly update hour hand if done by another hand
#if LOW_RES_TIME_OLD
        layer_mark_dirty(hour_layer);
		layer_mark_dirty(orbiting_body_layer);
#endif
	}
#if LOW_RES_TIME_OLD || !SHOW_SECONDS_OLD
	if (units_changed == 0 || units_changed & MINUTE_UNIT) {
        layer_mark_dirty(minute_layer);
#if !LOW_RES_TIME_OLD
		//Only move hour hand every two minutes (due to 2 minutes per degree rotation).
		if (units_changed == 0 || tick_time->tm_min%2 == 0) {
        	layer_mark_dirty(hour_layer);
	  		//Only move orb every 4 minutes (due to 4 minutes per degree rotation)
			if (tick_time->tm_min%4 == 0) {
				layer_mark_dirty(orbiting_body_layer);
			}
		}
  #endif
	}
#endif
#if SHOW_SECONDS_OLD
  	if (units_changed == 0 || units_changed & SECOND_UNIT) {
        layer_mark_dirty(second_layer);
#if !LOW_RES_TIME_OLD
		//Only move minute hand every two seconds (due to 10 seconds per degree rotation).
		if (units_changed == 0 || tick_time->tm_sec%10 == 0) {
        	layer_mark_dirty(minute_layer);
		}
	  	//Only move hour hand every two minutes (due to 2 minutes per degree rotation).
		if (units_changed == 0 || tick_time->tm_min%2 == 0) {
        	layer_mark_dirty(hour_layer);
	  		//Only move orb every 4 minutes (due to 4 minutes per degree rotation)
			if (tick_time->tm_min%4 == 0) {
				layer_mark_dirty(orbiting_body_layer);
			}
		}
#endif
	}
#endif
	//Always redraw the hand pin
	layer_mark_dirty(hand_pin_layer);
}

void handle_init() {
#if !INVERTED_OLD
	BackgroundColor = GColorBlack;
	ForegroundColor = GColorWhite;
#else
	BackgroundColor = GColorWhite;
	ForegroundColor = GColorBlack;
#endif
	load_saved_config_options();

	current_time = time(NULL);
	t = localtime(&current_time);
	
	window = window_create();
	window_stack_push(window, true /*animated */);
    window_set_background_color(window, BackgroundColor);
	
	/* Main Watch Face */
	watch_face_layer = layer_create(GRect(0, 14, 144, 144));
	layer_set_update_proc(watch_face_layer, draw_watch_face);
	layer_add_child(window_get_root_layer(window), watch_face_layer);
#if EAST_TO_WEST_ORB_ROTATION_OLD
	sunup_layer = text_layer_create(GRect((int)(144 - 44), (int)(144), 40, 30));
	text_layer_set_text_alignment(sunup_layer, GTextAlignmentRight);
#else
	sunup_layer = text_layer_create(GRect((int)(4), (int)(144), 40, 30));
	text_layer_set_text_alignment(sunup_layer, GTextAlignmentLeft);
#endif
	text_layer_set_font(sunup_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_color(sunup_layer, ForegroundColor);
	text_layer_set_background_color(sunup_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(sunup_layer));
#if EAST_TO_WEST_ORB_ROTATION_OLD
	sundown_layer = text_layer_create(GRect((int)(4), (int)(144), 40, 30));
	text_layer_set_text_alignment(sundown_layer, GTextAlignmentLeft);
#else
	sundown_layer = text_layer_create(GRect((int)(144 - 44), (int)(144), 40, 30));
	text_layer_set_text_alignment(sundown_layer, GTextAlignmentRight);
#endif
	text_layer_set_font(sundown_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_color(sundown_layer, ForegroundColor);
	text_layer_set_background_color(sundown_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(sundown_layer));

#if SHOW_DATE_OLD
	/* Date */
	date_layer = text_layer_create(GRect((int)(144/2 - 15), (int)(144/2 + 20), 30, 30));
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	text_layer_set_text_color(date_layer, BackgroundColor);
	text_layer_set_background_color(date_layer, GColorClear);
	layer_add_child(watch_face_layer, text_layer_get_layer(date_layer));	
	date_layer_shadow = text_layer_create(GRect((int)(144/2 - 17), (int)(144/2 + 18), 30, 30));
	text_layer_set_font(date_layer_shadow, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(date_layer_shadow, GTextAlignmentCenter);
	text_layer_set_text_color(date_layer_shadow, ForegroundColor);
	text_layer_set_background_color(date_layer_shadow, GColorClear);
	layer_add_child(watch_face_layer, text_layer_get_layer(date_layer_shadow));
	draw_date();
#endif
	
	/* Orbiting Body (aka 24 hour analog clock) */
	orbiting_body_layer = layer_create(layer_get_frame(watch_face_layer));
	layer_set_update_proc(orbiting_body_layer, draw_orbiting_body);
	layer_add_child(watch_face_layer, orbiting_body_layer);
	layer_set_frame(orbiting_body_layer, GRect(0, 0, 144, 144));
	/* Time (aka Clock Hands) */
#if SHOW_SECONDS_OLD
	//Second Hand
	second_layer = layer_create(layer_get_frame(watch_face_layer));
	layer_set_update_proc(second_layer, draw_second_hand);
	layer_add_child(watch_face_layer, second_layer);
	second_hand_foreground = gpath_create(&SECOND_HAND_POINTS_FOREGROUND);
	gpath_move_to(second_hand_foreground, GPoint(72, 58));
	second_hand_background = gpath_create(&SECOND_HAND_POINTS_BACKGROUND);
	gpath_move_to(second_hand_background, GPoint(72, 58));
#endif

	//Minute Hand
	minute_layer = layer_create(layer_get_frame(watch_face_layer));
	layer_set_update_proc(minute_layer, draw_minute_hand);
	layer_add_child(watch_face_layer, minute_layer);
	minute_hand = gpath_create(&MINUTE_HAND_POINTS);
	gpath_move_to(minute_hand, GPoint(72, 58));
	
	//Hour Hand
	hour_layer = layer_create(layer_get_frame(watch_face_layer));
	layer_set_update_proc(hour_layer, draw_hour_hand);
	layer_add_child(watch_face_layer, hour_layer);
	hour_hand = gpath_create(&HOUR_HAND_POINTS);
	gpath_move_to(hour_hand, GPoint(72, 58));
	
	//Hand Pin
	hand_pin_layer = layer_create(layer_get_frame(watch_face_layer));
	layer_set_update_proc(hand_pin_layer, draw_hand_pin);
	layer_add_child(watch_face_layer, hand_pin_layer);
	
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void handle_deinit() {
	//Save config data to local watch storage
//	persist_write_data(CONFIG_LOCATION, &myconfig, sizeof(myconfig));
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved Config information to Watch");

	layer_destroy(hand_pin_layer);
	gpath_destroy(hour_hand);
	layer_destroy(hour_layer);
	gpath_destroy(minute_hand);
	layer_destroy(minute_layer);
	gpath_destroy(second_hand_background);
	gpath_destroy(second_hand_foreground);
	layer_destroy(second_layer);
	text_layer_destroy(date_layer_shadow);
	text_layer_destroy(date_layer);
	text_layer_destroy(sundown_layer);
	text_layer_destroy(sunup_layer);
	gpath_destroy(night_pattern);
	layer_destroy(orbiting_body_layer);
	layer_destroy(watch_face_layer);
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}