// Description: 3D Point

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
// along with this library. If not, see <http://www.gnu.org/licenses/>

#ifndef Point3D_H
#define Point3D_H

//3D Point
class Point3D
{
	private:
		static unsigned int points_cart_id_counter;	//Static variable: counter of created points
		unsigned int point_id;				//Internal point ID (start from 0)
		double x;					//Point x coordinate
		double y;					//Point y coordinate
		double z;					//Point z coordinate
	
	public:
		Point3D() : point_id(0), x(0), y(0), z(0) {}
		Point3D(const double& x_, const double& y_) : point_id(points_cart_id_counter++), x(x_), y(y_), z(0.0) {}
		Point3D(const double& x_, const double& y_, const double& z_) : point_id(points_cart_id_counter++), x(x_), y(y_), z(z_) {}
		bool operator == (const Point3D& p) const
		{
			return (x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) < 0.001;
		}

		double getX() const { return x; }
		double getY() const { return y; }
		double getZ() const { return z; }

		void setX(const double x_) { x = x_; }
		void setY(const double y_) { y = y_; }
		void setZ(const double z_) { z = z_; }
};

#endif