/*
 * XmlReader.hpp, part of LatAnalyze 3
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

#ifndef Latan_XmlReader_hpp_
#define Latan_XmlReader_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/XML/tinyxml2.hpp>
#include <iostream>
#include <vector>

BEGIN_NAMESPACE

/******************************************************************************
 *                        XML parameter file reader                           *
 ******************************************************************************/
class XmlReader
{
public:
    // constructor
    XmlReader(void) = default;
    explicit XmlReader(const std::string &fileName);
    // destructor
    virtual ~XmlReader(void) = default;
    // IO
    template <typename T, typename... Strs>
    T getFirstValue(const std::string &nodeName, Strs... nodeNames);
    template <typename T, typename... Strs>
    std::vector<T> getAllValues(const std::string &nodeName, Strs... nodeNames);
    void open(const std::string &fileName);
private:
    template <typename... Strs>
    tinyxml2::XMLElement * getFirstNode(const std::string &nodeName,
                                        Strs... nodeNames);
private:
    std::string           name_;
    tinyxml2::XMLDocument doc_;
    tinyxml2::XMLElement  *root_{nullptr};
};

/******************************************************************************
 *                     XmlReader template implementation                      *
 ******************************************************************************/
template <typename T, typename... Strs>
T XmlReader::getFirstValue(const std::string &nodeName, Strs... nodeNames)
{
    tinyxml2::XMLElement *node = getFirstNode(nodeName, nodeNames...);
    
    if (node->GetText())
    {
        return strTo<T>(node->GetText());
    }
    else
    {
        return T();
    }
}

template <typename T, typename... Strs>
std::vector<T> XmlReader::getAllValues(const std::string &nodeName,
                                       Strs... nodeNames)
{
    tinyxml2::XMLElement *node = getFirstNode(nodeName, nodeNames...);
    std::vector<T>       value;
    
    while (node)
    {
        if (node->GetText())
        {
            value.push_back(strTo<T>(node->GetText()));
        }
        else
        {
            value.push_back(T());
        }
        node = node->NextSiblingElement();
    }
    
    return value;
}

template <typename... Strs>
tinyxml2::XMLElement * XmlReader::getFirstNode(const std::string &nodeName,
                                               Strs... nodeNames)
{
    static_assert(static_or<std::is_assignable<std::string, Strs>::value...>::value,
                  "getFirstValue arguments are not compatible with std::string");
    
    const unsigned int   nName  = sizeof...(nodeNames) + 1;
    const std::string    name[] = {nodeName, nodeNames...};
    tinyxml2::XMLElement *node  = root_;
    
    if (!root_)
    {
        LATAN_ERROR(Io, "no XML file opened");
    }
    for (unsigned int i = 0; i < nName; ++i)
    {
        node = node->FirstChildElement(name[i].c_str());
        if (!node)
        {
            LATAN_ERROR(Parsing, "XML node " + name[i] + " not found");
        }
    }
    
    return node;
}

END_NAMESPACE

#endif // Latan_XmlReader_hpp_
