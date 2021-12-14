/*
 * Plot.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Core/Mat.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                              Plot objects                                  *
 ******************************************************************************/
// PlotObject access ///////////////////////////////////////////////////////////
const string & PlotObject::getCommand(void) const
{
    return command_;
}

const string & PlotObject::getHeadCommand(void) const
{
    return headCommand_;
}

void PlotObject::setCommand(const string &command)
{
    command_ = command;
}

void PlotObject::setHeadCommand(const string &command)
{
    headCommand_ = command;
}

string PlotObject::popTmpFile(void)
{
    string res = tmpFileName_.top();
    
    tmpFileName_.pop();
    
    return res;
}

void PlotObject::pushTmpFile(const std::string &fileName)
{
    tmpFileName_.push(fileName);
}

// PlotObject dump a matrix to a temporary file ////////////////////////////////
string PlotObject::dumpToTmpFile(const DMat &m)
{
    char   tmpFileName[MAX_PATH_LENGTH];
    int    fd;
    FILE   *tmpFile;
    
    for (Index j = 0; j < m.cols(); ++j)
    {
    }
    sprintf(tmpFileName, "%s/latan_plot_tmp.XXXXXX.dat", P_tmpdir);
    fd = mkstemps(tmpFileName, 4);
    if (fd == -1)
    {
        LATAN_ERROR(System, "impossible to create a temporary file from template "
                    + strFrom(tmpFileName));
    }
    tmpFile = fdopen(fd, "w");
    for (Index i = 0; i < m.rows(); ++i)
    {
        for (Index j = 0; j < m.cols(); ++j)
        {
            fprintf(tmpFile, "%e ", m(i, j));
        }
        fprintf(tmpFile, "\n");
    }
    fclose(tmpFile);
    
    return string(tmpFileName);
}

// PlotObject test /////////////////////////////////////////////////////////////
bool PlotObject::gotTmpFile(void) const
{
    return !tmpFileName_.empty();
}

// PlotCommand constructor /////////////////////////////////////////////////////
PlotCommand::PlotCommand(const string &command)
{
    setCommand(command);
}

// PlotHeadCommand constructor /////////////////////////////////////////////////
PlotHeadCommand::PlotHeadCommand(const string &command)
{
    setHeadCommand(command);
}

// PlotData constructor ////////////////////////////////////////////////////////
PlotData::PlotData(const DMatSample &x, const DMatSample &y, const bool abs)
{
    if (x[central].rows() != y[central].rows())
    {
        LATAN_ERROR(Size, "x and y vectors do not have the same size");
    }

    DMat d(x[central].rows(), 4);
    string usingCmd, tmpFileName;

    d.col(0)    = x[central];
    d.col(2)    = y[central];
    d.col(1)    = x.variance().cwiseSqrt();
    d.col(3)    = y.variance().cwiseSqrt();
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    if (!abs)
    {
        setCommand("'" + tmpFileName + "' u 1:3:2:4 w xyerr");
    }
    else
    {
        setCommand("'" + tmpFileName + "' u 1:(abs($3)):2:4 w xyerr");
    }
}

PlotData::PlotData(const DVec &x, const DMatSample &y, const bool abs)
{
    if (x.rows() != y[central].rows())
    {
        LATAN_ERROR(Size, "x and y vector does not have the same size");
    }

    DMat d(x.rows(), 3);
    string usingCmd, tmpFileName;

    d.col(0)    = x;
    d.col(1)    = y[central];
    d.col(2)    = y.variance().cwiseSqrt();
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    if (!abs)
    {
        setCommand("'" + tmpFileName + "' u 1:2:3 w yerr");
    }
    else
    {
        setCommand("'" + tmpFileName + "' u 1:(abs($2)):3 w yerr");
    }
}

PlotData::PlotData(const DMatSample &x, const DVec &y, const bool abs)
{
    if (x[central].rows() != y.rows())
    {
        LATAN_ERROR(Size, "x and y vectors do not have the same size");
    }

    DMat d(x[central].rows(), 3), xerr, yerr;
    string usingCmd, tmpFileName;

    d.col(0)    = x[central];
    d.col(2)    = y;
    d.col(1)    = x.variance().cwiseSqrt();
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    if (!abs)
    {
        setCommand("'" + tmpFileName + "' u 1:3:2 w xerr");
    }
    else
    {
        setCommand("'" + tmpFileName + "' u 1:($3):2 w xerr");
    }
}

PlotData::PlotData(const XYStatData &data, const Index i, const Index j, const bool abs)
{
    string usingCmd, tmpFileName;
    
    if (!abs)
    {
        usingCmd = (data.isXExact(i)) ? "u 1:3:4 w yerr" : "u 1:3:2:4 w xyerr";
    }
    else
    {
        usingCmd = (data.isXExact(i)) ? "u 1:(abs($3)):4 w yerr" : "u 1:(abs($3)):2:4 w xyerr";
    }
   
    tmpFileName = dumpToTmpFile(data.getTable(i, j));
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' " + usingCmd);
}

// PlotLine constructor ////////////////////////////////////////////////////////
PlotLine::PlotLine(const DVec &x, const DVec &y)
{
    if (x.size() != y.size())
    {
        LATAN_ERROR(Size, "x and y vectors do not have the same size");
    }

    DMat d(x.size(), 2);
    string usingCmd, tmpFileName;

    d.col(0)    = x;
    d.col(1)    = y;
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' u 1:2 w lines");
}

// PlotHLine constructor ///////////////////////////////////////////////////////
PlotHLine::PlotHLine(const double y)
{
    setCommand(strFrom(y));
}

// PlotHBand constructor ///////////////////////////////////////////////////////
PlotBand::PlotBand(const double xMin, const double xMax, const double yMin,
                   const double yMax, const double opacity)
{
    setCommand("'< printf \"%e %e\\n%e %e\\n%e %e\\n%e %e\\n%e %e\\n\" "
               + strFrom(xMin) + " " + strFrom(yMin) + " "
               + strFrom(xMax) + " " + strFrom(yMin) + " "
               + strFrom(xMax) + " " + strFrom(yMax) + " "
               + strFrom(xMin) + " " + strFrom(yMax) + " "
               + strFrom(xMin) + " " + strFrom(yMin)
               + "' u 1:2 w filledcurves closed fs solid " + strFrom(opacity)
               + " noborder");
}

// PlotFunction constructor ////////////////////////////////////////////////////
PlotFunction::PlotFunction(const DoubleFunction &function, const double xMin,
                           const double xMax, const unsigned int nPoint,
                           const bool abs)
{
    DMat   d(nPoint, 2);
    string tmpFileName;
    double dx = (xMax - xMin)/static_cast<double>(nPoint - 1);
    
    for (Index i = 0; i < nPoint; ++i)
    {
        d(i, 0) = xMin + i*dx;
        d(i, 1) = function(d(i, 0));
    }
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    if (!abs)
    {
        setCommand("'" + tmpFileName + "' u 1:2 w lines");
    }
    else
    {
        setCommand("'" + tmpFileName + "' u 1:(abs($2)) w lines");
    }
}

// PlotPredBand constructor ////////////////////////////////////////////////////
void PlotPredBand::makePredBand(const DMat &low, const DMat &high, const double opacity)
{
    string lowFileName, highFileName, contFileName;
    DMat   contour(low.rows() + high.rows() + 1, 2);

    FOR_MAT(low, i, j)
    {
        contour(i, j) = low(i, j);
    }
    FOR_MAT(high, i, j)
    {
        contour(low.rows() + i, j) = high(high.rows() - i - 1, j);
    }
    contour.row(low.rows() + high.rows()) = low.row(0);
    contFileName = dumpToTmpFile(contour);
    pushTmpFile(contFileName);
    setCommand("'" + contFileName + "' u 1:2 w filledcurves closed" +
               " fs solid " + strFrom(opacity) + " noborder");
}

PlotPredBand::PlotPredBand(const DVec &x, const DVec &y, const DVec &yerr,
                           const double opacity)
{
    if (x.size() != y.size())
    {
        LATAN_ERROR(Size, "x and y vectors do not have the same size");
    }
    if (y.size() != yerr.size())
    {
        LATAN_ERROR(Size, "y and y error vectors do not have the same size");
    }

    Index nPoint = x.size();
    DMat  dLow(nPoint, 2), dHigh(nPoint, 2);

    dLow.col(0)  = x;
    dLow.col(1)  = y - yerr;
    dHigh.col(0) = x;
    dHigh.col(1) = y + yerr;
    makePredBand(dLow, dHigh, opacity);
}

PlotPredBand::PlotPredBand(const DoubleFunctionSample &function,
                           const double xMin, const double xMax,
                           const unsigned int nPoint, const double opacity)
{
    DMat       dLow(nPoint, 2), dHigh(nPoint, 2);
    DSample    pred(function.size());
    double     dx = (xMax - xMin)/static_cast<double>(nPoint - 1);
    string     lowFileName, highFileName;
    
    for (Index i = 0; i < nPoint; ++i)
    {
        double x = xMin + i*dx, err;
        
        pred        = function(x);
        err         = sqrt(pred.variance());
        dLow(i, 0)  = x;
        dLow(i, 1)  = pred[central] - err;
        dHigh(i, 0) = x;
        dHigh(i, 1) = pred[central] + err;
    }
    makePredBand(dLow, dHigh, opacity);
}


// PlotHistogram constructor ///////////////////////////////////////////////////
PlotHistogram::PlotHistogram(const Histogram &h)
{
    DMat   d(h.size(), 2);
    string tmpFileName;

    for (Index i = 0; i < h.size(); ++i)
    {
        d(i, 0) = h.getX(i);
        d(i, 1) = h[i];
    }
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' u 1:2 w steps");
}

// PlotImpulses constructor ////////////////////////////////////////////////////
PlotImpulses::PlotImpulses(const DVec &x, const DVec &y)
{
    if (x.rows() != y.rows())
    {
        LATAN_ERROR(Size, "x and y vector does not have the same size");
    }

    DMat   d(x.rows(), 2);
    string tmpFileName;

    for (Index i = 0; i < x.rows(); ++i)
    {
        d(i, 0) = x(i);
        d(i, 1) = y(i);
    }
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' u 1:2 w impulses");
}

// PlotMatrixNoRange constructor ///////////////////////////////////////////////
PlotMatrixNoRange::PlotMatrixNoRange(const DMat &m)
{
    string tmpFileName = dumpToTmpFile(m);
    
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' matrix w image");
}

/******************************************************************************
 *                             Plot modifiers                                 *
 ******************************************************************************/
// Caption constructor /////////////////////////////////////////////////////////
Caption::Caption(const string &caption)
: caption_(caption)
{}

// Caption modifier ////////////////////////////////////////////////////////////
void Caption::operator()(PlotOptions &option) const
{
    option.caption = caption_;
}

// Label constructor ///////////////////////////////////////////////////////////
Label::Label(const string &label, const Axis axis)
: label_(label)
, axis_(axis)
{}

// Label modifier //////////////////////////////////////////////////////////////
void Label::operator()(PlotOptions &option) const
{
    option.label[static_cast<int>(axis_)] = label_;
}

// Color constructor ///////////////////////////////////////////////////////////
Color::Color(const string &color)
: color_(color)
{}

// Color modifier //////////////////////////////////////////////////////////////
void Color::operator()(PlotOptions &option) const
{
    option.lineColor = color_;
}

// LineWidth constructor ///////////////////////////////////////////////////////
LineWidth::LineWidth(const unsigned int width)
: width_(width)
{}

// LineWidth modifier //////////////////////////////////////////////////////////
void LineWidth::operator()(PlotOptions &option) const
{
    option.lineWidth = static_cast<int>(width_);
}

// Dash constructor ///////////////////////////////////////////////////////////
Dash::Dash(const string &dash)
: dash_(dash)
{}

// Dash modifier //////////////////////////////////////////////////////////////
void Dash::operator()(PlotOptions &option) const
{
    option.dashType = dash_;
}

// LogScale constructor ////////////////////////////////////////////////////////
LogScale::LogScale(const Axis axis)
: axis_(axis)
{}

// Logscale modifier ///////////////////////////////////////////////////////////
void LogScale::operator()(PlotOptions &option) const
{
    option.scaleMode[static_cast<int>(axis_)] |= Plot::Scale::log;
}

// PlotRange constructors //////////////////////////////////////////////////////
PlotRange::PlotRange(const Axis axis)
: axis_(axis)
, reset_(true)
, min_(0.)
, max_(0.)
{}

PlotRange::PlotRange(const Axis axis, const double min, const double max)
: axis_(axis)
, reset_(false)
, min_(min)
, max_(max)
{}

// PlotRange modifier ///////////////////////////////////////////////////////////
void PlotRange::operator()(PlotOptions &option) const
{
    int a = static_cast<int>(axis_);
    
    if (!reset_)
    {
        option.scaleMode[a] |= Plot::Scale::manual;
        option.scale[a].min  = min_;
        option.scale[a].max  = max_;
    }
    else
    {
        option.scaleMode[a] = Plot::Scale::reset;
    }
}

// Terminal constructor ////////////////////////////////////////////////////////
Terminal::Terminal(const string &terminal, const std::string &options)
: terminalCmd_(terminal + " " + options)
{}

// Terminal modifier ///////////////////////////////////////////////////////////
void Terminal::operator()(PlotOptions &option) const
{
    option.terminal = terminalCmd_;
}

// Title constructor ///////////////////////////////////////////////////////////
Title::Title(const string &title)
: title_(title)
{}

// Title modifier //////////////////////////////////////////////////////////////
void Title::operator()(PlotOptions &option) const
{
    option.title = title_;
}

// Palette constructor /////////////////////////////////////////////////////////
Palette::Palette(const std::vector<std::string> &palette)
: palette_(palette)
{}

// Palette modifier ////////////////////////////////////////////////////////////
void Palette::operator()(PlotOptions &option) const
{
    option.palette = palette_;
}

// category10 palette //////////////////////////////////////////////////////////
const std::vector<std::string> Palette::category10 =
{
    "rgb '#1f77b4'",
    "rgb '#ff7f0e'",
    "rgb '#2ca02c'",
    "rgb '#d62728'",
    "rgb '#9467bd'",
    "rgb '#8c564b'",
    "rgb '#e377c2'",
    "rgb '#7f7f7f'",
    "rgb '#bcbd22'"
};

/******************************************************************************
 *                          Plot implementation                               *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
Plot::Plot(void)
{
    initOptions();
}

// default options /////////////////////////////////////////////////////////////
void Plot::initOptions(void)
{
    options_.terminal     = "qt noenhanced font 'Arial,12'";
    options_.output       = "";
    options_.caption      = "";
    options_.title        = "";
    options_.scaleMode[0] = Plot::Scale::reset;
    options_.scaleMode[1] = Plot::Scale::reset;
    options_.scale[0]     = {0.0, 0.0};
    options_.scale[1]     = {0.0, 0.0};
    options_.label[0]     = "";
    options_.label[1]     = "";
    options_.lineColor    = "";
    options_.lineWidth    = -1;
    options_.dashType     = "";
    options_.palette      = Palette::category10;
}

// plot reset //////////////////////////////////////////////////////////////////
void Plot::reset(void)
{
    headCommand_.clear();
    plotCommand_.clear();
    tmpFileName_.clear();
    initOptions();
}

// plot objects ////////////////////////////////////////////////////////////////
Plot & Plot::operator<<(PlotObject &&command)
{
    string commandStr;
    
    while (command.gotTmpFile())
    {
        tmpFileName_.push_back(command.popTmpFile());
        commandStr += "'" + tmpFileName_.back() + "' ";
    }
    commandStr = command.getCommand();
    if (!commandStr.empty())
    {
        if (!options_.lineColor.empty())
        {
            commandStr         += " lc " + options_.lineColor;
            options_.lineColor  = "";
        }
        if (options_.lineWidth > 0)
        {
            commandStr         += " lw " + strFrom(options_.lineWidth);
            options_.lineWidth  = -1;
        }
        if (!options_.dashType.empty())
        {
            commandStr        += " dt " + options_.dashType;
            options_.dashType  = "";
        }
        if (options_.title.empty())
        {
            commandStr += " notitle";
        }
        else
        {
            commandStr     += " t '" + options_.title + "'";
            options_.title  = "";
        }
        plotCommand_.push_back(commandStr);
    }
    if (!command.getHeadCommand().empty())
    {
        headCommand_.push_back(command.getHeadCommand());
    }
    
    return *this;
}

Plot & Plot::operator<<(PlotModifier &&modifier)
{
    modifier(options_);
    
    return *this;
}

// find gnuplot ////////////////////////////////////////////////////////////////
#define SEARCH_DIR(dir) \
sprintf(buf, "%s/%s", dir, gnuplotBin_.c_str());\
if (access(buf, X_OK) == 0)\
{\
    return dir;\
}

std::string Plot::getProgramPath(void)
{
    int         i, j, lg;
    char        *path;
    static char buf[MAX_PATH_LENGTH];
    
    // try out in all paths given in the PATH variable
    buf[0] = 0;
    path = getenv("PATH") ;
    if (path)
    {
        for (i=0;path[i];)
        {
            for (j=i;(path[j]) and (path[j]!=':');j++);
            lg = j - i;
            strncpy(buf,path + i,(size_t)(lg));
            if (lg == 0)
            {
                buf[lg++] = '.';
            }
            buf[lg++] = '/';
            strcpy(buf + lg, gnuplotBin_.c_str());
            if (access(buf, X_OK) == 0)
            {
                // found it!
                break ;
            }
            buf[0] = 0;
            i = j;
            if (path[i] == ':') i++ ;
        }
    }
    // if the buffer is still empty, the command was not found
    if (buf[0] != 0)
    {
        lg = (int)(strlen(buf) - 1);
        while (buf[lg]!='/')
        {
            buf[lg] = 0;
            lg--;
        }
        buf[lg] = 0;
        gnuplotPath_ = buf;

        return gnuplotPath_;
    }

    // try in CWD, /usr/bin & /usr/local/bin
    SEARCH_DIR(".");
    SEARCH_DIR("/usr/bin");
    SEARCH_DIR("/usr/local/bin");

    // if this code is reached nothing was found
    LATAN_ERROR(System, "cannot find gnuplot");

    return "";
}

// plot parsing and output /////////////////////////////////////////////////////
void Plot::display(void)
{
    int pid = fork();

    if (pid == 0)
    {
        FILE          *gnuplotPipe;
        string        command;
        ostringstream scriptBuf;

        getProgramPath();
        command     = gnuplotPath_ + "/" + gnuplotBin_ + " 2>/dev/null";
        gnuplotPipe = popen(command.c_str(), "w");
        if (!gnuplotPipe)
        {
            LATAN_ERROR(System, "error starting gnuplot (command was '"
                        + command + "')");
        }
        commandBuffer_.str("");
        commandBuffer_ << *this << endl;
        commandBuffer_ << "pause mouse close" << endl;
        fprintf(gnuplotPipe, "%s", commandBuffer_.str().c_str());
        if (pclose(gnuplotPipe) == -1)
        {
            LATAN_ERROR(System, "problem closing communication to gnuplot");
        }

        exit(EXIT_SUCCESS);
    }
    else if (pid == -1)
    {
        perror("fork error");
        LATAN_ERROR(System, "problem forking to the process handling gnuplot");
    }
}

void Plot::save(string dirName, bool savePdf)
{
    vector<string> commandBack;
    string         path, terminalBack, outputBack, gpCommand, scriptName;
    mode_t         mode755;
    ofstream       script;
    
    mode755 = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;

    // generate directory
    if (mkdir(dirName))
    {
        LATAN_ERROR(Io, "impossible to create directory '" + dirName + "'");
    }
    
    // backup I/O parameters
    terminalBack = options_.terminal;
    outputBack   = options_.output;
    commandBack  = plotCommand_;

    // save PDF
    if (savePdf)
    {
        options_.terminal = "pdf";
        options_.output   = dirName + "/plot.pdf";
        display();
        options_.terminal = terminalBack;
        options_.output   = outputBack;
    }
    
    // save script and datafiles
    for (unsigned int i = 0; i < tmpFileName_.size(); ++i)
    {
        ofstream dataFile;
        ifstream tmpFile;
        string   dataFileName = "points_" + strFrom(i) + ".dat";
        
        dataFile.open(dirName + "/" + dataFileName);
        tmpFile.open(tmpFileName_[i]);
        dataFile << tmpFile.rdbuf();
        dataFile.close();
        tmpFile.close();
        for (string &command: plotCommand_)
        {
            auto pos = command.find(tmpFileName_[i]);
            
            while (pos != string::npos)
            {
                command.replace(pos, tmpFileName_[i].size(), dataFileName);
                pos = command.find(tmpFileName_[i], pos + 1);
            }
        }
    }
    scriptName = dirName + "/source.plt";
    script.open(scriptName);
    getProgramPath();
    gpCommand = gnuplotPath_ + "/" + gnuplotBin_ + " " + gnuplotArgs_;
    script << "#!/usr/bin/env " << gpCommand << "\n" << endl;
    script << "# script generated by " << Env::fullName << "\n" << endl;
    script << *this;
    script.close();
    if (chmod(scriptName.c_str(), mode755))
    {
        LATAN_ERROR(Io, "impossible to set file '" + scriptName +
                    "' in mode 755");
    }
    plotCommand_ = commandBack;
}

ostream & Latan::operator<<(ostream &out, const Plot &plot)
{
    std::string begin, end;
    int x = static_cast<int>(Axis::x), y = static_cast<int>(Axis::y);
    
    if (!plot.options_.terminal.empty())
    {
        out << "set term " << plot.options_.terminal << endl;
    }
    if (!plot.options_.output.empty())
    {
        out << "set output '" << plot.options_.output << "'" << endl;
    }
    if (!plot.options_.caption.empty())
    {
        out << "set title '" << plot.options_.caption << "'" << endl;
    }
    if (plot.options_.scaleMode[x] & Plot::Scale::manual)
    {
        out << "xMin = " << plot.options_.scale[x].min << endl;
        out << "xMax = " << plot.options_.scale[x].max << endl;
    }
    if (plot.options_.scaleMode[y] & Plot::Scale::manual)
    {
        out << "yMin = " << plot.options_.scale[y].min << endl;
        out << "yMax = " << plot.options_.scale[y].max << endl;
    }
    out << "unset xrange" << endl;
    if (plot.options_.scaleMode[x] & Plot::Scale::manual)
    {
        out << "set xrange [xMin:xMax]" << endl;
    }
    else
    {
        out << "set xrange [:]" << endl;
    }
    out << "unset yrange" << endl;
    if (plot.options_.scaleMode[y] & Plot::Scale::manual)
    {
        out << "set yrange [yMin:yMax]" << endl;
    }
    else
    {
        out << "set yrange [:]" << endl;
    }
    out << "unset log" << endl;
    if (plot.options_.scaleMode[x] & Plot::Scale::log)
    {
        out << "set log x" << endl;
    }
    if (plot.options_.scaleMode[y] & Plot::Scale::log)
    {
        out << "set log y" << endl;
    }
    if (!plot.options_.label[x].empty())
    {
        out << "set xlabel '" << plot.options_.label[x] << "'" << endl;
    }
    if (!plot.options_.label[y].empty())
    {
        out << "set ylabel '" << plot.options_.label[y] << "'" << endl;
    }
    for (unsigned int i = 0; i < plot.options_.palette.size(); ++i)
    {
        out << "set linetype " << i + 1 << " lc " 
            << plot.options_.palette[i] << endl;
    }
    for (unsigned int i = 0; i < plot.headCommand_.size(); ++i)
    {
        out << plot.headCommand_[i] << endl;
    }
    for (unsigned int i = 0; i < plot.plotCommand_.size(); ++i)
    {
        begin = (i == 0)                            ? "plot " : "     ";
        end   = (i == plot.plotCommand_.size() - 1) ? ""      : ",\\";
        out << begin << plot.plotCommand_[i] << end << endl;
    }
    
    return out;
}
