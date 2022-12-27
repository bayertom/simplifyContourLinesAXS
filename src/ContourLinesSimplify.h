// Description: Contour line simplification using axial splines
// 
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

#ifndef ContourLinesSimplify_H
#define ContourLinesSimplify_H

#include <memory>
#include <map>

#include "TVector2D.h"
#include "Point3D.h"

//Contour line simplification using potential and minimum energy splines
class ContourLinesSimplify
{
	public:
		static TVector2D < std::shared_ptr <Point3D > >  smoothContourLinesBySplineE(const TVector2D <std::shared_ptr <Point3D > >& contours, std::multimap <double, TVector < std::shared_ptr < Point3D > > >& contour_points_buffers_dz1, std::multimap <double, TVector < std::shared_ptr < Point3D > > >& contour_points_buffers_dz2, 
			const double dh, const unsigned int min_points, const double lambda1, const double lambda2, const int ns, const int d, const bool weighted, const bool scaled);
	private:	
		static TVector2D <std::shared_ptr <Point3D > > splitContourLine (const TVector <std::shared_ptr <Point3D > > &c, const int np);
		static std::tuple<TVector <int>, TVector <int>, TVector <float>, TVector <std::shared_ptr <Point3D > > > findNearestNeighbors(const TVector <std::shared_ptr <Point3D> >& qpoints, const TVector2D <std::shared_ptr <Point3D> >& buffers);
		static std::tuple<int, double, double, double> getNearestLineSegmentPoint(const double xq, const double yq, const TVector <std::shared_ptr <Point3D > >& points);

		
}; 

#include "ContourLinesSimplify.hpp"

#endif