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
    explicit ParserState(std::istream* pt_stream, std::string* pt_name,\
                         DataObj* pt_data);
    // destructor
    virtual ~ParserState(void);
    // public members
    DataObj*      data;
    void*         scanner;
    std::istream* stream;
    std::string*  stream_name;
};

template <typename DataObj>
ParserState<DataObj>::ParserState(std::istream* pt_stream,\
                                  std::string* pt_name,   \
                                  DataObj* pt_data)
: data(pt_data), scanner(NULL), stream(pt_stream), stream_name(pt_name)
{}

template <typename DataObj>
ParserState<DataObj>::~ParserState(void)
{}

LATAN_END_CPPDECL

#endif