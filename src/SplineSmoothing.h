// Description: Contour line smoothing using axial spline

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


#ifndef SplineSmoothing_H
#define SplineSmoothing_H

#include <Eigen/Dense>                               
#include <Eigen/Sparse>                              
#include <Eigen/Core>

//Contour line smoothing using axial spline
class SplineSmoothing
{
        public:
               
                template <typename T>
                static std::tuple<Eigen::SparseMatrix <T>, Eigen::SparseMatrix <T> > smoothPolylineCorridorE(const Eigen::SparseMatrix<T>& X, const Eigen::SparseMatrix<T>& Y, const Eigen::SparseMatrix<T>& X1, const Eigen::SparseMatrix<T>& Y1, const Eigen::SparseMatrix<T>& X2, const Eigen::SparseMatrix<T>& Y2, const Eigen::SparseMatrix<T>& W, const T lambda1, const T lambda2, const int k);
               
                template <typename T>
                static std::tuple<Eigen::SparseMatrix <T>, Eigen::SparseMatrix <T> > smoothPolylineCorridorE(const Eigen::SparseMatrix<T>& X, const Eigen::SparseMatrix<T>& Y, const Eigen::SparseMatrix<T>& X1, const Eigen::SparseMatrix<T>& Y1, const Eigen::SparseMatrix<T>& X2, const Eigen::SparseMatrix<T>& Y2, const Eigen::SparseMatrix<T>& W, const Eigen::SparseMatrix<T>& I0, const T lambda1, const T lambda2, const int k);

                template <typename T>
                static Eigen::SparseMatrix<T> diff(Eigen::SparseMatrix<T> E, const int k);

         
};

#include "SplineSmoothing.hpp"

#endif