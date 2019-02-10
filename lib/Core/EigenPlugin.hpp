/*
 * EigenPlugin.hpp, part of LatAnalyze
 *
 * Copyright (C) 2015 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

Derived pInverse(const double tolerance = 1.0e-10)
{
    auto         svd   = jacobiSvd(Eigen::ComputeThinU|Eigen::ComputeThinV);
    const auto   u     = svd.matrixU();
    const auto   v     = svd.matrixV();
    auto         s     = svd.singularValues();
    double       maxsv = 0.;
    unsigned int elim  = 0;
    
    for (Index i = 0; i < s.rows(); ++i)
    {
        if (fabs(s(i)) > maxsv) maxsv = fabs(s(i));
    }
    for (Index i = 0; i < s.rows(); ++i)
    {
        if (fabs(s(i)) > maxsv*tolerance)
        {
            s(i) = 1./s(i);
        }
        else
        {
            elim++;
            s(i) = 0.;
        }
    }
    if (elim)
    {
        std::cerr << "warning: pseudoinverse: " << elim << "/";
        std::cerr << s.rows() << " singular value(s) eliminated (tolerance= ";
        std::cerr << tolerance << ")" << std::endl;
    }
    
    return v*s.asDiagonal()*u.transpose();
}

Derived singularValues(void)
{
    auto svd = jacobiSvd();
    
    return svd.singularValues();
}
