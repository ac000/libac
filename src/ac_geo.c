/*
 * ac_geo.c - Geospatial related functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 *
 * See COPYING
 */

#define _GNU_SOURCE

#include <stdlib.h>		/* abs(3) */
#include <math.h>

#include "include/libac.h"

#define DEG_TO_RAD		(M_PI/180)

struct ellipsoid {
	const char *shape;
	/* Equatorial radius in meters */
	const double a;
	/* Polar radius in meters */
	const double b;
	/* Inverse flattening */
	const double f;
};

static const struct ellipsoid ellipsoids[] = {
	{ "WGS84", 6378137.0, 6356752.314245, 298.257223563 },
	{ "GRS80", 6378137.0, 6356752.314140, 298.257222100882711 },
	{ },
};

/**
 * ac_geo_dd_to_dms - convert decimal degrees into degrees, minutes & seconds
 *
 * Note: It is up to the user to track if the value is North/South of the
 *       Equator or East/West of the prime meridian
 *
 * @degrees: The decimal degrees to convert
 * @dms: Filled out with the degrees, minutes & seconds
 */
void ac_geo_dd_to_dms(double degrees, ac_geo_dms_t *dms)
{
	dms->degrees = abs(trunc(degrees));
	dms->minutes = trunc(fmod(degrees * 60, 60.0));
	dms->seconds = fmod(fabs(degrees) * 3600, 60.0);
}

/**
 * ac_geo_dms_to_dd - convert degrees, minutes & seconds into decimal degrees
 *
 * @dms: The degrees, minutes & seconds to convert
 *
 * Note: It is up to the user to track if the value is North/South of the
 *       Equator or East/West of the prime meridian
 *
 * Returns:
 *
 * A double containing the converted decimal degrees
 */
double ac_geo_dms_to_dd(const ac_geo_dms_t *dms)
{
	return dms->degrees + (dms->minutes/60.0) + (dms->seconds/3600.0);
}

/**
 * ac_geo_haversine - calculate the distance between two points on Earth
 *
 * @from: The start latitude and longitude
 * @to: The end latitude and longitude
 *
 * This uses the Haversine formula to calculate the distance between the
 * from and to points
 *
 * https://en.wikipedia.org/wiki/Haversine_formula
 *
 * Returns:
 *
 * A double containing the distance in meters
 */
double ac_geo_haversine(const ac_geo_t *from, const ac_geo_t *to)
{
        double latitude_arc;
        double longitude_arc;
        double latitude_h;
        double lontitude_h;
        double tmp;

        latitude_arc = (from->lat - to->lat) * DEG_TO_RAD;
        longitude_arc = (from->lon - to->lon) * DEG_TO_RAD;
        latitude_h = sin(latitude_arc * 0.5);
        latitude_h *= latitude_h;
        lontitude_h = sin(longitude_arc * 0.5);
        lontitude_h *= lontitude_h;
        tmp = cos(from->lat * DEG_TO_RAD) * cos(to->lat * DEG_TO_RAD);

        return 2.0 * asin(sqrt(latitude_h+tmp * lontitude_h)) *
		ellipsoids[from->ref].a;
}

/**
 * ac_geo_vincenty_direct - given an initial point, bearing and distance
 * 			    calculate the end point
 *
 * @from: The starting latitude and longitude, the bearing and the distance
 * @to: Filled out with the end latitude and longitude and bearing
 *
 * This uses Vincenty's direct formula
 *
 * https://en.wikipedia.org/wiki/Vincenty%27s_formulae#Direct_Problem
 */
void ac_geo_vincenty_direct(const ac_geo_t *from, ac_geo_t *to,
			    double distance)
{
	const double A = ellipsoids[from->ref].a;
	const double B = ellipsoids[from->ref].b;
	const double F = 1 / ellipsoids[from->ref].f;
	double a_squared = A * A;
	double b_squared = B * B;
	double phi1 = from->lat * DEG_TO_RAD;
	double alpha1 = from->bearing * DEG_TO_RAD;
	double cos_alpha1 = cos(alpha1);
	double sin_alpha1 = sin(alpha1);
	double tan_u1 = (1.0 - F) * tan(phi1);
	double cos_u1 = 1.0 / sqrt(1.0 + tan_u1 * tan_u1);
	double sin_u1 = tan_u1 * cos_u1;

	/* eq. 1 */
	double sigma1 = atan2(tan_u1, cos_alpha1);

	/* eq. 2 */
	double sin_alpha = cos_u1 * sin_alpha1;

	double sin2_alpha = sin_alpha * sin_alpha;
	double cos2_alpha = 1 - sin_alpha;
	double u_squared = cos2_alpha * (a_squared - b_squared) / b_squared;

	/* eq. 3 */
	double a = 1 + (u_squared / 16384) * (4096 + u_squared *
			(-768 + u_squared * (320 - 175 * u_squared)));

	/* eq. 4 */
	double b = (u_squared / 1024) * (256 + u_squared *
		    (-128 + u_squared * (74 - 47 * u_squared)));

	/* iterate until there is a negligible change in sigma */
	double s_over_ba = distance / (B * a);
	double sigma = s_over_ba;
	double sin_sigma;
	double prev_sigma = s_over_ba;
	double sigma_m2;
	double cos_sigma_m2;
	double cos2_sigma_m2;

	for ( ; ; ) {
		/* eq. 5 */
		double delta_sigma;

		sigma_m2 = 2.0 * sigma1 + sigma;
		cos_sigma_m2 = cos(sigma_m2);
		cos2_sigma_m2 = cos_sigma_m2 * cos_sigma_m2;
		sin_sigma = sin(sigma);
		double cos_signma = cos(sigma);

		/* eq. 6 */
		delta_sigma = b * sin_sigma * (cos_sigma_m2 + (b / 4.0) *
				(cos_signma * (-1 + 2 * cos2_sigma_m2) -
				 (b / 6.0) * cos_sigma_m2 *
				 (-3 + 4 * sin_sigma * sin_sigma) *
				 (-3 + 4 * cos2_sigma_m2)));

		/* eq. 7 */
		sigma = s_over_ba + delta_sigma;

		/* break after converging to tolerance */
		if (abs(sigma - prev_sigma) < 0.0000000000001)
			break;

		prev_sigma = sigma;
	}

	sigma_m2 = 2.0 * sigma1 + sigma;
	cos_sigma_m2 = cos(sigma_m2);
	cos2_sigma_m2 = cos_sigma_m2 * cos_sigma_m2;

	double cos_sigma = cos(sigma);
	sin_sigma = sin(sigma);

	/* eq. 8 */
	double phi2 = atan2(sin_u1 * cos_sigma + cos_u1 * sin_sigma *
			cos_alpha1, (1.0 - F) *
			sqrt(sin2_alpha + pow(sin_u1 * sin_sigma - cos_u1 *
					      cos_sigma * cos_alpha1, 2.0)));

	/*
	 * eq. 9
	 * This fixes the pole crossing defect spotted by Matt Feemster.
	 * When a path passes a pole and essentially crosses a line of
	 * latitude twice - once in each direction - the longitude
	 * calculation got messed up. Using Atan2 instead of Atan fixes
	 * the defect. The change is in the next 3 lines.
	 * double tanLambda = sinSigma * sinAlpha1 /
	 * (cosU1 * cosSigma - sinU1*sinSigma*cosAlpha1);
	 * double lambda = atan(tanLambda);
	 */
	double lambda = atan2(sin_sigma * sin_alpha1, cos_u1 *
			      cos_sigma - sin_u1 * sin_sigma * cos_alpha1);

	/* eq. 10 */
	double c = (F / 16) * cos2_alpha * (4 + F * (4 - 3 * cos2_alpha));

	/* eq. 11 */
	double l = lambda - (1 - c) * F * sin_alpha *
		(sigma + c * sin_sigma *
		 (cos_sigma_m2 + c *
		  cos_sigma * (-1 + 2 * cos2_sigma_m2)));

	to->lat = phi2 / DEG_TO_RAD;
	to->lon = from->lon + (l / DEG_TO_RAD);

	/* eq. 12 - final bearing */
	to->bearing = atan2(sin_alpha, -sin_u1 * sin_sigma + cos_u1 *
			    cos_sigma * cos_alpha1);

	to->ref = from->ref;
}
