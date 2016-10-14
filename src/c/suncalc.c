/*
 * Based on NOAA's Solar Calculator avaliable at 
 * - http://www.esrl.noaa.gov/gmd/grad/solcalc/calcdetails.html
 *
 *	"The sunrise and sunset results are theoretically accurate to within a minute
 *	 for locations between +/- 72° latitude, and within 10 minutes outside of those latitudes.  
 *	 However, due to variations in atmospheric composition, temperature, pressure and conditions, 
 *	 observed values may vary from calculations.
 *	...
 *	Please note that calculations in the spreadsheets are only valid for dates between 1901 and 2099, 
 *	 due to an approximation used in the Julian Day calculation."
 */
#include <pebble.h>
#include "my_math.h"
#include <math.h>
#include "suncalc.h"

double radians(double degrees)
{
    return (M_PI/180.0)*degrees;
}

double degrees(double radians)
{
    return (180.0/M_PI)*radians;
}

struct tm * excel_to_pebble_time(double excel_time)
{
	time_t current_time = time(NULL);
  	struct tm *t = localtime(&current_time);  //Assume current date for time
	int seconds_time;
	excel_time = excel_time - (int)excel_time; //Remove date portion if there is one

	seconds_time = excel_time*24*60*60;
	t->tm_hour = seconds_time/(60*60);	//Hours since midnight.
	t->tm_min = (seconds_time/60)%60;		//Minutes after the hour.
	t->tm_sec = seconds_time%(60);		//Seconds after the minute.
	
	return t;
}

double ha_sunrise_deg_calc(double angle, float lat, double sun_declin_deg) 
{
	return degrees(my_acos(cos(radians(angle))/(cos(radians(lat))*cos(radians(sun_declin_deg))) - tan(radians(lat))*tan(radians(sun_declin_deg))));
}

//angle = 90.833 for sunrise & sunset
times my_suntimes(float lat, float lon, struct tm *time, float timezone, double angle) {
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 1");
	times return_times;
    double julian_day, julian_century, geom_mean_long_sun_deg, geom_mean_anom_sun_deg, eccent_earth_orbit;
	double sun_eq_of_ctr, sun_true_long_deg, sun_app_long_deg, mean_obliq_ecliptic_deg, obliq_corr_deg;
	double y, eq_of_time_minutes, ha_sunrise_deg, solar_noon_LST, sunrise_time_LST, sunset_time_LST;
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 2");

	//Julian date formula from http://en.wikipedia.org/wiki/Julian_day
	int year = time->tm_year + 1900;
	int a = (14 - time->tm_mon)/12; //1 for Jan and Feb, 0 for all other months
        year = year + 4800 - a;
	int month = (time->tm_mon + 1) + (12*a) - 3; //0 for Mar, 11 for Feb
	//Day portion
	julian_day = time->tm_mday + (((153*month) + 2)/5) + (365*year) + (year/4) - (year/100) + (year/400) - 32045;
	//Time portion
	julian_day += ((time->tm_hour - timezone -12)/24.0) + (time->tm_min/1440.0) + (time->tm_sec/86400.0);
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 3");

	julian_century = (julian_day-2451545.0)/36525.0;

	geom_mean_long_sun_deg = (280.46646 + julian_century*(36000.76983 + julian_century*0.0003032));
	//Force geom_mean_long_sun_deg to be within 360 degrees as C does not support decimals in %
    geom_mean_long_sun_deg = 360.0*((geom_mean_long_sun_deg/360.0)-floor(geom_mean_long_sun_deg/360.0));
    if (geom_mean_long_sun_deg < 0) geom_mean_long_sun_deg += 360.0;

APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 4");
	geom_mean_anom_sun_deg = 357.52911 + julian_century*(35999.05029 - 0.0001537*julian_century);
	eccent_earth_orbit = 0.016708634 - julian_century*(0.000042037 + 0.0000001267*julian_century);
	sun_eq_of_ctr = sin(radians(geom_mean_anom_sun_deg))*(1.914602 - julian_century*(0.004817 + 0.000014*julian_century)) + sin(radians(2*geom_mean_anom_sun_deg))*(0.019993 - 0.000101*julian_century) + sin(radians(3*geom_mean_anom_sun_deg))*0.000289;

APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 5");
	sun_true_long_deg = geom_mean_long_sun_deg + sun_eq_of_ctr;
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 5a");
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting juilian_centry = %d.%d", (int)julian_century, (int)(julian_century*1000)%1000);
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting radians = %d.%d", (int)radians(125.04 - 1934.136*julian_century), (int)(radians(125.04 - 1934.136*julian_century)*1000)%1000);
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting radians = %d.%d", (int)sin(radians(125.04 - 1934.136*julian_century)), (int)(sin(radians(125.04 - 1934.136*julian_century))*1000)%1000);
	sun_app_long_deg = sun_true_long_deg - 0.00569 - 0.00478*sin(radians(125.04 - 1934.136*julian_century));
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 5b");
	mean_obliq_ecliptic_deg	= 23 + (26 + ((21.448 - julian_century*(46.815 + julian_century*(0.00059 - julian_century*0.001813))))/60)/60;
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 5c");
	obliq_corr_deg = mean_obliq_ecliptic_deg + 0.00256*cos(radians(125.04 - 1934.136*julian_century));

APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 6");
	return_times.sun_declin_deg = degrees(my_asin(sin(radians(obliq_corr_deg))*sin(radians(sun_app_long_deg))));
	y = tan(radians(obliq_corr_deg/2))*tan(radians(obliq_corr_deg/2));
	eq_of_time_minutes = 4*degrees(y*sin(2*radians(geom_mean_long_sun_deg)) - 2*eccent_earth_orbit*sin(radians(geom_mean_anom_sun_deg)) + 4*eccent_earth_orbit*y*sin(radians(geom_mean_anom_sun_deg))*cos(2*radians(geom_mean_long_sun_deg)) - 0.5*y*y*sin(4*radians(geom_mean_long_sun_deg)) - 1.25*eccent_earth_orbit*eccent_earth_orbit*sin(2*radians(geom_mean_anom_sun_deg)));
	ha_sunrise_deg = ha_sunrise_deg_calc(angle, lat, return_times.sun_declin_deg);
	solar_noon_LST = (720 - 4*lon - eq_of_time_minutes+timezone*60)/1440;

APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 7");
	return_times.solar_noon = excel_to_pebble_time(solar_noon_LST);
	sunrise_time_LST = solar_noon_LST - ha_sunrise_deg*4/1440;
	return_times.sunup = excel_to_pebble_time(sunrise_time_LST);
	sunset_time_LST = solar_noon_LST + ha_sunrise_deg*4/1440;
	return_times.sundown = excel_to_pebble_time(sunset_time_LST);
APP_LOG(APP_LOG_LEVEL_DEBUG, "Problem Hunting 8");

	return return_times;
}

