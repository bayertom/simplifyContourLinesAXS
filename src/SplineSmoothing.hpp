// Description: Smoothing using the minimum energy (axial) spline

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


#ifndef SplineSmoothing_HPP
#define SplineSmoothing_HPP

 
template <typename T>
std::tuple<Eigen::SparseMatrix <T>, Eigen::SparseMatrix <T> > SplineSmoothing::smoothPolylineInCorridorAsLS(const Eigen::SparseMatrix <T>& X, const Eigen::SparseMatrix <T>& Y, const Eigen::SparseMatrix <T>& X1, const Eigen::SparseMatrix <T>& Y1, const Eigen::SparseMatrix <T>& X2, const Eigen::SparseMatrix <T>& Y2, const Eigen::SparseMatrix <T>& W, const T lambda1, const T lambda2, const int k)
{
	//Spline smoothing with the constraints (Eigen version).
	//Non-scaled version, asymetric least squares
	const unsigned int m = X.rows();

	//Create initial matrices
	Eigen::SparseMatrix <T> E(m, m);
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<T> > solver(E);
	E.setIdentity();
	const auto D = diff(E, k);
	
	//Inverse
	const auto DT = D.transpose();
	solver.compute(W + lambda1 * DT * D + 2.0 * lambda2 * E);
	const auto I = solver.solve(E);

	//Solution of AXS
	const auto XS = I * (W * X + lambda2 * (X1 + X2));
	const auto YS = I * (W * Y + lambda2 * (Y1 + Y2));

	return { XS, YS };
}


template <typename T>
std::tuple<Eigen::SparseMatrix <T>, Eigen::SparseMatrix <T> > SplineSmoothing::smoothPolylineInCorridorAsLSS(const Eigen::SparseMatrix <T>& X, const Eigen::SparseMatrix <T>& Y, const Eigen::SparseMatrix <T>& X1, const Eigen::SparseMatrix <T>& Y1, const Eigen::SparseMatrix <T>& X2, const Eigen::SparseMatrix <T>& Y2, const Eigen::SparseMatrix <T>& W, const T lambda1, const T lambda2, const int k)
{
	//Spline smoothing with the constraints (Eigen version)
	//Scaled version, asymetric least squares
	const unsigned int m = X.rows();

	//Create initial matrices
	Eigen::SparseMatrix <T> E(m, m), ZX(m, m), ZY(m, m);
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<T> > solverx(E), solvery(E);
	E.setIdentity();
	const auto D = diff(E, k);

	//Compute elements of ZX, ZY scaling matrices
	const double min_element = 0.01;
	for (int i = 0; i < m; i++)
	{
		const double dx = std::max(fabs(X1.coeff(i, 0) - X2.coeff(i, 0)), min_element);
		const double dy = std::max(fabs(Y1.coeff(i, 0) - Y2.coeff(i, 0)), min_element);
		ZX.insert(i, i) = 1.0 / dx;
		ZY.insert(i, i) = 1.0 / dy;
	}

	//Inverse
	const auto DT = D.transpose();
	const auto ZXT = ZX.transpose();
	const auto ZYT = ZY.transpose();
	
	solverx.compute(ZXT * W * ZX + lambda1 * DT * D + 2.0 * lambda2 * ZXT * ZX);
	solvery.compute(ZYT * W * ZY + lambda1 * DT * D + 2.0 * lambda2 * ZYT * ZY);

	const auto IX = solverx.solve(E);
	const auto IY = solvery.solve(E);

	//Solution of AXS
	const auto XS = IX * (ZXT * W * ZX * X + lambda2 * ZXT * ZX * (X1 + X2));
	const auto YS = IY * (ZYT * W * ZY * Y + lambda2 * ZYT * ZY * (Y1 + Y2));

	return { XS, YS };
}


template <typename T>
std::tuple<Eigen::SparseMatrix <T>, Eigen::SparseMatrix <T> > SplineSmoothing::smoothPolylineInCorridorAsLSS2(const Eigen::SparseMatrix <T>& X, const Eigen::SparseMatrix <T>& Y, const Eigen::SparseMatrix <T>& X1, const Eigen::SparseMatrix <T>& Y1, const Eigen::SparseMatrix <T>& X2, const Eigen::SparseMatrix <T>& Y2, const Eigen::SparseMatrix <T>& W, const T lambda1, const T lambda2, const int k)
{
	//Spline smoothing with the constraints (Eigen version)
	//Scaled version, asymetric least squares
	const unsigned int m = X.rows();

	//Create initial matrices
	Eigen::SparseMatrix <T> E(m, m), ZX(m, m), ZY(m, m);
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<T> > solverx(E), solvery(E);
	E.setIdentity();
	const auto D = diff(E, k);

	//Compute elements of ZX, ZY scaling matrices
	const double min_element = 0.01;
	for (int i = 0; i < m; i++)
	{
		const double dx = std::max(fabs(X1.coeff(i, 0) - X2.coeff(i, 0)), min_element);
		const double dy = std::max(fabs(Y1.coeff(i, 0) - Y2.coeff(i, 0)), min_element);
		ZX.insert(i, i) = 1.0 / dx;
		ZY.insert(i, i) = 1.0 / dy;
	}

	//Inverse
	const auto DT = D.transpose();
	const auto ZXT = ZX.transpose();
	const auto ZYT = ZY.transpose();

	solverx.compute(W + lambda1 * DT * D + 2.0 * lambda2 * ZXT * ZX);
	solvery.compute(W + lambda1 * DT * D + 2.0 * lambda2 * ZYT * ZY);

	const auto IX = solverx.solve(E);
	const auto IY = solvery.solve(E);

	//Solution of AXS
	const auto XS = IX * (W * X + lambda2 * ZXT * ZX * (X1 + X2));
	const auto YS = IY * (W * Y + lambda2 * ZYT * ZY * (Y1 + Y2));

	return { XS, YS };
}


template <typename T>
Eigen::SparseMatrix<T> SplineSmoothing::diff(Eigen::SparseMatrix<T> E, const int k)
{
	//Compute difference matrix
	for (int i = 0; i < k; i++)
	{
		//Get submatrices
		Eigen::SparseMatrix<T> E1 = E.block(0, 0, E.rows() - 1, E.cols());
		Eigen::SparseMatrix<T> E2 = E.block(1, 0, E.rows() - 1, E.cols());

		//Compute difference matrix
		E = E2 - E1;
	}

	return E;
}


template <typename T>
std::tuple<Eigen::SparseMatrix <T>, Eigen::SparseMatrix <T> > SplineSmoothing::smoothPolylineInCorridorAsLS(const Eigen::SparseMatrix <T>& X, const Eigen::SparseMatrix <T>& Y, const Eigen::SparseMatrix <T>& X1, const Eigen::SparseMatrix <T>& Y1, const Eigen::SparseMatrix <T>& X2, const Eigen::SparseMatrix <T>& Y2, const Eigen::SparseMatrix <T>& W, const Eigen::SparseMatrix <T>& I0, const T lambda1, const T lambda2, const int k)
{
	//Spline smoothing with the constraints (Eigen version)
	//Asymetric least squares
	//Precomputed inverse matrix
	const unsigned int m = X.rows();

	//Create inerse matrix
	Eigen::SparseMatrix <double> I(m, m);

	//Assign precomputed invere matrix
	if (I0.rows () == m)
	{
		I = I0;
	}

	//Different size, compute new inverse matrix
	else
	{
		Eigen::SparseMatrix <double> E(m, m);
		Eigen::SimplicialLDLT <Eigen::SparseMatrix<double> > solver(E);
		E.setIdentity();

		//Difference matrix
		const auto D = SplineSmoothing::diff(E, k);

		//Inverse
		const auto DT = D.transpose();
		solver.compute(W + lambda1 * DT * D + 2.0 * lambda2 * E);
		I = solver.solve(E);
	}

	//Solution of AXS
	const auto XS = I * (W * X + lambda2 * (X1 + X2));
	const auto YS = I * (W * Y + lambda2 * (Y1 + Y2));

	return { XS, YS };
}

#endif