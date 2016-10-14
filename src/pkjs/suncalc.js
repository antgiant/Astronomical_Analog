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

this.exports = {			// create a JSON object to export

//  "value" : 42,			// variable

	radians: function(degrees)
	{
		return (Math.PI/180.0)*degrees;
	},
	
	degrees: function(radians)
	{
		return (180.0/Math.PI)*radians;
	},

	excel_to_pebble_time: function(excel_time)
	{
		var t = new Date();  //Assume current date for time
		excel_time = excel_time - Math.floor(excel_time); //Remove date portion if there is one

		var seconds_time = excel_time*24*60*60;
		t.setHours(seconds_time/(60*60));		//Hours since midnight.
		t.setMinutes((seconds_time/60)%60);		//Minutes after the hour.
		t.setSeconds(seconds_time%(60));		//Seconds after the minute.

		return t;
	},

	ha_sunrise_deg_calc: function(angle, lat, sun_declin_deg) 
	{
		return this.degrees(Math.acos(Math.cos(this.radians(angle))/(Math.cos(this.radians(lat))*Math.cos(this.radians(sun_declin_deg))) - Math.tan(this.radians(lat))*Math.tan(this.radians(sun_declin_deg))));
	},

	//angle = 90.833 for sunrise & sunset
	my_suntimes: function(lat, lon, my_time, timezone, angle) {
		console.log('lat= ' + lat);
		console.log('lon= ' + lon);
		console.log('my_time= ' + my_time);
		console.log('timezone= ' + timezone);
		console.log('angle= ' + angle);

		//Initialize Return Object
		var return_times = {
			sun_declin_deg:-1,
			solar_noon:my_time,
			sunup:my_time,
			sundown:my_time,
		};
		
		//Julian date formula from http://en.wikipedia.org/wiki/Julian_day
		var year = my_time.getFullYear();
		var a = (14 - my_time.getMonth())/12; //1 for Jan and Feb, 0 for all other months
			year = year + 4800 - a;
		var month = (my_time.getMonth() + 1) + (12*a) - 3; //0 for Mar, 11 for Feb
		//Day portion
		var julian_day = my_time.getDate() + (((153*month) + 2)/5) + (365*year) + (year/4) - (year/100) + (year/400) - 32045;
		//Time portion
		julian_day += ((my_time.getHours() - timezone -12)/24.0) + (my_time.getMinutes()/1440.0) + (my_time.getSeconds()/86400.0);

		var julian_century = (julian_day-2451545.0)/36525.0;

		var geom_mean_long_sun_deg = (280.46646 + julian_century*(36000.76983 + julian_century*0.0003032));
		//Force geom_mean_long_sun_deg to be within 360 degrees as C does not support decimals in %
		geom_mean_long_sun_deg = 360.0*((geom_mean_long_sun_deg/360.0)-Math.floor(geom_mean_long_sun_deg/360.0));
		if (geom_mean_long_sun_deg < 0) geom_mean_long_sun_deg += 360.0;
		var geom_mean_anom_sun_deg = 357.52911 + julian_century*(35999.05029 - 0.0001537*julian_century);
		var eccent_earth_orbit = 0.016708634 - julian_century*(0.000042037 + 0.0000001267*julian_century);
		var sun_eq_of_ctr = Math.sin(this.radians(geom_mean_anom_sun_deg))*(1.914602 - julian_century*(0.004817 + 0.000014*julian_century)) + Math.sin(this.radians(2*geom_mean_anom_sun_deg))*(0.019993 - 0.000101*julian_century) + Math.sin(this.radians(3*geom_mean_anom_sun_deg))*0.000289;
		var sun_true_long_deg = geom_mean_long_sun_deg + sun_eq_of_ctr;
		var sun_app_long_deg = sun_true_long_deg - 0.00569 - 0.00478*Math.sin(this.radians(125.04 - 1934.136*julian_century));
		var mean_obliq_ecliptic_deg	= 23 + (26 + ((21.448 - julian_century*(46.815 + julian_century*(0.00059 - julian_century*0.001813))))/60)/60;
		var obliq_corr_deg = mean_obliq_ecliptic_deg + 0.00256*Math.cos(this.radians(125.04 - 1934.136*julian_century));

		return_times.sun_declin_deg = this.degrees(Math.asin(Math.sin(this.radians(obliq_corr_deg))*Math.sin(this.radians(sun_app_long_deg))));
		var y = Math.tan(this.radians(obliq_corr_deg/2))*Math.tan(this.radians(obliq_corr_deg/2));
		var eq_of_time_minutes = 4*this.degrees(y*Math.sin(2*this.radians(geom_mean_long_sun_deg)) - 2*eccent_earth_orbit*Math.sin(this.radians(geom_mean_anom_sun_deg)) + 4*eccent_earth_orbit*y*Math.sin(this.radians(geom_mean_anom_sun_deg))*Math.cos(2*this.radians(geom_mean_long_sun_deg)) - 0.5*y*y*Math.sin(4*this.radians(geom_mean_long_sun_deg)) - 1.25*eccent_earth_orbit*eccent_earth_orbit*Math.sin(2*this.radians(geom_mean_anom_sun_deg)));
		var ha_sunrise_deg = this.ha_sunrise_deg_calc(angle, lat, return_times.sun_declin_deg);
		var solar_noon_LST = (720 - 4*lon - eq_of_time_minutes+timezone*60)/1440;
		return_times.solar_noon = this.excel_to_pebble_time(solar_noon_LST);
		var sunrise_time_LST = solar_noon_LST - ha_sunrise_deg*4/1440;
		return_times.sunup = this.excel_to_pebble_time(sunrise_time_LST);
		var sunset_time_LST = solar_noon_LST + ha_sunrise_deg*4/1440;
		return_times.sundown= this.excel_to_pebble_time(sunset_time_LST);

		return return_times;
	},

};
