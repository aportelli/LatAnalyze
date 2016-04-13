/*
 * ParserState.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
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

#ifndef Latan_ParserState_hpp_
#define	Latan_ParserState_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

template <typename DataObj>
class ParserState
{
public:
    // constructor
    ParserState(std::istream *streamPt, std::string *namePt, DataObj *dataPt);
    // destructor
    virtual ~ParserState(void) = default;
private:
    // scanner allocation/deallocation
    virtual void initScanner(void)    = 0;
    virtual void destroyScanner(void) = 0;
public:
    DataObj      *data;
    void         *scanner;
    std::istream *stream;
    std::string  *streamName;
    
};

template <typename DataObj>
ParserState<DataObj>::ParserState(std::istream *streamPt, std::string *namePt,
                                  DataObj *dataPt)
: data(dataPt)
, scanner(nullptr)
, stream(streamPt)
, streamName(namePt)
{}

END_LATAN_NAMESPACE

#endif // Latan_ParserState_hpp_
