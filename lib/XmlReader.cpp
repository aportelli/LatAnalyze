/*
 * XmlReader.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/XmlReader.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                         XmlReader implementation                           *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
XmlReader::XmlReader(const string &fileName)
{
    open(fileName);
}

// IO //////////////////////////////////////////////////////////////////////////
void XmlReader::open(const std::string &fileName)
{
    name_ = fileName;
    doc_.LoadFile(name_.c_str());
    if (doc_.Error())
    {
        string errMsg1, errMsg2;
        
        if (doc_.GetErrorStr1())
        {
            errMsg1 = doc_.GetErrorStr1();
        }
        if (doc_.GetErrorStr2())
        {
            errMsg2 = doc_.GetErrorStr2();
        }
        LATAN_ERROR(Io, "tinyxml2 code " + strFrom(doc_.ErrorID()) + ": " +
                    errMsg1 + " - " + errMsg2);
    }
    root_ = doc_.RootElement();
}
