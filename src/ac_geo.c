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
#define RAD_TO_DEG		(180/M_PI)

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
	{ "AIRY1830", 6377563.396, 6356256.909, 299.3249646 },
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

#define LAM0		(-2.0*DEG_TO_RAD)
#define PHI0		(49.0*DEG_TO_RAD)
#define E0		400000
#define N0		(-100000)
#define F0		0.9996012717
/**
 * ac_geo_bng_to_lat_lon - convert British National Grid Eastings & Northings
 * 			   to latitude & longitude decimal degrees
 *
 * @geo - Contains the Eastings & Northings (in meters) and an optional
 * 	  altitude to be converted. It should also specify the target
 * 	  ellipsoid to map the converted co-ordinates onto.
 *
 * 	  It is then filled out with the latitude & longitude decimal degrees
 * 	  and the new altitude
 *
 * This function is largely based on Python code from Dr Hannah Fry.
 * http://www.hannahfry.co.uk/blog/2012/02/01/converting-british-national-grid-to-latitude-and-longitude-ii
 */
void ac_geo_bng_to_lat_lon(ac_geo_t *geo)
{
	const double E = geo->easting;
	const double N = geo->northing;
	double a = ellipsoids[AC_GEO_EREF_AIRY1830].a;
	double b = ellipsoids[AC_GEO_EREF_AIRY1830].b;
	double e2 = 1 - (b*b) / (a*a);
	double n = (a-b) / (a+b);
	double dN = N - N0;
	double phi = PHI0;
	double M = 0;

	while (dN-M >= 0.00001) {
		phi += (dN-M) / (a*F0);
		double Ma = (1.0 + n + (5.0/4)*pow(n, 2.0) + (5.0/4) *
			     pow(n, 3.0)) *(phi-PHI0);
		double Mb = (3.0*n + 3.0*pow(n, 2.0) + (21.0/8)*pow(n, 3.0)) *
			sin(phi-PHI0) * cos(phi+PHI0);
		double Mc = ((15.0/8)*pow(n, 2.0) + (15.0/8)*pow(n, 3.0)) *
			sin(2*(phi-PHI0)) * cos(2*(phi+PHI0));
		double Md = (35.0/24)*pow(n, 3.0) * sin(3*(phi-PHI0)) *
			cos(3*(phi+PHI0));

		M = b * F0 * (Ma-Mb + Mc-Md);
	}

	double nu = a * F0 / sqrt(1-e2*pow(sin(phi), 2.0));
	double rho = a * F0 * (1.0-e2) * pow((1-e2*pow(sin(phi), 2.0)), -1.5);
	double eta2 = nu / rho - 1.0;

	double tp = tan(phi);
	double tp2 = tp*tp;
	double tp4 = tp2*tp2;
	double tp6 = tp4*tp2;
	double sp = 1.0 / cos(phi);

	double VII = tp / (2 * rho*nu);
	double VIII = tp / (24*rho * pow(nu, 3.0)) *
		(5+3*tp2+eta2 - 9*tp2*eta2);
	double IX = tp / (720*rho * pow(nu, 5.0)) * (61 + 90*tp2 + 45*tp4);
	double X = sp / nu;
	double XI = sp / (6 * pow(nu, 3.0)) * (nu/rho + 2*tp2);
	double XII = sp / (120 * pow(nu, 5.0)) * (5 + 28*tp2 + 24*tp4);
	double XIIA = sp / (5040 * pow(nu, 7.0)) *
		(61 + 662*tp2 + 1320*tp4 + 720*tp6);
	double dE = E - E0;

	phi = phi - VII*pow(dE, 2.0) + VIII*pow(dE, 4.0) - IX*pow(dE, 6.0);
	double lam = LAM0 + X*dE - XI*pow(dE, 3.0) + XII*pow(dE, 5.0) -
		XIIA*pow(dE, 7.0);

	/* That gives us co-ordinates on the Airy 1830 ellipsoid */

	/* Convert to Cartesian from spherical polar coordinates */
	double H = geo->alt;
	double x_1 = (nu/F0 + H)*cos(phi)*cos(lam);
	double y_1 = (nu/F0 + H)*cos(phi)*sin(lam);
	double z_1 = ((1-e2)*nu/F0 + H)*sin(phi);

	/* Perform Helmut transform to go between Airy 1830 and geo->ref */
	double s = -20.4894*pow(10, -6.0);
	double tx = 446.448, ty = -125.157, tz = 542.060;
	double rxs = 0.1502, rys = 0.2470, rzs = 0.8421;
	double rx = rxs*M_PI / (180*3600.0);
	double ry = rys*M_PI / (180*3600.0);
	double rz = rzs*M_PI / (180*3600.0);
	double x_2 = tx + (1+s)*x_1 + (-rz)*y_1 + (ry)*z_1;
	double y_2 = ty + (rz)*x_1 + (1+s)*y_1 + (-rx)*z_1;
	double z_2 = tz + (-ry)*x_1 + (rx)*y_1 + (1+s)*z_1;

	/* Back to spherical polar coordinates from Cartesian */
	a = ellipsoids[geo->ref].a;
	b = ellipsoids[geo->ref].b;
	e2 = 1 - (b*b) / (a*a);
	double p = sqrt(pow(x_2, 2.0) + pow(y_2, 2.0));

	phi = atan2(z_2, p*(1-e2));
	double phiold = 2 * M_PI;
	while (abs(phi-phiold) > pow(10, -16.0)) {
		phiold = phi;
		nu = a/sqrt(1-e2*pow(sin(phi), 2.0));
		phi = atan2(z_2+e2*nu*sin(phi), p);
	}
	lam = atan2(y_2, x_2);

	geo->alt = p/cos(phi) - nu;
	geo->lat = phi * RAD_TO_DEG;
	geo->lon = lam * RAD_TO_DEG;
}
