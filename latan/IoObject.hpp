#ifndef LATAN_IOOBJECT_HPP_
#define LATAN_IOOBJECT_HPP_

#include <latan/Global.hpp>

LATAN_BEGIN_CPPDECL

// Abstract base for IO objects
class IoObject
{
public:
    class IoType
    {
    public:
        enum
        {
            noType = 0,
            dMat   = 1,
            sample = 2
        };
    };
public:
    virtual ~IoObject(void) {};
    virtual unsigned int getType(void) const = 0;
};

LATAN_END_CPPDECL

#endif
