// Description: Export contour lines to DXF file

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


#ifndef DXFExport_H
#define DXFExport_H

#include <memory>

#include "TVector.h"
#include "TVector2D.h"

//Export contour lines to DXF file
class DXFExport
{
        public:
              
		template <typename T>
		static void exportContourLinesToDXF(const std::string &file_name, TVector2D <std::shared_ptr <Point3D > > contours_polylines, const T font_height);

        private:

                static void createHeaderSection (std::ofstream & file);
                static void createTableSection (std::ofstream & file);
                static void endTableSection (std::ofstream & file);
                static void createLayerSection (std::ofstream & file, const std::string &layer_name, const unsigned int color );
                static void createEntitySection (std::ofstream & file);
		static void endHeaderSection(std::ofstream & file);

                template <typename T>
                static void createLine (std::ofstream & file, const std::string &layer_name, const T x1, const T y1, const T z1, const T x2, const T y2, const T z2, const int color);
		
		static void processPolyline(std::ofstream & file, TVector <std::shared_ptr<Point3D > > polyline, const std::string &layer_name, const unsigned int color);

		template <typename T>
		static std::string to_string(const T value, const unsigned short dec_places);

};

#include "DXFExport.hpp"

#endif
