/*
 * AsciiFile.hpp, part of LatAnalyze 3
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

#ifndef Latan_AsciiFile_hpp_
#define Latan_AsciiFile_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Io/File.hpp>
#include <LatAnalyze/Core/Mat.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>
#include <LatAnalyze/Core/ParserState.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                          ASCII datafile class                              *
 ******************************************************************************/
class AsciiFile: public File
{
public:
    class AsciiParserState: public ParserState<IoDataTable>
    {
    public:
        // constructor
        AsciiParserState(std::istream *stream, std::string *name,
                         IoDataTable *data);
        // destructor
        virtual ~AsciiParserState(void);
        // first element reference
        bool        isFirst;
        std::string first;
        // parsing buffers
        int                intBuf;
        DSample            dSampleBuf;
        DMatSample         dMatSampleBuf;
        std::queue<DMat>   dMatQueue;
        std::queue<double> doubleQueue;
    private:
        // allocation/deallocation functions defined in IoAsciiLexer.lpp
        virtual void initScanner(void);
        virtual void destroyScanner(void);
    };
public:
    // constructors
    AsciiFile(void) = default;
    AsciiFile(const std::string &name, const unsigned int mode);
    // destructor
    virtual ~AsciiFile(void);
    // access
    virtual void save(const DMat &m, const std::string &name);
    virtual void save(const DSample &ds, const std::string &name);
    virtual void save(const DMatSample &ms, const std::string &name);
    // read first name
    virtual std::string getFirstName(void);
    // tests
    virtual bool isOpen(void) const;
    // IO
    virtual void close(void);
    virtual void open(const std::string &name, const unsigned int mode);
public:
    // default ASCII precision
    static const unsigned int defaultDoublePrec = 15;
private:
    // IO
    virtual std::string load(const std::string &name = "");
    // parser
    void parse(void);
private:
    std::fstream                      fileStream_;
    bool                              isParsed_{false};
    std::unique_ptr<AsciiParserState> state_{nullptr};
};

END_LATAN_NAMESPACE

#endif // Latan_AsciiFile_hpp_
