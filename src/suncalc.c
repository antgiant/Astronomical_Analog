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
#include "pebble_os.h"
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

PblTm excel_to_pebble_time(double excel_time)
{
	PblTm time;
	int seconds_time;
	get_time(&time);	//Assume current date for time
	excel_time = excel_time - (int)excel_time; //Remove date portion if there is one

	seconds_time = excel_time*24*60*60;
	time.tm_hour = seconds_time/(60*60);	//Hours since midnight.
	time.tm_min = (seconds_time/60)%60;		//Minutes after the hour.
	time.tm_sec = seconds_time%(60);		//Seconds after the minute.
	
	return time;
}

double ha_sunrise_deg_calc(double angle, float lat, double sun_declin_deg) 
{
	return degrees(my_acos(cos(radians(angle))/(cos(radians(lat))*cos(radians(sun_declin_deg))) - tan(radians(lat))*tan(radians(sun_declin_deg))));
}

//angle = 90.833 for sunrise & sunset
times my_suntimes(float lat, float lon, PblTm time, float timezone, double angle) {
	times return_times;
    double julian_day, julian_century, geom_mean_long_sun_deg, geom_mean_anom_sun_deg, eccent_earth_orbit;
	double sun_eq_of_ctr, sun_true_long_deg, sun_app_long_deg, mean_obliq_ecliptic_deg, obliq_corr_deg;
	double y, eq_of_time_minutes, ha_sunrise_deg, solar_noon_LST, sunrise_time_LST, sunset_time_LST;
	int year = time.tm_year + 1900;

    if (time.tm_mon < 3) {
        time.tm_mon += 12;
        year--;
    }

    julian_day = (int)(365.25*(year+4716.0));
	julian_day += (int)(30.6001*(time.tm_mon+1));
	
	//Day in decimal format
	julian_day += (time.tm_mday + (time.tm_hour - timezone + (time.tm_min + (time.tm_sec)/60.0)/60.0)/24.0) - 1524.5;

    if (julian_day > 2299160.0) {
        julian_day += 2 - (int)(year/100);
		julian_day += (int)((int)(year/100)/4);
    }

	julian_century = (julian_day-2451545.0)/36525.0;

	geom_mean_long_sun_deg = (280.46646 + julian_century*(36000.76983 + julian_century*0.0003032));
	//Force geom_mean_long_sun_deg to be within 360 degrees as C does not support decimals in %
    geom_mean_long_sun_deg = 360.0*((geom_mean_long_sun_deg/360.0)-floor(geom_mean_long_sun_deg/360.0));
    if (geom_mean_long_sun_deg < 0) geom_mean_long_sun_deg += 360.0;

	geom_mean_anom_sun_deg = 357.52911 + julian_century*(35999.05029 - 0.0001537*julian_century);
	eccent_earth_orbit = 0.016708634 - julian_century*(0.000042037 + 0.0000001267*julian_century);
	sun_eq_of_ctr = sin(radians(geom_mean_anom_sun_deg))*(1.914602 - julian_century*(0.004817 + 0.000014*julian_century)) + sin(radians(2*geom_mean_anom_sun_deg))*(0.019993 - 0.000101*julian_century) + sin(radians(3*geom_mean_anom_sun_deg))*0.000289;
	sun_true_long_deg = geom_mean_long_sun_deg + sun_eq_of_ctr;
	sun_app_long_deg = sun_true_long_deg - 0.00569 - 0.00478*sin(radians(125.04 - 1934.136*julian_century));
	mean_obliq_ecliptic_deg	= 23 + (26 + ((21.448 - julian_century*(46.815 + julian_century*(0.00059 - julian_century*0.001813))))/60)/60;
	obliq_corr_deg = mean_obliq_ecliptic_deg + 0.00256*cos(radians(125.04 - 1934.136*julian_century));
	return_times.sun_declin_deg = degrees(my_asin(sin(radians(obliq_corr_deg))*sin(radians(sun_app_long_deg))));
	y = tan(radians(obliq_corr_deg/2))*tan(radians(obliq_corr_deg/2));
	eq_of_time_minutes = 4*degrees(y*sin(2*radians(geom_mean_long_sun_deg)) - 2*eccent_earth_orbit*sin(radians(sun_eq_of_ctr)) + 4*eccent_earth_orbit*y*sin(radians(sun_eq_of_ctr))*cos(2*radians(geom_mean_long_sun_deg)) - 0.5*y*y*sin(4*radians(geom_mean_long_sun_deg)) - 1.25*eccent_earth_orbit*eccent_earth_orbit*sin(2*radians(sun_eq_of_ctr)));
	ha_sunrise_deg = ha_sunrise_deg_calc(angle, lat, return_times.sun_declin_deg);
	solar_noon_LST = (720 - 4*lon - eq_of_time_minutes+timezone*60)/1440;
	return_times.solar_noon = excel_to_pebble_time(solar_noon_LST);
	sunrise_time_LST = solar_noon_LST - ha_sunrise_deg*4/1440;
	return_times.sunup = excel_to_pebble_time(sunrise_time_LST);
	sunset_time_LST = solar_noon_LST + ha_sunrise_deg*4/1440;
	return_times.sundown = excel_to_pebble_time(sunset_time_LST);

	return return_times;
}

