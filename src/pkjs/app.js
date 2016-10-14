var SunTimes = require('./suncalc.js');
var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

function locationSuccess(pos) {
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
	// Assemble dictionary using our keys
	var dictionary = {
		'latitude': Math.round(pos.coords.latitude*10000),
		'longitude': Math.round(pos.coords.longitude*10000)
	};

	// Send to Pebble
	Pebble.sendAppMessage(dictionary,
						  function(e) {
							  console.log('Location sent to Pebble successfully!');
						  },
						  function(e) {
							  console.log('Error sending location to Pebble!');
						  }
						 );
	var temp_date = new Date();
	var suntime = SunTimes.my_suntimes(pos.coords.latitude, pos.coords.longitude, temp_date, temp_date.getTimeZoneOffset(), 90.833);
	
	console.log('sun_declin_deg: ' + suntime.sun_declin_deg);
	console.log('solar_noon: ' + suntime.solar_noon);
	console.log('sunup: ' + suntime.sunup);
	console.log('sundown: ' + suntime.sundown);
	
}

function locationError(err) {
  if(err.code == err.PERMISSION_DENIED) {
    console.log('Location access was denied by the user.');  
  } else {
    console.log('location error (' + err.code + '): ' + err.message);
  }
}

function getLocation() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {enableHighAccuracy: false, timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');

    // Get the initial Location
    getLocation();
  }
);