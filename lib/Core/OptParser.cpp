/*
 * OptParser.cpp, part of LatAnalyze
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

#include <LatAnalyze/Core/OptParser.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

static char optRegex[] = "(-([a-zA-Z])(.+)?)|(--([a-zA-Z_-]+)=?(.+)?)";

/******************************************************************************
 *                         OptParser implementation                           *
 ******************************************************************************/
// regular expressions /////////////////////////////////////////////////////////
const regex OptParser::optRegex_(optRegex);

// access //////////////////////////////////////////////////////////////////////
void OptParser::addOption(const std::string shortName,
                          const std::string longName,
                          const OptType type, const bool optional,
                          const std::string helpMessage,
                          const std::string defaultVal)
{
    OptPar par;
    
    par.shortName   = shortName;
    par.longName    = longName;
    par.defaultVal  = defaultVal;
    par.helpMessage = helpMessage;
    par.type        = type;
    par.optional    = optional;
    opt_.push_back(par);
}

bool OptParser::gotOption(const std::string name) const
{
    int i = optIndex(name);
    
    if (result_.size() != opt_.size())
    {
        throw(runtime_error("options not parsed"));
    }
    if (i >= 0)
    {
        return result_[i].present;
    }
    else
    {
        throw(out_of_range("no option with name '" + name + "'"));
    }
}

const vector<string> & OptParser::getArgs(void) const
{
    return arg_;
}

// parse ///////////////////////////////////////////////////////////////////////
bool OptParser::parse(int argc, char *argv[])
{
    smatch        sm;
    queue<string> arg;
    int           expectVal = -1;
    bool          isCorrect = true;
    
    for (int i = 1; i < argc; ++i)
    {
        arg.push(argv[i]);
    }
    result_.clear();
    result_.resize(opt_.size());
    arg_.clear();
    for (unsigned int i = 0; i < opt_.size(); ++i)
    {
        result_[i].value = opt_[i].defaultVal;
    }
    while (!arg.empty())
    {
        // option
        if (regex_match(arg.front(), sm, optRegex_))
        {
            // should it be a value?
            if (expectVal >= 0)
            {
                cerr << "warning: expected value for option ";
                cerr << optName(opt_[expectVal]);
                cerr << ", got option '" << arg.front() << "' instead" << endl;
                expectVal = -1;
                isCorrect = false;
            }
            // short option
            if (sm[1].matched)
            {
                string optName = sm[2].str();
                
                // find option
                auto it = find_if(opt_.begin(), opt_.end(),
                                  [&optName](const OptPar &p)
                                  {
                                      return (p.shortName == optName);
                                  });
                
                // parse if found
                if (it != opt_.end())
                {
                    unsigned int i =  it - opt_.begin();
                    
                    result_[i].present = true;
                    if (opt_[i].type == OptType::value)
                    {
                        if (sm[3].matched)
                        {
                            result_[i].value = sm[3].str();
                        }
                        else
                        {
                            expectVal = i;
                        }
                    }
                }
                // warning if not found
                else
                {
                    cerr << "warning: unknown option '" << arg.front() << "'";
                    cerr << endl;
                }
            }
            // long option
            else if (sm[4].matched)
            {
                string optName = sm[5].str();
                
                // find option
                auto it = find_if(opt_.begin(), opt_.end(),
                                  [&optName](const OptPar &p)
                                  {
                                      return (p.longName == optName);
                                  });
                
                // parse if found
                if (it != opt_.end())
                {
                    unsigned int i =  it - opt_.begin();
                    
                    result_[i].present = true;
                    if (opt_[i].type == OptType::value)
                    {
                        if (sm[6].matched)
                        {
                            result_[i].value = sm[6].str();
                        }
                        else
                        {
                            expectVal = i;
                        }
                    }
                }
                // warning if not found
                else
                {
                    cerr << "warning: unknown option '" << arg.front() << "'";
                    cerr << endl;
                }
            }
        }
        else if (expectVal >= 0)
        {
            result_[expectVal].value = arg.front();
            expectVal                = -1;
        }
        else
        {
            arg_.push_back(arg.front());
        }
        arg.pop();
    }
    if (expectVal >= 0)
    {
        cerr << "warning: expected value for option ";
        cerr << optName(opt_[expectVal]) << endl;
        expectVal = -1;
        isCorrect = false;
    }
    for (unsigned int i = 0; i < opt_.size(); ++i)
    {
        if (!opt_[i].optional and !result_[i].present)
        {
            cerr << "warning: mandatory option " << optName(opt_[i]);
            cerr << " is missing" << endl;
            isCorrect = false;
        }
    }
    
    return isCorrect;
}

// find option index ///////////////////////////////////////////////////////////
int OptParser::optIndex(const string name) const
{
    auto it = find_if(opt_.begin(), opt_.end(), [&name](const OptPar &p)
                      {
                          return (p.shortName == name) or (p.longName == name);
                      });
    
    if (it != opt_.end())
    {
        return static_cast<int>(it - opt_.begin());
    }
    else
    {
        return -1;
    }
}

// option name for messages ////////////////////////////////////////////////////
std::string OptParser::optName(const OptPar &opt)
{
    std::string res = "";
    
    if (!opt.shortName.empty())
    {
        res += "-" + opt.shortName;
        if (!opt.longName.empty())
        {
            res += "/";
        }
    }
    if (!opt.longName.empty())
    {
        res += "--" + opt.longName;
        if (opt.type == OptParser::OptType::value)
        {
            res += "=";
        }
    }
    
    return res;
}

// print option list ///////////////////////////////////////////////////////////
std::ostream & Latan::operator<<(std::ostream &out, const OptParser &parser)
{
    for (auto &o: parser.opt_)
    {
        out << setw(20) << OptParser::optName(o);
        out << ": " << o.helpMessage;
        if (!o.defaultVal.empty())
        {
            out << " (default: " << o.defaultVal << ")";
        }
        out << endl;
    }
    
    return out;
}
