// Description: Distance point and line

// Copyright (c) 2021 - 2023
// Tomas Bayer
// Charles University in Prague, Faculty of Science
// bayertom@natur.cuni.cz

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


#ifndef PointLineDistance_HPP
#define PointLineDistance_HPP

#include <cmath>

#include "EuclDistance.h"

#include "MathZeroDevisionException.h"


double PointLineDistance::getPointLineDistance2D ( const double xa, const double ya, const double x1, const double y1, const double x2, const double y2, double &xi, double &yi )
{
        //Compute unsigned distance point and line
        return fabs ( getPointLineDistance2DSigned ( xa, ya, x1, y1, x2, y2, xi, yi ) );
}


double PointLineDistance::getPointLineDistance2DSigned ( const double xa, const double ya, const double x1, const double y1, const double x2, const double y2, double & xi, double & yi)
{
        //Compute signed distance point and line
        //         If distance:
        //		> 0 = point lies in left half plane,
        //		< 0 = point lies in the right half plane,
        //		= 0 = point lies on the line,

        double d12 = sqrt ( ( x2 - x1 ) * ( x2 - x1 ) + ( y2 - y1 ) * ( y2 - y1 ) );

        //Throw exception
        if ( fabs (d12) < MIN_FLOAT )
        {
                //std::cout << "error";
                throw MathZeroDevisionException <double> ( "MathZeroDevisionException: can not compute distance point - line, ", "end points of the line are identical.", d12 );
        }

        const double d =  ( xa * ( y1 - y2 ) + x1 * ( y2 - ya ) + x2 * ( ya - y1 ) ) / d12;

        //Intersection point
        const double da1 = sqrt((xa - x1) * (xa - x1) + (ya - y1) * (ya - y1));
        const double k = sqrt(std::max(da1 * da1 - d * d, 0.0));

        xi = x1 + k * (x2 - x1) / d12;
        yi = y1 + k * (y2 - y1) / d12;

        return d;
}



double PointLineDistance::getPointLineSegmentDistance2D(const double x, const double y, const double x1, const double y1, const double x2, const double y2, double &xi, double &yi)
{
        //Compute distance point and line segment
        const double nx = y1 - y2, ny = x2 - x1;

        //Point p3 is on normal n1: given by p1 and perpendicular to (p1, p2)
        const double x3 = x1 + nx, y3 =  y1 + ny;

        //Point p4 is on normal n2: given by p2 and perpendicular to (p1, p2)
        const double x4 = x2 + nx, y4 = y2 + ny;

        //Position of the point according to the both normals using signed distance
        double xi3, yi3, xi4, yi4;
        const double dist1 = getPointLineDistance2DSigned(x, y, x1, y1, x3, y3, xi3, yi3);
        const double dist2 = getPointLineDistance2DSigned(x, y, x2, y2, x4, y4, xi4, yi4);

        //Point is between both normals n1 and n2
        if ((dist1 < 0) && (dist2 > 0) || (dist1 > 0) && (dist2 < 0))
        {
                return getPointLineDistance2D(x, y, x1, y1, x2, y2, xi, yi);
        }

        //Point is left to the n1
        if (dist1 >= 0)
        {
                xi = x1; yi = y1;
                return EuclDistance::getEuclDistance(x, y, 0.0, x1, y1, 0.0);
        }

        //Point is right to n2
        xi = x2; yi = y2;
        return EuclDistance::getEuclDistance(x, y, 0.0, x2, y2, 0.0);
}


#endif
