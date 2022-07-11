/*
 * Plot.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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
#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/Core/Mat.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>
#include <LatAnalyze/Statistics/Histogram.hpp>
#include <LatAnalyze/Statistics/XYStatData.hpp>

// gnuplot default parameters
#ifndef GNUPLOT_BIN
#define GNUPLOT_BIN "gnuplot"
#endif

#ifndef GNUPLOT_ARGS
#define GNUPLOT_ARGS "-p"
#endif

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                              Plot objects                                  *
 ******************************************************************************/
class PlotObject
{
public:
    // destructor
    virtual ~PlotObject(void) = default;
    // access
    std::string         popTmpFile(void);
    const std::string & getCommand(void) const;
    const std::string & getHeadCommand(void) const;
    // test
    bool                gotTmpFile(void) const;
protected:
    // access
    void pushTmpFile(const std::string &fileName);
    void setCommand(const std::string &command);
    void setHeadCommand(const std::string &command);
    // dump a matrix to a temporary file
    std::string dumpToTmpFile(const DMat &m);
private:
    // plot command
    std::string command_;
    std::string headCommand_;
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

class PlotHeadCommand: public PlotObject
{
public:
    // constructor
    explicit PlotHeadCommand(const std::string &command);
    // destructor
    virtual ~PlotHeadCommand(void) = default;
};

class PlotData: public PlotObject
{
public:
    // constructor
    // PlotData(const DMatSample &x, const DMatSample &y);
    // PlotData(const DVec       &x, const DMatSample &y);
    // PlotData(const DMatSample &x, const DVec       &y);
    PlotData(const DVec &x, const DVec &y, const DVec &yerr, DVec * opacity=nullptr);

    PlotData(const XYStatData &data, const Index i = 0, const Index j = 0);
    PlotData(const DMatSample &x, const DMatSample &y, const bool abs = false);
    PlotData(const DVec       &x, const DMatSample &y, const bool abs = false);
    PlotData(const DMatSample &x, const DVec       &y, const bool abs = false);
    PlotData(const XYStatData &data, const Index i = 0, const Index j = 0, 
             const bool abs = false);
    // destructor
    virtual ~PlotData(void) = default;
};

class PlotHLine: public PlotObject
{
public:
    // constructor
    PlotHLine(const double y);
    // destructor
    virtual ~PlotHLine(void) = default;
};

class PlotLine: public PlotObject
{
public:
    // constructor
    PlotLine(const DVec &x, const DVec &y);
    // destructor
    virtual ~PlotLine(void) = default;
};

class PlotPoints: public PlotObject
{
public:
    // constructor
    PlotPoints(const DVec &x, const DVec &y);
    // destructor
    virtual ~PlotPoints(void) = default;
};

class PlotBand: public PlotObject
{
public:
    // constructor
    PlotBand(const double xMin, const double xMax, const double yMin,
             const double yMax, const double opacity = 0.15);
    // destructor
    virtual ~PlotBand(void) = default;
};

class PlotFunction: public PlotObject
{
public:
    // constructor
    PlotFunction(const DoubleFunction &function, const double xMin,
                 const double xMax, const unsigned int nPoint = 1000, 
                 const bool abs = false);
    // destructor
    virtual ~PlotFunction(void) = default;
};

class PlotPredBand: public PlotObject
{
public:
    // constructor
    PlotPredBand(const DVec &x, const DVec &y, const DVec &yerr,
                 const double opacity = 0.15);
    PlotPredBand(const DoubleFunctionSample &function, const double xMin,
                 const double xMax, const unsigned int nPoint = 1000,
                 const double opacity = 0.15);
    // destructor
    virtual ~PlotPredBand(void) = default;
private:
    void makePredBand(const DMat &low, const DMat &high, const double opacity);
};

class PlotHistogram: public PlotObject
{
public:
    // constructor
    PlotHistogram(const Histogram &h);
    // destructor
    virtual ~PlotHistogram(void) = default;
};

class PlotImpulses: public PlotObject
{
public:
    // constructor
    PlotImpulses(const DVec &x, const DVec &y);
    // destructor
    virtual ~PlotImpulses(void) = default;
};

class PlotMatrixNoRange: public PlotObject
{
public:
    // constructor
    PlotMatrixNoRange(const DMat &m);
    // destructor
    virtual ~PlotMatrixNoRange(void) = default;
};

#define PlotMatrix(m)\
Latan::PlotRange(Latan::Axis::x, -.5, (m).cols() - .5) <<\
Latan::PlotRange(Latan::Axis::y, (m).rows() - .5, -.5) <<\
Latan::PlotMatrixNoRange(m)

#define PlotCorrMatrix(m)\
PlotHeadCommand("set cbrange [-1:1]") <<\
PlotHeadCommand("set palette defined (0 'blue', 1 'white', 2 'red')") <<\
PlotMatrix(m)

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
    std::string              terminal;
    std::string              output;
    std::string              caption;
    std::string              title;
    unsigned int             scaleMode[2];
    Range                    scale[2];
    std::string              label[2];
    std::string              lineColor;
    int                      lineWidth;
    std::string              dashType;
    std::vector<std::string> palette;
};

class PlotModifier
{
public:
    // destructor
    virtual ~PlotModifier(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const = 0;
};

class Caption: public PlotModifier
{
public:
    // constructor
    explicit Caption(const std::string &title);
    // destructor
    virtual ~Caption(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::string caption_;
};

class Label: public PlotModifier
{
public:
    // constructor
    explicit Label(const std::string &label, const Axis axis);
    // destructor
    virtual ~Label(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::string label_;
    const Axis        axis_;
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

class LineWidth: public PlotModifier
{
public:
    // constructor
    explicit LineWidth(const unsigned int width);
    // destructor
    virtual ~LineWidth(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const unsigned width_;
};

class Dash: public PlotModifier
{
public:
    // constructor
    explicit Dash(const std::string &dash);
    // destructor
    virtual ~Dash(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::string dash_;
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
    // constructors
    PlotRange(const Axis axis);
    PlotRange(const Axis axis, const double min, const double max);
    // destructor
    virtual ~PlotRange(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const Axis   axis_;
    const bool   reset_;
    const double min_, max_;
};

class Terminal: public PlotModifier
{
public:
    // constructor
    Terminal(const std::string &terminal, const std::string &options = "");
    // destructor
    virtual ~Terminal(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::string terminalCmd_;
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

class Palette: public PlotModifier
{
public:
    static const std::vector<std::string> category10;
public:
    // constructor
    explicit Palette(const std::vector<std::string> &palette);
    // destructor
    virtual ~Palette(void) = default;
    // modifier
    virtual void operator()(PlotOptions &option) const;
private:
    const std::vector<std::string> palette_;
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
    virtual ~Plot(void) = default;
    // plot operations
    Plot & operator<<(PlotObject   &&command);
    Plot & operator<<(PlotModifier &&modifier);
    // plot parsing and output
    void display(void);
    void save(std::string dirName, bool savePdf = true);
    friend std::ostream & operator<<(std::ostream &out, const Plot &plot);
    // plot reset
    void reset(void);
    // find gnuplot
    std::string getProgramPath(void);
private:
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
    std::vector<std::string> tmpFileName_;
    // plot content
    PlotOptions              options_;
    std::vector<std::string> headCommand_;
    std::vector<std::string> plotCommand_;
};

std::ostream & operator<<(std::ostream &out, const Plot &plot);

END_LATAN_NAMESPACE

#endif // Latan_Plot_hpp_
