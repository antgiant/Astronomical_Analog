typedef struct {
	struct tm *solar_noon;
	struct tm *sunup;
	struct tm *sundown;
	double sun_declin_deg;
} times;

times my_suntimes(float lat, float lon, struct tm *time, float timezone, double angle);