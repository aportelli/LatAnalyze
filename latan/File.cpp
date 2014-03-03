/*
 * File.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2014 Antonin Portelli
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

#include <latan/File.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                          File implementation                               *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
File::File(const string &name, const unsigned int mode)
: name_(name)
, mode_(mode)
{}

// destructor //////////////////////////////////////////////////////////////////
File::~File(void)
{
    deleteData();
}

// access //////////////////////////////////////////////////////////////////////
const string & File::getName(void) const
{
    return name_;
}

unsigned int File::getMode(void) const
{
    return mode_;
}

// internal functions //////////////////////////////////////////////////////////
void File::deleteData(void)
{
    for (auto &i : data_)
    {
        i.second.reset();
    }
    data_.clear();
}

void File::checkWritability(void)
{
    if (!((mode_ & Mode::write)||(mode_ & Mode::append))||!isOpen())
    {
        LATAN_ERROR(Io, "file '" + name_ + "' is not writable");
    }
}
