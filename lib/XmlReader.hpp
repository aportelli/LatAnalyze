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
typedef tinyxml2::XMLElement XmlNode;

class XmlReader
{
public:
    // constructor
    XmlReader(void) = default;
    explicit XmlReader(const std::string &fileName);
    // destructor
    virtual ~XmlReader(void) = default;
    // IO
    void open(const std::string &fileName);
    // XML structure access
    template <typename... Strs>
    static const XmlNode * getFirstNode(const XmlNode *startNode,
                                        const std::string &nodeName,
                                        Strs... nodeNames);
    template <typename... Strs>
    const XmlNode * getFirstNode(const std::string &nodeName,
                                 Strs... nodeNames) const;
    static const XmlNode * getNextNode(const XmlNode *node);
    template <typename T>
    static T getValue(const XmlNode *node);
    template <typename T, typename... Strs>
    static T getFirstValue(const XmlNode *startNode,
                           const std::string &nodeName, Strs... nodeNames);
    template <typename T, typename... Strs>
    T getFirstValue(const std::string &nodeName, Strs... nodeNames) const;
    template <typename T, typename... Strs>
    static std::vector<T> getAllValues(const XmlNode *startNode,
                                       const std::string &nodeName,
                                       Strs... nodeNames);
    template <typename T, typename... Strs>
    std::vector<T> getAllValues(const std::string &nodeName,
                                Strs... nodeNames) const;
private:
    
private:
    std::string           name_;
    tinyxml2::XMLDocument doc_;
    XmlNode               *root_{nullptr};
};

/******************************************************************************
 *                     XmlReader template implementation                      *
 ******************************************************************************/
// XML structure access ////////////////////////////////////////////////////////
template <typename... Strs>
const XmlNode * XmlReader::getFirstNode(const XmlNode *startNode,
                                        const std::string &nodeName,
                                        Strs... nodeNames)
{
    static_assert(static_or<std::is_assignable<std::string, Strs>::value...>::value,
                  "getFirstValue arguments are not compatible with std::string");
    
    const unsigned int nName  = sizeof...(nodeNames) + 1;
    const std::string  name[] = {nodeName, nodeNames...};
    const XmlNode      *node  = startNode;
    
    if (!node)
    {
        LATAN_ERROR(Io, "root node is null, no XML file opened");
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

template <typename... Strs>
const XmlNode * XmlReader::getFirstNode(const std::string &nodeName,
                                        Strs... nodeNames) const
{
    if (!root_)
    {
        LATAN_ERROR(Io, "root node is null, no XML file opened");
    }
    
    return getFirstNode(root_, nodeName, nodeNames...);
}

template <typename T>
T XmlReader::getValue(const XmlNode *node)
{
    if (node)
    {
        if (node->GetText())
        {
            return strTo<T>(node->GetText());
        }
        else
        {
            return T();
        }
    }
    else
    {
        return T();
    }
}

template <typename T, typename... Strs>
T XmlReader::getFirstValue(const XmlNode *startNode,
                           const std::string &nodeName, Strs... nodeNames)
{
    const XmlNode *node = getFirstNode(startNode, nodeName, nodeNames...);
    
    return getValue<T>(node);
}

template <typename T, typename... Strs>
T XmlReader::getFirstValue(const std::string &nodeName, Strs... nodeNames) const
{
    return getFirstValue<T>(root_, nodeName, nodeNames...);
}

template <typename T, typename... Strs>
std::vector<T> XmlReader::getAllValues(const XmlNode *startNode,
                                       const std::string &nodeName,
                                       Strs... nodeNames)
{
    const XmlNode  *node = getFirstNode(startNode, nodeName, nodeNames...);
    std::vector<T> value;
    
    while (node)
    {
        value.push_back(getValue<T>(node));
        node = getNextNode(node);
    }
    
    return value;
}

template <typename T, typename... Strs>
std::vector<T> XmlReader::getAllValues(const std::string &nodeName,
                                       Strs... nodeNames) const
{
    return getAllValues<T>(root_, nodeName, nodeNames...);
}

END_NAMESPACE

#endif // Latan_XmlReader_hpp_
