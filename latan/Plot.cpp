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

#define _POSIX_C_SOURCE 199209L

#include <latan/Plot.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                         PlotCommand implementation                         *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
PlotCommand::PlotCommand(void)
{}

PlotCommand::PlotCommand(const string &command)
: command_(command)
{}

// access //////////////////////////////////////////////////////////////////////
const std::string & PlotCommand::getCommand(void) const
{
    return command_;
}

/******************************************************************************
 *                          Plot implementation                               *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
Plot::Plot(void)
: gnuplotBin_(GNUPLOT_BIN)
, gnuplotArgs_(GNUPLOT_ARGS)
, gnuplotPath_("")
, terminal_("")
, title_("")
{
    scaleMode_[Axis::x] = 0;
    scaleMode_[Axis::y] = 0;
    scale_[Axis::x].min = 0.0;
    scale_[Axis::x].max = 0.0;
    scale_[Axis::y].min = 0.0;
    scale_[Axis::y].max = 0.0;
    label_[Axis::x]     = "";
    label_[Axis::y]     = "";
}

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
Plot & Plot::operator<<(const PlotCommand &command)
{
    plotCommand_.push_back(command.getCommand());
    
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
    
    if (!plot.terminal_.empty())
    {
        out << "set term " << plot.terminal_ << endl;
    }
    if (!plot.output_.empty())
    {
        out << "set output '" << plot.terminal_ << "'" << endl;
    }
    if (plot.scaleMode_[Plot::Axis::x] & Plot::Scale::manual)
    {
        out << "xMin = " << plot.scale_[Plot::Axis::x].min << endl;
        out << "xMax = " << plot.scale_[Plot::Axis::x].max << endl;
    }
    if (plot.scaleMode_[Plot::Axis::y] & Plot::Scale::manual)
    {
        out << "yMin = " << plot.scale_[Plot::Axis::y].min << endl;
        out << "yMax = " << plot.scale_[Plot::Axis::y].max << endl;
    }
    if (!plot.title_.empty())
    {
        out << "set title '" << plot.title_ << "'" << endl;
    }
    if (plot.scaleMode_[Plot::Axis::x] & Plot::Scale::manual)
    {
        out << "set xrange [xMin:xMax]" << endl;
    }
    if (plot.scaleMode_[Plot::Axis::y] & Plot::Scale::manual)
    {
        out << "set yrange [yMin:yMax]" << endl;
    }
    if (plot.scaleMode_[Plot::Axis::x] & Plot::Scale::log)
    {
        out << "set log x" << endl;
    }
    if (plot.scaleMode_[Plot::Axis::y] & Plot::Scale::log)
    {
        out << "set log y" << endl;
    }
    if (!plot.label_[Plot::Axis::x].empty())
    {
        out << "set xlabel '" << plot.label_[Plot::Axis::x] << "'" << endl;
    }
    if (!plot.label_[Plot::Axis::y].empty())
    {
        out << "set ylabel '" << plot.label_[Plot::Axis::y] << "'" << endl;
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
