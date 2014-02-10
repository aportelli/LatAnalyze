/*
 * Plot.hpp, part of LatAnalyze 3
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

#ifndef Latan_Plot_hpp_
#define Latan_Plot_hpp_

#include <sstream>
#include <stack>
#include <vector>
#include <latan/Global.hpp>

// gnuplot default parameters
#ifndef GNUPLOT_BIN
#define GNUPLOT_BIN "gnuplot"
#endif

#ifndef GNUPLOT_ARGS
#define GNUPLOT_ARGS "-p"
#endif

BEGIN_NAMESPACE

/******************************************************************************
 *                             Plot commands                                  *
 ******************************************************************************/
class PlotCommand
{
public:
    // constructors/destructor
    PlotCommand(void);
    PlotCommand(const std::string &command);
    virtual ~PlotCommand(void);
    // access
    virtual const std::string & getCommand(void) const;
protected:
    std::string command_;
};

/******************************************************************************
 *                               Plot class                                   *
 ******************************************************************************/

class Plot
{
public:
    class Scale
    {
    public:
        enum
        {
            reset  = 0,
            manual = 1 << 0,
            log    = 1 << 1
        };
    };
private:
    struct Range
    {
        double min, max;
    };
    class Axis
    {
    public:
        enum
        {
            x = 0,
            y = 1
        };
    };
public:
    // constructor/destructor
    Plot(void);
    virtual ~Plot(void);
    // plot objects
    Plot & operator<<(const PlotCommand &command);
    // plot parsing and output
    void display(void);
    friend std::ostream & operator<<(std::ostream &out, const Plot &plot);
private:
    // find gnuplot
    void getProgramPath(void);
private:
    // gnuplot execution parameters
    std::string              gnuplotBin_;
    std::string              gnuplotArgs_;
    std::string              gnuplotPath_;
    // string buffer for commands
    std::ostringstream       commandBuffer_;
    // stack of created temporary files
    std::stack<std::string>  tmpFileName_;
    // plot content
    std::string              terminal_;
    std::string              output_;
    std::string              title_;
    unsigned int             scaleMode_[2];
    Range                    scale_[2];
    std::string              label_[2];
    std::vector<std::string> headCommand_;
    std::vector<std::string> plotCommand_;
};

END_NAMESPACE

#endif // Latan_Plot_hpp_
