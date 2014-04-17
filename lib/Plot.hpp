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

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Mat.hpp>
#include <LatAnalyze/XYStatData.hpp>
#include <sstream>
#include <stack>
#include <vector>

// gnuplot default parameters
#ifndef GNUPLOT_BIN
#define GNUPLOT_BIN "gnuplot"
#endif

#ifndef GNUPLOT_ARGS
#define GNUPLOT_ARGS "-p"
#endif

BEGIN_NAMESPACE

/******************************************************************************
 *                              Plot objects                                  *
 ******************************************************************************/
class PlotObject
{
public:
    // destructor
    virtual ~PlotObject(void) = default;
    // access
    std::string                 popTmpFile(void);
    const std::string &         getCommand(void) const;
    // test
    bool                        gotTmpFile(void) const;
protected:
    // access
    void pushTmpFile(const std::string &fileName);
    void setCommand(const std::string &command);
    // dump a matrix to a temporary file
    std::string dumpToTmpFile(const DMat &m);
private:
    // plot command
    std::string command_;
    // stack of created temporary files
    std::stack<std::string>  tmpFileName_;
};

class PlotCommand: public PlotObject
{
public:
    // constructor
    explicit PlotCommand(const std::string &command);
    // destructor
    virtual ~PlotCommand(void) = default;
};

class PlotData: public PlotObject
{
public:
    // constructor
    PlotData(const XYStatData &data, const Index i = 0, const Index j = 0);
    // destructor
    virtual ~PlotData(void) = default;
};

class PlotFunction: public PlotObject
{
public:
    // constructor
    PlotFunction(const DoubleFunction &function, const double xMin,
                 const double xMax, const unsigned int nPoint = 1000);
    // destructor
    virtual ~PlotFunction(void) = default;
};

/******************************************************************************
 *                             Plot modifiers                                 *
 ******************************************************************************/
enum class Axis {x = 0, y = 1};

struct Range
{
    double min, max;
};

struct PlotOptions
{
    std::string  terminal;
    std::string  output;
    std::string  title;
    unsigned int scaleMode[2];
    Range        scale[2];
    std::string  label[2];
    std::string  lineColor;
};

class PlotModifier
{
public:
    // destructor
    virtual ~PlotModifier(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const = 0;
};

class Color: public PlotModifier
{
public:
    // constructor
    explicit Color(const std::string &color);
    // destructor
    virtual ~Color(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::string color_;
};

class LogScale: public PlotModifier
{
public:
    // constructor
    explicit LogScale(const Axis axis);
    // destructor
    virtual ~LogScale(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const Axis axis_;
};

class PlotRange: public PlotModifier
{
public:
    // constructor
    PlotRange(const Axis axis, const double min, const double max);
    // destructor
    virtual ~PlotRange(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const Axis   axis_;
    const double min_, max_;
};

class Title: public PlotModifier
{
public:
    // constructor
    explicit Title(const std::string &title);
    // destructor
    virtual ~Title(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::string title_;
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
public:
    // constructor/destructor
    Plot(void);
    virtual ~Plot(void);
    // plot operations
    Plot & operator<<(PlotObject   &&command);
    Plot & operator<<(PlotModifier &&modifier);
    // plot parsing and output
    void display(void);
    friend std::ostream & operator<<(std::ostream &out, const Plot &plot);
    // plot reset
    void reset(void);
private:
    // clean temporary files
    void cleanTmpFiles(void);
    // find gnuplot
    void getProgramPath(void);
    // default options
    void initOptions(void);
private:
    // gnuplot execution parameters
    std::string              gnuplotBin_  {GNUPLOT_BIN};
    std::string              gnuplotArgs_ {GNUPLOT_ARGS};
    std::string              gnuplotPath_ {""};
    // string buffer for commands
    std::ostringstream       commandBuffer_;
    // stack of created temporary files
    std::stack<std::string>  tmpFileName_;
    // plot content
    PlotOptions              options_;
    std::vector<std::string> headCommand_;
    std::vector<std::string> plotCommand_;
};

std::ostream & operator<<(std::ostream &out, const Plot &plot);

END_NAMESPACE

#endif // Latan_Plot_hpp_
