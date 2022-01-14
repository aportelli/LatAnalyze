/*
 * Utilities.hpp, part of LatAnalyze
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

#ifndef LatAnalyze_Utilities_hpp_
#define	LatAnalyze_Utilities_hpp_

#ifndef LATAN_GLOBAL_HPP_
#include <LatAnalyze/Global.hpp>
#endif

BEGIN_LATAN_NAMESPACE

// Random seed type ////////////////////////////////////////////////////////////
typedef std::random_device::result_type SeedType;

// Type utilities //////////////////////////////////////////////////////////////
// pointer type test
template <typename Derived, typename Base>
inline bool isDerivedFrom(const Base *pt)
{
    return (dynamic_cast<const Derived *>(pt) != nullptr);
}

// static logical or
template <bool... b>
struct static_or;

template <bool... tail>
struct static_or<true, tail...> : static_or<tail...> {};

template <bool... tail>
struct static_or<false, tail...> : std::false_type {};

template <>
struct static_or<> : std::true_type {};

// Environment /////////////////////////////////////////////////////////////////
void testFunction(void);

// String utilities ////////////////////////////////////////////////////////////
inline std::string extension(const std::string fileName)
{
    return fileName.substr(fileName.find_last_of(".") + 1);
}

template <typename T>
inline T strTo(const std::string &str)
{
    T buf;
    std::istringstream stream(str);
    
    stream >> buf;
    
    return buf;
}

// optimized specializations
template <>
inline float strTo<float>(const std::string &str)
{
    return strtof(str.c_str(), (char **)NULL);
}
template <>
inline double strTo<double>(const std::string &str)
{
    return strtod(str.c_str(), (char **)NULL);
}
template <>
inline int strTo<int>(const std::string &str)
{
    return (int)(strtol(str.c_str(), (char **)NULL, 10));
}
template <>
inline long strTo<long>(const std::string &str)
{
    return strtol(str.c_str(), (char **)NULL, 10);
}
template <>
inline std::string strTo<std::string>(const std::string &str)
{
    return str;
}

template <typename T>
inline std::string strFrom(const T x)
{
    std::ostringstream stream;
    
    stream << x;
    
    return stream.str();
}

// specialization for vectors
template<>
inline std::vector<Index> strTo<std::vector<Index>>(const std::string &str)
{
    std::vector<Index>  res;
    std::vector<double> vbuf;
    double              buf;
    std::istringstream  stream(str);
    
    while (!stream.eof())
    {
        stream >> buf;
        res.push_back(buf);
    }
    
    return res;
}

template<>
inline DVec strTo<DVec>(const std::string &str)
{
    DVec                res;
    std::vector<double> vbuf;
    double              buf;
    std::istringstream  stream(str);
    
    while (!stream.eof())
    {
        stream >> buf;
        vbuf.push_back(buf);
    }
    res = Map<DVec>(vbuf.data(), static_cast<Index>(vbuf.size()));
    
    return res;
}

template<>
inline IVec strTo<IVec>(const std::string &str)
{
    IVec                res;
    std::vector<int>    vbuf;
    int                 buf;
    std::istringstream  stream(str);
    
    while (!stream.eof())
    {
        stream >> buf;
        vbuf.push_back(buf);
    }
    res = Map<IVec>(vbuf.data(), static_cast<Index>(vbuf.size()));
    
    return res;
}

template<>
inline UVec strTo<UVec>(const std::string &str)
{
    UVec                      res;
    std::vector<unsigned int> vbuf;
    unsigned int              buf;
    std::istringstream        stream(str);

    while (!stream.eof())
    {
        stream >> buf;
        vbuf.push_back(buf);
    }
    res = Map<UVec>(vbuf.data(), static_cast<Index>(vbuf.size()));

    return res;
}

template <typename T>
void tokenReplace(std::string &str, const std::string token,
                  const T &x, const std::string mark = "@")
{
    std::string fullToken = mark + token + mark;
    
    auto pos = str.find(fullToken);
    if (pos != std::string::npos)
    {
        str.replace(pos, fullToken.size(), strFrom(x));
    }
}

// Manifest file reader ////////////////////////////////////////////////////////
inline std::vector<std::string> readManifest(const std::string manFileName)
{
    std::vector<std::string> list;
    std::ifstream            manFile;
    char                     buf[MAX_PATH_LENGTH];
    
    manFile.open(manFileName);
    while (!manFile.eof())
    {
        manFile.getline(buf, MAX_PATH_LENGTH);
        if (!std::string(buf).empty())
        {
            list.push_back(buf);
        }
    }
    manFile.close();
    
    return list;
}

// Recursive directory creation ////////////////////////////////////////////////
int mkdir(const std::string dirName);

// C++ version of basename/dirname /////////////////////////////////////////////
std::string basename(const std::string& s);
std::string dirname(const std::string& s);

// Progress bar class //////////////////////////////////////////////////////////
class ProgressBar
{
public:
    // constructor
    template <typename A, typename B>
    ProgressBar(const A current, const B total, const Index nCol = 60);
    // IO
    friend std::ostream & operator<<(std::ostream &out,
                                     const ProgressBar &&bar);
private:
    Index current_, total_, nCol_;
};

template <typename A, typename B>
ProgressBar::ProgressBar(const A current, const B total, const Index nCol)
: current_(static_cast<Index>(current))
, total_(static_cast<Index>(total))
, nCol_(nCol)
{}

std::ostream & operator<<(std::ostream &out, const ProgressBar &&bar);

// named variable interface ////////////////////////////////////////////////////
// FIXME: check redundant names and variable number limit
class VarName
{
public:
    // constructor
    VarName(const std::string defName);
    // destructor
    virtual ~VarName(void) = default;
    // access
    std::string getName(const Index i) const;
    void        setName(const Index i, const std::string name);
    // test
    bool hasName(const Index i) const;
private:
    std::string                            defName_;
    std::unordered_map<Index, std::string> name_;
};

END_LATAN_NAMESPACE

#endif // LatAnalyze_Utilities_hpp_
