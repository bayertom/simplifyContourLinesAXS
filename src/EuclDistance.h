// Description: Compute Euclidian distance

// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.


#ifndef EuclDistance_H
#define EuclDistance_H

#include <cmath>

#include "Point3D.h"


//Euclidian distance of 2 points
class EuclDistance
{
        public:
                static inline double getEuclDistance ( const double x1, const double y1, const double z1, const double x2, const double y2, const double z2 ) { return sqrt ( ( x2 - x1 ) * ( x2 - x1 ) + ( y2 - y1 ) * ( y2 - y1 ) + ( z2 - z1 ) * ( z2 - z1 ) );}

                static inline double getEuclDistance2D(const double x1, const double y1, const double x2, const double y2) { return getEuclDistance(x1, y1, 0.0, x2, y2, 0.0); }
};

#endif

