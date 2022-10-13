// Description: Distance point and line

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


#ifndef PointLineDistance_H
#define PointLineDistance_H

//Distance point and line
class PointLineDistance
{
        public:
              
                static double getPointLineDistance2D ( const double xa, const double ya, const double x1, const double y1, const double x2, const double y2, double & xi, double & yi);
                static double getPointLineDistance2DSigned ( const double xa, const double ya, const double x1, const double y1, const double x2, const double y2, double& xi, double& yi);
                static double getPointLineSegmentDistance2D(const double x, const double y, const double x1, const double y1, const double x2, const double y2, double &xi , double &yi);

};

#include "PointLineDistance.hpp"

#endif
