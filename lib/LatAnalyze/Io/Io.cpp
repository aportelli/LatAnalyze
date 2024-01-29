/*
 * Io.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#include <LatAnalyze/Io/Io.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Io/AsciiFile.hpp>
#include <LatAnalyze/Io/Hdf5File.hpp>

using namespace std;
using namespace Latan;

string Io::getFirstName(const string &fileName)
{
    std::unique_ptr<File> file = open(fileName);
    
    return file->getFirstName();
}

unique_ptr<File> Io::open(const std::string &fileName, const unsigned int mode)
{
    string ext = extension(fileName);
    
    if ((ext == "dat") or (ext == "sample") or (ext == "seed"))
    {
        return unique_ptr<File>(new AsciiFile(fileName, mode));
    }
    else if (ext == "h5")
    {
        return unique_ptr<File>(new Hdf5File(fileName, mode));
    }
    else
    {
        LATAN_ERROR(Io, "unknown file extension '" + ext + "'");
    }
}
