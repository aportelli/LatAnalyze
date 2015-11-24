/*
 * TabFunction.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli, Matt Spraggs
 *
 * LatAnalyze 3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze 3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze 3.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LatAnalyze/TabFunction.hpp>


int main(int argc, char* argv[])
{
    Latan::DVec xs(3);
    xs << -1.0, 0.0, 1.0;
    Latan::DVec ys(3);
    ys << 1.0, 0.0, 1.0;

    auto tab = Latan::TabFunction(xs, ys, Latan::InterpType::QUADRATIC);

    std::cout << "Interpolating naive y = x^2 data using quadratic ";
    std::cout << "interpolation..." << std::endl;
    for (double x = -1.0; x < 1.0; x += 0.1) {
        double y = tab(&x);
        std::cout << "y @ " << x << " = " << y;
        std::cout << " ( " << x * x << " expected)" << std::endl;
    }
    std::cout << std::endl;

    tab = Latan::TabFunction(xs, ys, Latan::InterpType::NEAREST);

    std::cout << "Interpolating naive y = x^2 data using nearest ";
    std::cout << "interpolation..." << std::endl;
    for (double x = -1.0; x < 1.0; x += 0.1) {
        double y = tab(&x);
        std::cout << "y @ " << x << " = " << y;
        double expected = (x > 0.5) ? 1.0 : ((x <= -0.5) ? 1.0 : 0.0);
        std::cout << " ( " << expected << " expected)" << std::endl;
    }
}