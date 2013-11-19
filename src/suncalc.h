typedef struct {
	PblTm solar_noon;
	PblTm sunup;
	PblTm sundown;
	double sun_declin_deg;
} times;

times my_suntimes(float lat, float lon, PblTm time, float timezone, double angle);