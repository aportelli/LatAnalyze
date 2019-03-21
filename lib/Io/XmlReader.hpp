/*
 * XmlReader.hpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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

#ifndef LatAnalyze_XmlReader_hpp_
#define LatAnalyze_XmlReader_hpp_

#include <LatAnalyze/Global.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <LatAnalyze/Io/Xml/tinyxml2.hpp>
#pragma GCC diagnostic pop

BEGIN_LATAN_NAMESPACE

// parsing exception
class XmlParsing: public std::runtime_error
{
public:
    XmlParsing(std::string msg, std::string loc);
};

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
    static const XmlNode * getNextNode(const XmlNode *node,
                                       const std::string &nodeName = "");
    static const XmlNode * getNextSameNode(const XmlNode *node);
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
    // XML structure test
    template <typename... Strs>
    static bool hasNode(const XmlNode *startNode, const std::string &nodeName,
                        Strs... nodeNames);
    template <typename... Strs>
    bool hasNode(const std::string &nodeName, Strs... nodeNames) const;
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
                  "getFirstNode arguments are not compatible with std::string");
    
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
            LATAN_ERROR(Io, "XML node " + name[i] + " not found");
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
            return Latan::strTo<T>(node->GetText());
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
        node = getNextSameNode(node);
    }
    
    return value;
}

template <typename T, typename... Strs>
std::vector<T> XmlReader::getAllValues(const std::string &nodeName,
                                       Strs... nodeNames) const
{
    return getAllValues<T>(root_, nodeName, nodeNames...);
}

// XML structure test //////////////////////////////////////////////////////////
template <typename... Strs>
bool XmlReader::hasNode(const XmlNode *startNode, const std::string &nodeName,
             Strs... nodeNames)
{
    static_assert(static_or<std::is_assignable<std::string, Strs>::value...>::value,
                  "hasNode arguments are not compatible with std::string");

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
            return false;
        }
    }

    return true;
}

template <typename... Strs>
bool XmlReader::hasNode(const std::string &nodeName, Strs... nodeNames) const
{
    if (!root_)
    {
        LATAN_ERROR(Io, "root node is null, no XML file opened");
    }

    return hasNode(root_, nodeName, nodeNames...);
}

END_LATAN_NAMESPACE

#endif // LatAnalyze_XmlReader_hpp_
