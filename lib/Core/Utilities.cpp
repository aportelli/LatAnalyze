/*
 * Utilities.cpp, part of LatAnalyze
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

#include <LatAnalyze/Core/Utilities.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

void Latan::testFunction(void)
{}

ostream & Latan::operator<<(ostream &out, const ProgressBar &&bar)
{
    const Index nTick = bar.nCol_*bar.current_/bar.total_;
    
    out << "[";
    for (Index i = 0; i < nTick; ++i)
    {
        out << "=";
    }
    for (Index i = nTick; i < bar.nCol_; ++i)
    {
        out << " ";
    }
    out << "] " << bar.current_ << "/" << bar.total_;
    out.flush();
    
    return out;
}

int Latan::mkdir(const std::string dirName)
{
    if (access(dirName.c_str(), R_OK|W_OK|X_OK))
    {
        mode_t mode755;
        char   tmp[MAX_PATH_LENGTH];
        char   *p = NULL;
        size_t len;

        mode755 = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;

        snprintf(tmp, sizeof(tmp), "%s", dirName.c_str());
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
        {
            tmp[len - 1] = 0;
        }
        for(p = tmp + 1; *p; p++)
        {
            if(*p == '/')
            {
                *p = 0;
                ::mkdir(tmp, mode755);
                *p = '/';
            }
        }

        return ::mkdir(tmp, mode755);
    }
    else
    {
        return 0;
    }
}

string Latan::basename(const string &s)
{
    constexpr char sep = '/';
    size_t         i   = s.rfind(sep, s.length());
    
    if (i != string::npos)
    {
        return s.substr(i+1, s.length() - i);
    }
    else
    {
        return s;
    }
}

std::string Latan::dirname(const std::string &s)
{
    constexpr char sep = '/';
    size_t         i   = s.rfind(sep, s.length());
    
    if (i != std::string::npos)
    {
        return s.substr(0, i);
    }
    else
    {
        return "";
    }
}

VarName::VarName(const string defName)
: defName_(defName)
{}

string VarName::getName(const Index i) const
{
    if (hasName(i))
    {
        return name_.at(i);
    }
    else
    {
        return defName_ + "_" + strFrom(i);
    }
}

void VarName::setName(const Index i, const string name)
{
    name_[i] = name;
}

bool VarName::hasName(const Index i) const
{
    return (name_.find(i) != name_.end());
}

unsigned int VarName::size(void) const
{
    return name_.size();
}

