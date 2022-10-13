// Description: Input file operations, load text files

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


#ifndef File_H
#define File_H

#include <string>
#include <memory>
#include <map>

#include "TVector.h"
#include "TVector2D.h"
#include "Point3D.h"

//Input file operations, load text files
class File
{
        public:
		static void findFilesInDirByMask(const std::string& path, const std::string &mask, const bool full_path, TVector <std::string>& files);
		static void loadPoints(const std::string& file_name, TVector <std::shared_ptr<Point3D > >& nl);
		static void loadContours(const TVector <std::string>& cont_files, TVector2D <std::shared_ptr <Point3D> >& contours_polylines);
		static void loadBuffers(const TVector <std::string>& buf_files, std::multimap <double, TVector < std::shared_ptr < Point3D > > >& contour_buffers);

};

#endif
