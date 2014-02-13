/*
 * Io.cpp, part of LatAnalyze 3
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

#include <latan/Io.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                          File implementation                               *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
File::File(void)
: name_("")
, mode_(Mode::null)
, data_()
{}

File::File(const string &name, const unsigned int mode)
: name_(name)
, mode_(mode)
, data_()
{}

// destructor //////////////////////////////////////////////////////////////////
File::~File(void)
{
    deleteData();
}

// access //////////////////////////////////////////////////////////////////////
string File::getName(void) const
{
    return name_;
}

unsigned int File::getMode(void) const
{
    return mode_;
}

// internal functions //////////////////////////////////////////////////////////
void File::deleteData(void)
{
    for (auto &i : data_)
    {
        i.second.reset();
    }
    data_.clear();
}

void File::checkWritability(void)
{
    if (!((mode_ & Mode::write)||(mode_ & Mode::append))||!isOpen())
    {
        LATAN_ERROR(Io, "file '" + name_ + "' is not writable");
    }
}

/******************************************************************************
 *                        AsciiFile implementation                            *
 ******************************************************************************/
// AsciiParserState constructor ////////////////////////////////////////////////
AsciiFile::AsciiParserState::AsciiParserState(istream *stream, string *name,
                                              IoDataTable *data)
: ParserState<IoDataTable>(stream, name, data)
{
    initScanner();
}

// AsciiParserState destructor /////////////////////////////////////////////////
AsciiFile::AsciiParserState::~AsciiParserState(void)
{
    destroyScanner();
}

// constructor /////////////////////////////////////////////////////////////////
AsciiFile::AsciiFile(void)
: File(), fileStream_()
, isParsed_(false)
, state_(nullptr)
{}

AsciiFile::AsciiFile(const string &name, const unsigned int mode)
{
    open(name, mode);
}

// destructor //////////////////////////////////////////////////////////////////
AsciiFile::~AsciiFile(void)
{
    close();
}

// access //////////////////////////////////////////////////////////////////////
void AsciiFile::save(const DMat &m, const std::string &name)
{
    checkWritability();
    isParsed_ = false;
    fileStream_ << "#L latan_begin mat " << name << endl;
    fileStream_ << m.cols() << endl;
    fileStream_ << m << endl;
    fileStream_ << "#L latan_end mat " << endl;
}

void AsciiFile::save(const RandGen::State &state, const std::string &name)
{
    checkWritability();
    isParsed_ = false;
    fileStream_ << "#L latan_begin rg_state " << name << endl;
    fileStream_ << state << endl;
    fileStream_ << "#L latan_end rg_state " << endl;
}

// tests ///////////////////////////////////////////////////////////////////////
bool AsciiFile::isOpen() const
{
    return fileStream_.is_open();
}

// IO //////////////////////////////////////////////////////////////////////////
void AsciiFile::close(void)
{
    state_.reset(nullptr);
    if (isOpen())
    {
        fileStream_.close();
    }
    name_     = "";
    mode_     = Mode::null;
    isParsed_ = false;
    deleteData();
}

void AsciiFile::open(const string &name, const unsigned int mode)
{
    if (isOpen())
    {
        LATAN_ERROR(Io, "file already opened with name '" + name_ + "'");
    }
    else
    {
        ios_base::openmode stdMode = static_cast<ios_base::openmode>(0);
        
        if (mode & Mode::write)
        {
            stdMode |= ios::out|ios::trunc;
        }
        if (mode & Mode::read)
        {
            stdMode |= ios::in;
        }
        if (mode & Mode::append)
        {
            stdMode |= ios::out|ios::app;
        }
        name_     = name;
        mode_     = mode;
        isParsed_ = false;
        fileStream_.open(name_.c_str(), stdMode);
        if (mode_ & Mode::read)
        {
            state_.reset(new AsciiParserState(&fileStream_, &name_, &data_));
        }
        else
        {
            state_.reset(nullptr);
        }
    }
}

void AsciiFile::load(const string &name __dumb)
{
    if ((mode_ & Mode::read)&&(isOpen()))
    {
        if (!isParsed_)
        {
            parse();
        }
    }
    else
    {
        if (isOpen())
        {
            LATAN_ERROR(Io, "file '" + name_ + "' is not opened in read mode");
        }
        else
        {
            LATAN_ERROR(Io, "file not opened");
        }
    }
}

// parser //////////////////////////////////////////////////////////////////////

// Bison/Flex parser declaration
int _ioAscii_parse(AsciiFile::AsciiParserState *state);

void AsciiFile::parse()
{
    fileStream_.seekg(0);
    _ioAscii_parse(state_.get());
    isParsed_ = true;
}
