// Description: Contour line simplification using minimum energy splines
// Main cpp file: read input files (contour lines + buffers), simplify, and export simplified contour lines

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
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <format>
#include <string>
#include <filesystem>

#include "Exception.h"
#include "TVector.h"
#include "TVector2D.h"
#include "File.h"
#include "ContourLinesSimplify.h"
#include "DXFExport.h"
#include "SplineSmoothing.h"


int main(int argc, char* argv[])
{
	//Initial parameters of the contour lines and the simplification
	bool weighted = false, scaled = false;
	int min_points = 20, k = 2, ns = 2000;
	double z_min = 0.0, z_max = 1000.0, dh = 0.20;
	double lambda1 = 6000.0, lambda2 = 2.0;
	
	//Path to the folder
	//std::filesystem::current_path("..//results//");
	//std::string path = "E:\\Tomas\\CPP\\simplifyContourLinesEMS\\dtm_test\\smoothing_ems_clip\\";
	//std::string path = "E:\\Tomas\\CPP\\simplifyContourLinesEMS\\smoothing_test\\";
	//std::string path = "E://Tomas//CPP//simplifyContourLinesEMS//dtm_test//smoothing_iaxs_clip//";
	std::string path = std::filesystem::current_path().generic_string();

	//Countour line file mask
	std::string contours_file_mask = "*contour_lines*.csv";

	//Buffers z-dz and z+dz file masks
	std::string buff1_file_mask = "*buffer_B1*.csv";			  //All vertical buffers z-dz
	std::string buff2_file_mask = "*buffer_B2*.csv";		          //All vertical buffers z+dz

	//Output file name
	std::string output_file_name = "contours.xyz";

	//Process command-line argument:
	while (--argc > 0)
	{

		//Get - (A parameter follows)
		if (*argv[argc] == '-')
		{
			//Process parameter after -
			for (char* parameter = argv[argc]; ; )
			{
				switch (*(++parameter))
				{
					//Set weighted version
					case 'w':
					{
						weighted = true;
						break;
					}

					//Set scaled version
					case 's':
					{
						scaled = true;
						break;
					}

					//Terminate character \0 of the argument
					case '\0':
						break;

						//Throw exception
					default:
						throw Exception("Exception: Invalid parameter in command line!");
				}

				//Stop processing of the argument
				break;
			}
		}

		//Set values
		else if (*argv[argc] == '+')
		{
			//Get new command line parameter
			char* attribute = const_cast <char*> (argv[argc] + 1), * value = NULL;
			char* command = attribute;

			//Find splitter: =
			for (; (*command != '=') && (*command != '\0'); command++);

			//We found splitter, trim command and copy to value
			if ((*command == '=') && (*command != '\0'))
			{
				*command = '\0';
				value = command + 1;
			}

			//Throw exception
			if (attribute == NULL || value == NULL)
				throw Exception("Exception: Invalid value in command line!");

			//Set z_step
			if (!strcmp("dh", attribute))
			{
				dh = std::max(std::min(atof(value), 1.0), 0.0);
			}

			//Set lambda1
			else if (!strcmp("lambda1", attribute))
			{
				lambda1 = std::max(std::min(atof(value), 1000000.0), 0.0);
			}

			//Set lambda2
			else if (!strcmp("lambda2", attribute))
			{
				lambda2 = std::max(std::min(atof(value), 100000.0), 0.0);
			}

			//Set smoothing step
			else if (!strcmp("k", attribute))
			{
				k = std::max(std::min(atoi(value), 5), 1);
			}

			//Set amount of vertices
			else if (!strcmp("ns", attribute))
			{
				ns = std::max(std::min(atoi(value), 10000), 500);
			}

			//Set buffer 1 file
			else if (!strcmp("buff1", attribute))
			{
				buff1_file_mask = value;
			}

			//Set buffer 2 file
			else if (!strcmp("buff2", attribute))
			{
				buff2_file_mask = value;
			}

			//Set contour file
			else if (!strcmp("cont", attribute))
			{
				contours_file_mask = value;
			}

			//Set file name
			else if (!strcmp("file", attribute))
			{
				output_file_name = value;
			}

			//Set path
			else if (!strcmp("path", attribute))
			{
				path = value;
			}

			//Bad argument
			else
			{
				std::cout << attribute << '\n';
				throw Exception("Exception: Invalid attribute in command line!");
			}
		}

		//Process file
		else
		{
			//contours_file_name = argv[argc];
		}
	}

	//Write parameters
	std::cout << "*** SIMPLIFY CONTOUR LINES, AXIAL SPLINES ***\n\n";
	std::cout << ">>> Input parameters: \n" <<
		"  Buffer height = " << dh << '\n' <<
		"  Lambda1 = " << lambda1 << '\n' <<
		"  Lambda2 = " << lambda2 << '\n' <<
		"  Processed vertices = " << ns << '\n' <<
		"  Smoothing order = " << k << '\n' <<
		"  Weighted = " << weighted << '\n' <<
		"  Scaled = " << scaled << '\n' <<
		"  Contour mask =" << contours_file_mask << '\n' <<
		"  Buffer 1 mask = " << buff1_file_mask << '\n' <<
		"  Buffer 2 mask = " << buff2_file_mask << '\n' <<
		"  Output file = " << output_file_name << '\n' <<
		"  Path = " << path << '\n' << "\n";

	//Find contour line files
	std::cout << ">>> Read input files: ";
	TVector <std::string> cont_files;
	File::findFilesInDirByMask(path, contours_file_mask, 1, cont_files);

	//Load contours one by one
	TVector2D <std::shared_ptr <Point3D > > contours_polylines;
	File::loadContours(cont_files, contours_polylines);

	//Find buffer 1 files
	TVector <std::string> buff1_files;
	File::findFilesInDirByMask(path, buff1_file_mask, 1, buff1_files);

	//Load first buffer one by one
	std::multimap <double, TVector < std::shared_ptr < Point3D > > > contour_buffers1;
	File::loadBuffers(buff1_files, contour_buffers1);

	//Find buffer 2 files
	TVector <std::string> buff2_files;
	File::findFilesInDirByMask(path, buff2_file_mask, 1, buff2_files);

	//Load second buffer one by one
	std::multimap <double, TVector < std::shared_ptr < Point3D > > > contour_buffers2;
	File::loadBuffers(buff2_files, contour_buffers2);
	std::cout << "OK \n";

	//Simplify contour lines
	try
	{
		//Patial displacement with axial spline
		TVector2D <std::shared_ptr <Point3D > > contours_polylines_smooth = ContourLinesSimplify::smoothContourLinesBySplineE(contours_polylines, contour_buffers1, contour_buffers2, dh, min_points,lambda1, lambda2, ns, k, weighted, scaled);

		//Export simplified contour lines to DXF
		std::string file_name_simp = "results_" + output_file_name + "_simp_dh_" + std::format("{:.2f}", dh) + "_lambda1_"
			+ std::format("{:.2f}", lambda1) + "_lambda2_" + std::format("{:.2f}", lambda2) + "_ns_"
			+ std::format("{:1}", int(ns)) + +"_k_" + std::format("{:1}", int(k)) + "_weighted_" 
			+ std::format("{:1}", int(weighted)) + "_scaled_" + std::format("{:1}", int(scaled)) + ".dxf";
		
		DXFExport::exportContourLinesToDXF(file_name_simp, contours_polylines_smooth, 10.0);
	}

	//Throw exception
	catch (Exception& e)
	{
		e.printException();
	}


	std::cout << "Hello CMake." << std::endl;
	return 0;
}