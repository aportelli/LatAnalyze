#include <latan/IOObject.hpp>
#include <latan/includes.hpp>

using namespace Latan;

IOObject::~IOObject(void)
{}

IOTypes::Type IOObject::IOType(void)
{
    return IOTypes::NoType;
}
