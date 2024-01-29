/*
 * OptParser.hpp, part of LatAnalyze
 *
 * Copyright (C) 2016 Antonin Portelli
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

#ifndef LatAnalyze_OptParser_hpp_
#define LatAnalyze_OptParser_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                       command-line option parser                           *
 ******************************************************************************/
class OptParser
{
public:
    enum class OptType {value, trigger};
private:
    struct OptPar
    {
        std::string shortName, longName, defaultVal, helpMessage;
        OptType     type;
        bool        optional;
    };
    struct OptRes
    {
        std::string value;
        bool        present;
    };
public:
    // constructor
    OptParser(void) = default;
    // destructor
    virtual ~OptParser(void) = default;
    // access
    void addOption(const std::string shortName, const std::string longName,
                   const OptType type, const bool optional = false,
                   const std::string helpMessage = "",
                   const std::string defaultVal = "");
    bool gotOption(const std::string name) const;
    template <typename T = std::string>
    T    optionValue(const std::string name) const;
    const std::vector<std::string> & getArgs(void) const;
    // parse
    bool parse(int argc, char *argv[]);
    // print option list
    friend std::ostream & operator<<(std::ostream &out,
                                     const OptParser &parser);
private:
    // find option index
    int optIndex(const std::string name) const;
    // option name for messages
    static std::string optName(const OptPar &opt);
private:
    std::vector<OptPar>      opt_;
    std::vector<OptRes>      result_;
    std::vector<std::string> arg_;
    static const std::regex  optRegex_;
};

std::ostream & operator<<(std::ostream &out, const OptParser &parser);

/******************************************************************************
 *                     OptParser template implementation                      *
 ******************************************************************************/
template <typename T>
T OptParser::optionValue(const std::string name) const
{
    int i = optIndex(name);
    
    if (result_.size() != opt_.size())
    {
        throw(std::runtime_error("options not parsed"));
    }
    if (i >= 0)
    {
        return strTo<T>(result_[i].value);
    }
    else
    {
        throw(std::out_of_range("no option with name '" + name + "'"));
    }
}

END_LATAN_NAMESPACE

#endif // LatAnalyze_OptParser_hpp_
