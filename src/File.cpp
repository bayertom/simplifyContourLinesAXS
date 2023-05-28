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


#define _CRT_SECURE_NO_WARNINGS

#include "File.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <ctype.h>

#include "WildcardStringMatching.h"
#include "isEqualPointByPlanarCoordinates.h"
#include "Exception.h"
#include "FileReadException.h"
#include "Round.h"


void File::findFilesInDirByMask(const std::string& path, const std::string &mask, const bool full_path, TVector <std::string>& files)
{
        //Find all files containining "mask" in specified dir
        //Wildcards * ? are supported
        for (auto& p : std::filesystem::directory_iterator(path))
        {

                if (WildcardStringMatching::matchStringToPattern(p.path().string(), mask))
                {
                        //Store full path to the file
                        if (full_path)
                                files.push_back(p.path().string());

                        //Store only the file name
                        else
                                files.push_back(p.path().stem().string() + p.path().extension().string());
                }
        }
}


void File::loadPoints(const std::string& file_name, TVector <std::shared_ptr<Point3D > >& nl)
{
	//Load points from file
	std::string line;
	std::ifstream file;

	try
	{
		file.open(file_name);

		//Read line by line
		while (std::getline(file, line))
		{
			char* cline = &line[0u];
			char* item = strtok(cline, " \t");

			//Delimit the row
			std::vector<std::string> row;
			while (item != NULL)
			{
				row.push_back(item);
				item = strtok(NULL, " \t");
			}

			//Add point to the list
			nl.push_back(std::make_shared <Point3D>(std::stod(row[0]), std::stod(row[1]), std::stod(row[2])));
		}
	}

	//Throw exception
	catch (Exception & e)
	{
		throw FileReadException("FileReadException: can not open file. ", file_name);
	}
}


void File::loadContours(const TVector <std::string>& cont_files, TVector2D <std::shared_ptr <Point3D> >& contours_polylines)
{
	//Load contour lines
	for (std::string f : cont_files)
	{
		//Load contour
		TVector < std::shared_ptr <Point3D> > contour_polyline, contour_polyline_t;
		loadPoints(f, contour_polyline);

		//Remove possible duplicate points
		contour_polyline.erase(std::unique(contour_polyline.begin(), contour_polyline.end(), isEqualPointByPlanarCoordinates<std::shared_ptr <Point3D > >()), contour_polyline.end());

		//Add contour to the list
		contours_polylines.push_back(contour_polyline);
	}
}


void File::loadBuffers(const TVector <std::string>& buf_files, std::multimap <double, TVector < std::shared_ptr < Point3D > > >& contour_buffers)
{
	//Load contour line buffer
	for (std::string f : buf_files)
	{
		//Load buffer vertices
		TVector < std::shared_ptr <Point3D > > contour_buffer;
		loadPoints(f, contour_buffer);

		//Remove possible duplicate points
		contour_buffer.erase(std::unique(contour_buffer.begin(), contour_buffer.end(), isEqualPointByPlanarCoordinates<std::shared_ptr <Point3D > >()), contour_buffer.end());

		//Remove short buffers
		//if (contour_buffer.size() > 20)
		{
			//Get z coordinate
			const double z = contour_buffer[0]->getZ();
			const double zr = Round::roundNumber(z, 2);

			//Create buffer
			contour_buffers.insert(std::pair < double, TVector < std::shared_ptr < Point3D > >> (zr, contour_buffer));
		}
	}
}

