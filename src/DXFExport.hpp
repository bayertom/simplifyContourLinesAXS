// Description: Export lines, points, polygons to 2D/3D DXF file

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



#ifndef DXFExport_HPP
#define DXFExport_HPP

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sstream>


#include "Const.h"
#include "Point3D.h"
#include "FileWriteException.h"


template <typename T>
void DXFExport::exportContourLinesToDXF(const std::string &file_name, TVector2D <std::shared_ptr <Point3D> > contours_polylines, const T font_height)
{
	//Export contour lines given to DXF file
	const unsigned int color_cont = 1, color_cont_points = 5;
	const std::string level_cont = "contour_lines", level_cont_points = "contour_lines_points", level_points_labels = "dt_points_labels";

	std::ofstream file;

	try
	{
		file.open(file_name/*, ios::out*/);

		if (file.is_open())
		{
			//Create header section
			createHeaderSection(file);

			//Create table section
			createTableSection(file);

			//Create layer for contour lines
			createLayerSection(file, level_cont, color_cont);

			//Create layer for dt contour lines labels
			createLayerSection(file, level_cont_points, color_cont_points);

			//End table header
			endTableSection(file);

			//Create entity section
			createEntitySection(file);

			//Process contour lines
			for (TVector <std::shared_ptr<Point3D > > polyline : contours_polylines)
			{
				processPolyline(file, polyline, level_cont, color_cont);
			}

			//End header section
			endHeaderSection(file);

			//Close file
			file.close();
		}

		//Throw exception
		else
		{
			//Can not open file
			throw std::ios_base::failure("Exception: can not open the file. ");
		}
	}

	//Any error has appeared
	catch (std::ios_base::failure &)
	{
		//Close file
		file.close();

		//Throw exception
		throw FileWriteException("FileWriteException: can not write the file: ", file_name);
	}
}


inline void DXFExport::createHeaderSection ( std::ofstream & file )
{
        //Create header section
	const std::string object_type_id = "0\n";
	const std::string object_type = "SECTION\n";
	const std::string header_id = "2\n";
	const std::string header_name = "HEADER\n";
	const std::string variable_name = "9\n";
	const std::string acad_version = "$ACADVER\n";
	const std::string acad_version_id =  "1\n";
	const std::string acad_version_val = "AC1006\n";
	const std::string section_terminator = "ENDSEC\n";
        
	/* Add to file */
	file << object_type_id;
        file << object_type;
        file << header_id;
        file << header_name;
        file << variable_name;
        file << acad_version;
        file << acad_version_id;
        file << acad_version_val;
        file << object_type_id;
        file << section_terminator;
}


inline void DXFExport::endHeaderSection (std::ofstream & file)
{
        //Create end of the header section
	const std::string entity_id = "0\n";
	const std::string section_terminator ="ENDSEC\n";
	const std::string file_terminator = "EOF\n";
	
	/* Add to file */
	file << entity_id;
        file << section_terminator;
        file << entity_id;
        file << file_terminator;
}


inline void DXFExport::createTableSection (std::ofstream & file )
{
        //Create table section
	const std::string object_type_id = "0\n";
	const std::string object_name = "SECTION\n";
	const std::string table_id = "2\n";
	const std::string table = "TABLES\n";
	const std::string table_name = "TABLE\n";
	const std::string layer_name = "LAYER\n";
	const std::string max_number_of_entries_id = "70\n";
	const std::string max_number_of_entries = "0\n";

        file << object_type_id;
        file << object_name;
        file << table_id;
        file << table;
        file << object_type_id;
        file << table_name;
	file << table_id;
        file << layer_name;
        file << max_number_of_entries_id;
        file << max_number_of_entries;
}


inline void DXFExport::endTableSection ( std::ofstream & file )
{
        //Write end of the table section
	const std::string object_type_id = "0\n";
	const std::string table_end_name = "ENDTAB\n";
	const std::string section_end_name = "ENDSEC\n";

	/* Add to file */
        file << object_type_id;
        file << table_end_name;
        file << object_type_id;
        file << section_end_name;
}


inline void DXFExport::createLayerSection (std::ofstream & file, const std::string &layer_name, const unsigned int color )
{
        //Add section for one layer
	const std::string object_type_id = "0\n";
	const std::string object_name = "LAYER\n";
	const std::string layer_name_id = "2\n";
	const std::string layer_flag_id = "70\n";
	const std::string layer_flag = "0\n";
	const std::string color_number_id = "62\n";
	const std::string line_type_id = "6\n";
	const std::string line_type_name = "CONTINUOUS\n";

	/* Add to file */
        file << object_type_id;
        file << object_name;
        file << layer_name_id;
	file << layer_name << '\n';
        file << layer_flag_id;
        file << layer_flag;
        file << color_number_id;
        file << color << '\n';
        file << line_type_id;
        file << line_type_name;
}


inline void DXFExport::createEntitySection ( std::ofstream & file )
{
        //Create section for entities
	const std::string object_type_id = "0\n";
	const std::string object_name = "SECTION\n";
	const std::string entity_name_id = "2\n";
	const std::string entity_name = "ENTITIES\n";

	/* Add to file */
        file << object_type_id;
        file << object_name;
        file << entity_name_id;
        file << entity_name;
}


template <typename T>
void DXFExport::createLine (std::ofstream & file, const std::string &layer_name, const T x1, const T y1, const T z1, const T x2, const T y2, const T z2, const int color)
{
        //Write line to DXF file
	const std::string entity_id = "0\n";
	const std::string entity_name = "LINE\n";
	const std::string level_id = "8\n";
	const std::string color_id = "62\n";
	const std::string xi_id = "10\n";
	const std::string yi_id = "20\n";
	const std::string zi_id = "30\n";
	const std::string xii_id = "11\n";
	const std::string yii_id = "21\n";
	const std::string zii_id = "31\n";

	//Set format and precision
	file << std::fixed << std::setprecision(3);

        /* Add to file */
        file << entity_id;
        file << entity_name;
        file << level_id;
	file << layer_name << '\n';
	file << color_id;
	file << color << '\n';
        file << xi_id;
        file << x1 << '\n';
	file << yi_id;
	file << y1 << '\n';
        file << zi_id;
        file << z1 << '\n';
        file << xii_id;
        file << x2 << '\n';
        file << yii_id;
        file << y2 << '\n';
        file << zii_id;
        file << z2 << '\n';
}


void DXFExport::processPolyline(std::ofstream & file, TVector <std::shared_ptr<Point3D > > polyline, const std::string &layer_name, const unsigned int color)
{
	//Process polyline
	const unsigned int n = polyline.size();

	//Process halfedges one by one
	for (unsigned int i = 0; i < n - 1; i++)
	{
		// Get start point
		const double x1 = polyline[i]->getX();
		const double y1 = polyline[i]->getY();
		const double z1 = polyline[i]->getZ();

		// Get end point
		const double x2 = polyline[i+1]->getX();
		const double y2 = polyline[i+1]->getY();
		const double z2 = polyline[i+1]->getZ();

		//Create line
		createLine(file, layer_name, x1, y1, z1, x2, y2, z2, color);
	}
}


template <typename T>
std::string DXFExport::to_string(const T value, const unsigned short dec_places)
{
	//Convert to string,  set the decimal places
	std::ostringstream out;
	out << std::setprecision(dec_places) << value;
	return out.str();
}

#endif
