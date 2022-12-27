// Description: Contour line simplification using potential and minimum energy splines

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


#ifndef ContourLinesSimplify_HPP
#define ContourLinesSimplify_HPP

#include <memory>
#include <map>
#include <Eigen/Dense>                               
#include <Eigen/Sparse>                              
#include <Eigen/Core>

#include "TVector.h"
#include "TVector2D.h"
#include "Point3D.h"
#include "Const.h"
#include "Round.h"
#include "PointLineDistance.h"
#include "SplineSmoothing.h"

TVector2D < std::shared_ptr <Point3D > > ContourLinesSimplify::smoothContourLinesBySplineE(const TVector2D <std::shared_ptr <Point3D > >& contours, std::multimap <double, TVector < std::shared_ptr < Point3D > > >& contour_points_buffers_dh1, std::multimap <double, TVector < std::shared_ptr < Point3D > > >& contour_points_buffers_dh2, const double dh, const unsigned int min_points, const double lambda1, const double lambda2, const int ns, const int k, const bool weighted, const bool scaled)
{
	//Simplify contour lines inside the corridor using the spline (Eigen version)
	TVector2D <std::shared_ptr <Point3D > > contours_smoothed;

	const clock_t begin_time = clock();
	int index = 0;

	std::cout << "\n>>> PHASE: Smoothing contour lines \n\n";

	//Precompute inverse matrix (non-scaled version)
	Eigen::SparseMatrix <double> E0(ns, ns), W0(ns, ns), I0(ns, ns);
	Eigen::SimplicialLDLT <Eigen::SparseMatrix<double> > solver(E0);
	E0.setIdentity();  W0.setIdentity();

	const auto D0 = SplineSmoothing::diff(E0, k);
	const auto D0T = D0.transpose();
	solver.compute(W0 + lambda1 * D0T * D0 + 2.0 * lambda2 * E0);
	I0 = solver.solve(E0);

	//Process all contour lines
	for (auto c : contours)
	{
		//Get height of the contour
		const double h1 = c[0]->getZ() - dh;
		const double h1r = Round::roundNumber(h1, 2);
		const double h2 = c[0]->getZ() + dh;
		const double h2r = Round::roundNumber(h2, 2);

		//Print h
		std::cout << ">>> H = " << c[0]->getZ() << "m, n = " << c.size() << ":\n";

		//Find corresponding buffer h - dh
		TVector2D < std::shared_ptr<Point3D > > contour_points_buffer_dh1;
		auto res1 = contour_points_buffers_dh1.equal_range(h1r);

		///No buffer found
		if (res1.first == res1.second)
			continue;

		// Buffer h - dh already created
		else
		{
			//Copy buffer segments
			for (auto it = res1.first; it != res1.second; ++it)
			{
				if (it->second.size() > min_points)
					contour_points_buffer_dh1.push_back(it->second);
			}
		}

		//Find corresponding buffer h + dh
		TVector2D < std::shared_ptr<Point3D > > contour_points_buffer_dh2;
		auto res2 = contour_points_buffers_dh2.equal_range(h2r);

		///No buffer found
		if (res2.first == res2.second)
			continue;

		// Buffer h + dh already created
		else
		{
			//Copy buffer segments
			for (auto it = res2.first; it != res2.second; ++it)
			{
				if (it->second.size() > min_points)
					contour_points_buffer_dh2.push_back(it->second);
			}
		}

		//Are there enough points?
		if ((c.size() > min_points) && (contour_points_buffers_dh1.size() > 0) && (contour_points_buffers_dh2.size() > 0))
		{
			//Resulted contour line
			TVector  <std::shared_ptr <Point3D > > contour_smoothed;

			//Split contour line to parts formed by n points
			const TVector2D  <std::shared_ptr <Point3D > > cparts = splitContourLine(c, ns);

			//Process parts of the contour line
			for (auto cp : cparts)
			{
				std::cout << ".";
				int n = cp.size();

				//Find NN to contour line vertices
				const auto [nn_buffs1, nn_idxs1, nn_dist1, nn_points1] = findNearestNeighbors(cp, contour_points_buffer_dh1);
				const auto [nn_buffs2, nn_idxs2, nn_dist2, nn_points2] = findNearestNeighbors(cp, contour_points_buffer_dh2);

				//Create supplementary matrices
				Eigen::SparseMatrix <double> X(n, 1), Y(n, 1), X1(n, 1), Y1(n, 1), X2(n, 1), Y2(n, 1), W(n, n);
				W.setIdentity();

				for (int i = 0; i < n; i++)
				{
					X.insert(i, 0) = cp[i]->getX();
					Y.insert(i, 0) = cp[i]->getY();

					X1.insert(i, 0) = nn_points1[i]->getX();
					Y1.insert(i, 0) = nn_points1[i]->getY();

					X2.insert(i, 0) = nn_points2[i]->getX();
					Y2.insert(i, 0) = nn_points2[i]->getY();
				}

				//Compute weights
				if (weighted)
				{
					for (int i = 1; i < n - 1; i++)
					{
						//K points forward and backward
						const int i0 = std::max(0, i - 5);
						const int i2 = std::min(n - 1, i + 5);

						//Coordinate differences
						const double dx1 = cp[i0]->getX() - cp[i]->getX();
						const double dy1 = cp[i0]->getY() - cp[i]->getY();
						const double dx2 = cp[i2]->getX() - cp[i]->getX();
						const double dy2 = cp[i2]->getY() - cp[i]->getY();

						//Norms
						const double n1 = sqrt(dx1 * dx1 + dy1 * dy1);
						const double n2 = sqrt(dx2 * dx2 + dy2 * dy2);

						//Angle between segments
						const double arg = (dx1 * dx2 + dy1 * dy2) / (n1 * n2);
						const double om = acos(arg);

						//Weight
						const double w = sin(0.5 * om);
						W.insert(i, i) = w * w;
					}
				}

				//Perform partial displacement
				Eigen::SparseMatrix <double> XS(n, 1), YS(n, 1);

				//Scaled asymetric least squares
				if (scaled)
				{
					const auto [XST, YST] = SplineSmoothing::smoothPolylineInCorridorScaledAsLS(X, Y, X1, Y1, X2, Y2, W, lambda1, lambda2, k);
					XS = XST; YS = YST;
				}

				//Asymetric least squares
				else
				{
					const auto [XST, YST] = weighted ? SplineSmoothing::smoothPolylineInCorridorAsLS(X, Y, X1, Y1, X2, Y2, W, lambda1, lambda2, k) : SplineSmoothing::smoothPolylineInCorridorAsLS(X, Y, X1, Y1, X2, Y2, W, I0, lambda1, lambda2, k);
					XS = XST; YS = YST;
				}
				
				//Convert matrices to points
				TVector < std::shared_ptr<Point3D > > cps;
				for (int i = 0; i < n; i++)
				{
					//Create point
					std::shared_ptr <Point3D> p = std::make_shared <Point3D>(XS.coeff(i, 0), YS.coeff(i, 0), cp[i]->getZ());

					//Add to the list
					cps.push_back(p);
				}

				//Add simplified contour to the list of contours
				contour_smoothed.insert(std::end(contour_smoothed), std::begin(cps), std::end(cps));
			}

			contours_smoothed.push_back(contour_smoothed);

			std::cout << '\n';
		}
	}

	std::cout << "OK";
	std::cout << float(clock() - begin_time) / CLOCKS_PER_SEC;

	return contours_smoothed;
}


TVector2D <std::shared_ptr <Point3D > > ContourLinesSimplify::splitContourLine(const TVector <std::shared_ptr <Point3D > >& c, const int np)
{
	//Split long contour line to shorter segments of the length np
	TVector2D <std::shared_ptr <Point3D > > contours;

	//Zero length 
	if (np == 0)
		return contours;

	//Splitting procedure
	for (size_t i = 0; i < c.size() - 1; )
	{
		//Create new index
		size_t j = std::min(i + np, c.size());

		//Avoid to remain short last segment
		if (c.size() - i < 1.25 * np)
			j = c.size();

		//Copy np elements
		TVector <std::shared_ptr <Point3D > > cp(c.cbegin() + i, c.cbegin() + j);

		//Copy part of the contour
		contours.push_back(cp);

		//Assign j
		i = j - 1;
	}

	return contours;
}


std::tuple<TVector <int>, TVector <int>, TVector <float>, TVector <std::shared_ptr <Point3D > > > ContourLinesSimplify::findNearestNeighbors(const TVector <std::shared_ptr <Point3D> >& qpoints, const TVector2D <std::shared_ptr <Point3D> >& buffers)
{
	//Find nearest neighbor to any contour line vertex
	const int n = qpoints.size();

	TVector <int> nn_buffs(n, -1), nn_idxs(n, -1);
	TVector <float> nn_dists(n, MAX_FLOAT);
	TVector <std::shared_ptr <Point3D > > nn_points(n);

	//Process all query points
	for (int i = 0; i < n; i++)
	{
		//Process all buffer fragments
		for (int j = 0; j < buffers.size(); j++)
		{
			//Find nearest point on the segment
			const auto [i_nn, d_nn, xi_nn, yi_nn] = getNearestLineSegmentPoint(qpoints[i]->getX(), qpoints[i]->getY(), buffers[j]);

			//Update minimum
			if (d_nn < nn_dists[i])
			{
				//Create nearest point
				std::shared_ptr <Point3D> p_min = std::make_shared<Point3D>(xi_nn, yi_nn);

				//Actualize lists of neighbors and their indices
				nn_buffs[i] = j;
				nn_idxs[i] = i_nn;
				nn_dists[i] = d_nn;
				nn_points[i] = p_min;
			}
		}
	}

	return { nn_buffs, nn_idxs, nn_dists, nn_points };
}


std::tuple<int, double, double, double> ContourLinesSimplify::getNearestLineSegmentPoint(const double xq, const double yq, const TVector <std::shared_ptr <Point3D > >& points)
{
	//Get nearest point on the line segments
	int i_min = -1;
	double d_min = 1.0e16, xi_min = 0, yi_min = 0;

	//Check all line segments
	for (int i = 0; i < points.size() - 1; i++)
	{
		double xi, yi;
		const double d = PointLineDistance::getPointLineSegmentDistance2D(xq, yq, points[i]->getX(), points[i]->getY(), points[i + 1]->getX(), points[i + 1]->getY(), xi, yi);

		//Update minimum
		if (d < d_min)
		{
			i_min = i;
			d_min = d;
			xi_min = xi; yi_min = yi;
		}
	}

	return { i_min, d_min, xi_min, yi_min };
}


#endif
