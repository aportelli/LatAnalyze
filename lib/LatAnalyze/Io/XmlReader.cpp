/*
 * XmlReader.cpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#include <LatAnalyze/Io/XmlReader.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

XmlParsing::XmlParsing(string msg, string loc)
: runtime_error("XML reader error: " + msg + " (" + loc + ")")
{}

/******************************************************************************
 *                         XmlReader implementation                           *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
XmlReader::XmlReader(const string &fileName)
{
    open(fileName);
}

// IO //////////////////////////////////////////////////////////////////////////
void XmlReader::open(const string &fileName)
{
    name_ = fileName;
    doc_.LoadFile(name_.c_str());
    if (doc_.Error())
    {
        string errMsg;
        
        if (doc_.ErrorStr())
        {
            errMsg = doc_.ErrorStr();
        }
        LATAN_ERROR(Io, "cannot open file " + fileName + " [tinyxml2 code "
                  + strFrom(doc_.ErrorID()) + ": " + errMsg + "]");
    }
    root_ = doc_.RootElement();
}

// XML structure access ////////////////////////////////////////////////////////
const XmlNode * XmlReader::getNextNode(const XmlNode *node,
                                       const string &nodeName)
{
    const char *pt = (nodeName.empty()) ? nullptr : nodeName.c_str();
    
    if (node)
    {
        return node->NextSiblingElement(pt);
    }
    else
    {
        return nullptr;
    }
}

const XmlNode * XmlReader::getNextSameNode(const XmlNode *node)
{
    if (node)
    {
        return getNextNode(node, node->Name());
    }
    else
    {
        return nullptr;
    }
}
