    double julian_day, julian_century, geom_mean_long_sun_deg, geom_mean_anom_sun_deg, sun_eq_of_ctr, sun_true_long_deg, sun_app_long_deg;
	int year = time->tm_year + 1900;

    if (time->tm_mon < 3) {
        time->tm_mon += 12;
        year--;
    }

    julian_day = integer(365.25*(year+4716.0)) 
	julian_day += integer(30.6001*(time->tm_mon+1))
	
	//Day in decimal format
	julian_day += (time->tm_mday + (time->tm_hour - timezone + (time->tm_min + (time->tm_sec)/60.0)/60.0)/24.0) - 1524.5;

    if (julian_day > 2299160.0) {
        julian_day += 2 - integer(year/100) 
		julian_day += integer(integer(year/100)/4);
    }

	julian_century = (julian_day-2451545.0)/36525.0
	geom_mean_long_sun_deg = (280.46646+julian_century*(36000.76983 + julian_century*0.0003032))%360
	geom_mean_anom_sun_deg = 357.52911+julian_century*(35999.05029 - 0.0001537*julian_century)
	sun_eq_of_ctr = SIN(RADIANS(geom_mean_anom_sun_deg))*(1.914602-julian_century*(0.004817+0.000014*julian_century))+SIN(RADIANS(2*geom_mean_anom_sun_deg))*(0.019993-0.000101*julian_century)+SIN(RADIANS(3*geom_mean_anom_sun_deg))*0.000289
	sun_true_long_deg = geom_mean_long_sun_deg + sun_eq_of_ctr
	sun_app_long_deg = sun_true_long_deg - 0.00569-0.00478*SIN(RADIANS(125.04-1934.136*julian_century))
Q	Mean Obliq Ecliptic (deg)	23.43792972	=23+(26+((21.448-julian_century*(46.815+julian_century*(0.00059-julian_century*0.001813))))/60)/60
R	Obliq Corr (deg)	23.43848633	=Q2+0.00256*COS(RADIANS(125.04-1934.136*julian_century))
T	Sun Declin (deg)	23.43831216	=DEGREES(ASIN(SIN(RADIANS(R2))*SIN(RADIANS(sun_app_long_deg))))
W	HA Sunrise (deg)	112.6103464	=DEGREES(ACOS(COS(RADIANS(90.833))/(COS(RADIANS(lat))*COS(RADIANS(T2)))-TAN(RADIANS(lat))*TAN(RADIANS(T2))))
Y	Sunrise Time (LST)	5:31:16	=X2-W2*4/1440
Z	Sunset Time (LST)	20:32:09	=X2+W2*4/1440




F	Julian Day	2455368.75	=D2+2415018.5+E2-timezone/24
G	Julian Century	0.10468868	=(double)(F2-2451545.0)/36525.0
I	Geom Mean Long Sun (deg)	89.33966362	=MOD(280.46646+G2*(36000.76983 + G2*0.0003032),360)
J	Geom Mean Anom Sun (deg)	4126.222292	=357.52911+G2*(35999.05029 - 0.0001537*G2)
K	Eccent Earth Orbit	0.016704232	=0.016708634-G2*(0.000042037+0.0000001267*G2)
L	Sun Eq of Ctr	0.446799918	=SIN(RADIANS(J2))*(1.914602-G2*(0.004817+0.000014*G2))+SIN(RADIANS(2*J2))*(0.019993-0.000101*G2)+SIN(RADIANS(3*J2))*0.000289
M	Sun True Long (deg)	89.78646353	=I2+L2
N	Sun True Anom (deg)	4126.669092	=J2+L2
O	Sun Rad Vector (AUs)	1.016240085	=(1.000001018*(1-K2*K2))/(1+K2*COS(RADIANS(N2)))
P	Sun App Long (deg)	89.78543918	=M2-0.00569-0.00478*SIN(RADIANS(125.04-1934.136*G2))
Q	Mean Obliq Ecliptic (deg)	23.43792972	=23+(26+((21.448-G2*(46.815+G2*(0.00059-G2*0.001813))))/60)/60
R	Obliq Corr (deg)	23.43848633	=Q2+0.00256*COS(RADIANS(125.04-1934.136*G2))
S	Sun Rt Ascen (deg)	89.7661433	=DEGREES(ATAN2(COS(RADIANS(P2)),COS(RADIANS(R2))*SIN(RADIANS(P2))))
T	Sun Declin (deg)	23.43831216	=DEGREES(ASIN(SIN(RADIANS(R2))*SIN(RADIANS(P2))))
U	var y	0.04303149	=TAN(RADIANS(R2/2))*TAN(RADIANS(R2/2))
V	Eq of Time (minutes)	-1.706307841	=4*DEGREES(U2*SIN(2*RADIANS(I2))-2*K2*SIN(RADIANS(J2))+4*K2*U2*SIN(RADIANS(J2))*COS(2*RADIANS(I2))-0.5*U2*U2*SIN(4*RADIANS(I2))-1.25*K2*K2*SIN(2*RADIANS(J2)))
W	HA Sunrise (deg)	112.6103464	=DEGREES(ACOS(COS(RADIANS(90.833))/(COS(RADIANS(lat))*COS(RADIANS(T2)))-TAN(RADIANS(lat))*TAN(RADIANS(T2))))
X	Solar Noon (LST)	13:01:42	=(720-4*lon-V2+timezone*60)/1440
Y	Sunrise Time (LST)	5:31:16	=X2-W2*4/1440
Z	Sunset Time (LST)	20:32:09	=X2+W2*4/1440