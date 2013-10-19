#ifndef GPS_FUNCTIONS_H
#define GPS_FUNCTIONS_H

#include "messages/sensors.pb.h"

namespace asimov
{
/************************
*Thank you to MrTJ of
*stackoverflow
*************************/

#include <cmath> 
#define pi 3.14159265358979323846
#define earthRadiusKm 6371.0

// This function converts decimal degrees to radians
inline double deg2rad(double deg) {
  return (deg * pi / 180);
};

//  This function converts radians to decimal degrees
inline double rad2deg(double rad) {
  return (rad * 180 / pi);
};

/**
 * Returns the distance between two points on the Earth.
 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 * @param lat1d Latitude of the first point in degrees
 * @param lon1d Longitude of the first point in degrees
 * @param lat2d Latitude of the second point in degrees
 * @param lon2d Longitude of the second point in degrees
 * @return The distance between the two points in kilometers
 */
inline double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
  double lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(lat1d);
  lon1r = deg2rad(lon1d);
  lat2r = deg2rad(lat2d);
  lon2r = deg2rad(lon2d);
  u = sin(lat2r - lat1r);
  v = sin(lon2r - lon1r);
  return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
};

inline msg_Vector3  Distance_GPS( msg_GPS &origin, msg_GPS &relative )
{
   msg_Vector3 result;
   double X = distanceEarth( origin.latitude(), origin.longitude(), origin.latitude(), relative.longitude() );
   double Y = distanceEarth( origin.latitude(), origin.longitude(), relative.latitude(), origin.longitude() );
   double Z = relative.altitude() - origin.altitude();
   result.set_x( X );
   result.set_y( Y );
   result.set_z( Z );
   return result;
}

};
#endif

