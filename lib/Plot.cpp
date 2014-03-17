/*
 * Plot.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Plot.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Mat.hpp>

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

void PlotObject::setCommand(const string &command)
{
    command_ = command;
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
    char tmpFileName[MAX_PATH_LENGTH];
    int  fd;
    FILE *tmpFile;
    
    for (Index j = 0; j < m.cols(); ++j)
    {
    }
    strcpy(tmpFileName, "./latan_plot_tmp.XXXXXX.dat");
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

// PlotData constructor ////////////////////////////////////////////////////////
PlotData::PlotData(const XYStatData &data, const Index i, const Index j)
{
    DMat d(data.getNData(), 4);
    string usingCmd, tmpFileName;
    
    d.col(0)    = data.x(i);
    d.col(2)    = data.y(j);
    d.col(1)    = data.xxVar(i, i).diagonal().array().sqrt();
    d.col(3)    = data.yyVar(j, j).diagonal().array().sqrt();
    usingCmd    = (data.isXExact(i)) ? "u 1:3:4 w yerr" : "u 1:3:2:4 w xyerr";
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' " + usingCmd);
}

// PlotFunction constructor ////////////////////////////////////////////////////
PlotFunction::PlotFunction(const DoubleFunction &function, const double xMin,
                           const double xMax, const unsigned int nSample)
{
    DMat   d(nSample, 2);
    string tmpFileName;
    double dx = (xMax - xMin)/static_cast<double>(nSample - 1);
    
    for (Index i = 0; i < nSample; ++i)
    {
        d(i, 0) = xMin + i*dx;
        d(i, 1) = function(d(i, 0));
    }
    tmpFileName = dumpToTmpFile(d);
    pushTmpFile(tmpFileName);
    setCommand("'" + tmpFileName + "' u 1:2 w lines");
}

/******************************************************************************
 *                             Plot modifiers                                 *
 ******************************************************************************/
// Color constructor ///////////////////////////////////////////////////////////
Color::Color(const string &color)
: color_(color)
{}

// Color modifier //////////////////////////////////////////////////////////////
void Color::operator()(PlotOptions &option) const
{
    option.lineColor = color_;
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

// PlotRange constructor ////////////////////////////////////////////////////////
PlotRange::PlotRange(const Axis axis, const double min, const double max)
: axis_(axis)
, min_(min)
, max_(max)
{}

// PlotRange modifier ///////////////////////////////////////////////////////////
void PlotRange::operator()(PlotOptions &option) const
{
    int a = static_cast<int>(axis_);
    
    option.scaleMode[a] |= Plot::Scale::manual;
    option.scale[a].min  = min_;
    option.scale[a].max  = max_;
}

/******************************************************************************
 *                          Plot implementation                               *
 ******************************************************************************/
// destructor //////////////////////////////////////////////////////////////////
Plot::~Plot(void)
{
    while (!tmpFileName_.empty())
    {
        if (remove(tmpFileName_.top().c_str()))
        {
            LATAN_ERROR(System, "impossible to remove temporary file '" +
                        tmpFileName_.top() + "'");
        }
        tmpFileName_.pop();
    }
}

// plot objects ////////////////////////////////////////////////////////////////
Plot & Plot::operator<<(PlotObject &&command)
{
    string commandStr;
    
    while (command.gotTmpFile())
    {
        tmpFileName_.push(command.popTmpFile());
    }
    commandStr = command.getCommand();
    if (!options_.lineColor.empty())
    {
        commandStr         += " lc " + options_.lineColor;
        options_.lineColor  = "";
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
    
    return *this;
}

Plot & Plot::operator<<(PlotModifier &&modifier)
{
    modifier(options_);
    
    return *this;
}

// find gnuplot ////////////////////////////////////////////////////////////////
void Plot::getProgramPath(void)
{
    int         i, j, lg;
    char        *path;
    static char buf[MAX_PATH_LENGTH];
    
    /* Trivial case: try in CWD */
    sprintf(buf,"./%s", gnuplotBin_.c_str()) ;
    if (access(buf, X_OK) == 0)
    {
        sprintf(buf,".");
        gnuplotPath_ = buf;
    }
    /* Try out in all paths given in the PATH variable */
    else
    {
        buf[0] = 0;
        path = getenv("PATH") ;
        if (path)
        {
            for (i=0;path[i];)
            {
                for (j=i;(path[j])&&(path[j]!=':');j++);
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
                    /* Found it! */
                    break ;
                }
                buf[0] = 0;
                i = j;
                if (path[i] == ':') i++ ;
            }
        }
        else
        {
            LATAN_ERROR(System, "PATH variable not set");
        }
        /* If the buffer is still empty, the command was not found */
        if (buf[0] == 0)
        {
            LATAN_ERROR(System, "cannot find gnuplot in $PATH");
        }
        /* Otherwise truncate the command name to yield path only */
        lg = (int)(strlen(buf) - 1);
        while (buf[lg]!='/')
        {
            buf[lg] = 0;
            lg--;
        }
        buf[lg] = 0;
        gnuplotPath_ = buf;
    }
}

// plot parsing and output /////////////////////////////////////////////////////
void Plot::display(void)
{
    std::string command;
    FILE *gnuplotPipe;
    
    if (!getenv("DISPLAY"))
    {
        LATAN_ERROR(System, "cannot find DISPLAY variable: is it set ?");
    }
    getProgramPath();
    command     = gnuplotPath_ + "/" + gnuplotBin_ + " " + gnuplotArgs_;
    gnuplotPipe = popen(command.c_str(), "w");
    if (!gnuplotPipe)
    {
        LATAN_ERROR(System, "error starting gnuplot (command was '" + command
                    + "')");
    }
    commandBuffer_.str("");
    commandBuffer_ << *this;
    fprintf(gnuplotPipe, "%s", commandBuffer_.str().c_str());
    if (pclose(gnuplotPipe) == -1)
    {
        LATAN_ERROR(System, "problem closing communication to gnuplot");
    }
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
        out << "set output '" << plot.options_.terminal << "'" << endl;
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
    if (!plot.options_.title.empty())
    {
        out << "set title '" << plot.options_.title << "'" << endl;
    }
    if (plot.options_.scaleMode[x] & Plot::Scale::manual)
    {
        out << "set xrange [xMin:xMax]" << endl;
    }
    if (plot.options_.scaleMode[y] & Plot::Scale::manual)
    {
        out << "set yrange [yMin:yMax]" << endl;
    }
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
