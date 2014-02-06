#ifndef LATAN_SAMPLE_HPP_
#define	LATAN_SAMPLE_HPP_

#include <latan/Global.hpp>
#include <iostream>
#include <string>

LATAN_BEGIN_CPPDECL

template <typename DataObj>
class ParserState
{
public:
    // constructor
    explicit ParserState(std::istream *streamPt, std::string *namePt,
                         DataObj *dataPt);
    // destructor
    virtual ~ParserState(void);
private:
    // scanner allocation/deallocation
    virtual void initScanner(void)    = 0;
    virtual void destroyScanner(void) = 0;
public:
    DataObj*      data;
    void*         scanner;
    std::istream* stream;
    std::string*  streamName;
    
};

template <typename DataObj>
ParserState<DataObj>::ParserState(std::istream *streamPt, std::string *namePt,
                                  DataObj *dataPt)
: data(dataPt)
, scanner(NULL)
, stream(streamPt)
, streamName(namePt)
{}

template <typename DataObj>
ParserState<DataObj>::~ParserState(void)
{}

LATAN_END_CPPDECL

#endif
