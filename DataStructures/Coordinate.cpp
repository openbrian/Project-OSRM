/*

Copyright (c) 2013, Project OSRM, Dennis Luxen, others
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <osrm/Coordinate.h>
#include "../Util/SimpleLogger.h"
#include "../Util/StringUtil.h"

#include <boost/assert.hpp>

#ifndef NDEBUG
#include <bitset>
#endif
#include <iostream>
#include <limits>

FixedPointCoordinate::FixedPointCoordinate()
 :  lat(std::numeric_limits<int>::min()),
    lon(std::numeric_limits<int>::min())
{ }

FixedPointCoordinate::FixedPointCoordinate(int lat, int lon)
 :  lat(lat),
    lon(lon)
{
#ifndef NDEBUG
    if(0 != (std::abs(lat) >> 30))
    {
        std::bitset<32> y(lat);
        SimpleLogger().Write(logDEBUG) << "broken lat: " << lat << ", bits: " << y;
    }
    if(0 != (std::abs(lon) >> 30))
    {
        std::bitset<32> x(lon);
        SimpleLogger().Write(logDEBUG) << "broken lon: " << lon << ", bits: " << x;
    }
#endif
}

void FixedPointCoordinate::Reset() {
    lat = std::numeric_limits<int>::min();
    lon = std::numeric_limits<int>::min();
}
bool FixedPointCoordinate::isSet() const {
    return (std::numeric_limits<int>::min() != lat) &&
           (std::numeric_limits<int>::min() != lon);
}
bool FixedPointCoordinate::isValid() const {
    if (lat >   90*COORDINATE_PRECISION ||
        lat <  -90*COORDINATE_PRECISION ||
        lon >  180*COORDINATE_PRECISION ||
        lon < -180*COORDINATE_PRECISION)
    {
        return false;
    }
    return true;
}
bool FixedPointCoordinate::operator==(const FixedPointCoordinate & other) const {
    return lat == other.lat && lon == other.lon;
}

double FixedPointCoordinate::ApproximateDistance(
    const int lat1,
    const int lon1,
    const int lat2,
    const int lon2
) {
    BOOST_ASSERT(lat1 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lon1 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lat2 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lon2 != std::numeric_limits<int>::min());
    double RAD = 0.017453292519943295769236907684886;
    double lt1 = lat1/COORDINATE_PRECISION;
    double ln1 = lon1/COORDINATE_PRECISION;
    double lt2 = lat2/COORDINATE_PRECISION;
    double ln2 = lon2/COORDINATE_PRECISION;
    double dlat1=lt1*(RAD);

    double dlong1=ln1*(RAD);
    double dlat2=lt2*(RAD);
    double dlong2=ln2*(RAD);

    double dLong=dlong1-dlong2;
    double dLat=dlat1-dlat2;

    double aHarv= pow(sin(dLat/2.0),2.0)+cos(dlat1)*cos(dlat2)*pow(sin(dLong/2.),2);
    double cHarv=2.*atan2(sqrt(aHarv),sqrt(1.0-aHarv));
    //earth radius varies between 6,356.750-6,378.135 km (3,949.901-3,963.189mi)
    //The IUGG value for the equatorial radius is 6378.137 km (3963.19 miles)
    const double earth=6372797.560856;
    return earth*cHarv;
}

double FixedPointCoordinate::ApproximateDistance(
    const FixedPointCoordinate &c1,
    const FixedPointCoordinate &c2
) {
    return ApproximateDistance(c1.lat, c1.lon, c2.lat, c2.lon);
}

double FixedPointCoordinate::ApproximateEuclideanDistance(
    const FixedPointCoordinate &c1,
    const FixedPointCoordinate &c2
) {
    BOOST_ASSERT(c1.lat != std::numeric_limits<int>::min());
    BOOST_ASSERT(c1.lon != std::numeric_limits<int>::min());
    BOOST_ASSERT(c2.lat != std::numeric_limits<int>::min());
    BOOST_ASSERT(c2.lon != std::numeric_limits<int>::min());
    const double RAD = 0.017453292519943295769236907684886;
    const double lat1 = (c1.lat/COORDINATE_PRECISION)*RAD;
    const double lon1 = (c1.lon/COORDINATE_PRECISION)*RAD;
    const double lat2 = (c2.lat/COORDINATE_PRECISION)*RAD;
    const double lon2 = (c2.lon/COORDINATE_PRECISION)*RAD;

    const double x = (lon2-lon1) * cos((lat1+lat2)/2.);
    const double y = (lat2-lat1);
    const double earthRadius = 6372797.560856;
    return sqrt(x*x + y*y) * earthRadius;
}

void FixedPointCoordinate::convertInternalLatLonToString(
    const int value,
    std::string & output
) {
    char buffer[100];
    buffer[11] = 0; // zero termination
    char* string = printInt< 11, 6 >( buffer, value );
    output = string;
}

void FixedPointCoordinate::convertInternalCoordinateToString(
    const FixedPointCoordinate & coord,
    std::string & output
) {
    std::string tmp;
    tmp.reserve(23);
    convertInternalLatLonToString(coord.lon, tmp);
    output = tmp;
    output += ",";
    convertInternalLatLonToString(coord.lat, tmp);
    output += tmp;
}

void FixedPointCoordinate::convertInternalReversedCoordinateToString(
    const FixedPointCoordinate & coord,
    std::string & output
) {
    std::string tmp;
    tmp.reserve(23);
    convertInternalLatLonToString(coord.lat, tmp);
    output = tmp;
    output += ",";
    convertInternalLatLonToString(coord.lon, tmp);
    output += tmp;
}

void FixedPointCoordinate::Output(std::ostream & out) const
{
    out << "(" << lat/COORDINATE_PRECISION << "," << lon/COORDINATE_PRECISION << ")";
}
