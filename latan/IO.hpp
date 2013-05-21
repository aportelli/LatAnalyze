#ifndef LATAN_IO_HPP_
#define	LATAN_IO_HPP_

#include <fstream>
#include <map>
#include <stack>
#include <sstream>
#include <string>
#include <H5Cpp.h>
#include <latan/Global.hpp>
#include <latan/IOObject.hpp>
#include <latan/Mat.hpp>
#include <latan/Sample.hpp>

LATAN_BEGIN_CPPDECL

/******************************************************************************
 *                          Generic datafile class                            *
 ******************************************************************************/
typedef std::map<std::string,IOObject*> IODataTable;

namespace FileMode
{
    typedef enum
    {
        Null   = 0,
        Write  = 1 << 0,
        Read   = 1 << 1,
        Append = 1 << 2
    } Type;
}

class File
{
public:
    // constructors
    File(void);
    File(const std::string in_name, const FileMode::Type in_mode);
    // destructor
    virtual ~File(void);
    // access
    std::string Name(void) const;
    FileMode::Type Mode(void) const;
    // tests
    virtual bool IsOpen(void) const = 0;
    // IO
    virtual void Close(void) = 0;
    virtual void Open(const std::string new_name, const FileMode::Type new_mode) = 0;
    virtual void Save(void) = 0;
    virtual void SaveAs(const std::string new_name) = 0;
protected:
    // protected members
    std::string    name;
    FileMode::Type mode;
    IODataTable    data;
    // protected methods
    void DeleteData(void);
    template <typename IOObj> const IOObj& GetData(const std::string data_name);
};

// Template implementations
template <typename IOObj>
const IOObj& File::GetData(const std::string data_name)
{
    try
    {
        return dynamic_cast<const IOObj&>(*(data.at(data_name)));
    }
    catch (std::out_of_range& e)
    {
        LATAN_ERROR(Range,"no data with name '"+data_name+"'");
    }
}

/******************************************************************************
 *                          ASCII datafile class                              *
 ******************************************************************************/
class ASCIIParserState
{
public:
    // constructor
    explicit ASCIIParserState(std::istream* pt_stream, std::string* name,\
                              IODataTable* pt_data);
    // destructor
    virtual ~ASCIIParserState(void);
    // public members
    IODataTable*       data;
    std::stack<DMat>   dmat_buf;
    std::stack<double> double_buf;
    void*              scanner;
    std::istream*      stream;
    std::string*       stream_name;
private:
    // allocation/deallocation functions defined in IOASCIILexer.lpp
    void init_scanner(void);
    void destroy_scanner(void);
};

class ASCIIFile: public File
{
public:
    // constructors
    ASCIIFile(void);
    ASCIIFile(const std::string in_name, const FileMode::Type in_mode);
    // destructor
    virtual ~ASCIIFile(void);
    // access
    template <typename IOObj> const IOObj& Read(const std::string name);
    // tests
    virtual bool IsOpen(void) const;
    // IO
    virtual void Close(void);
    virtual void Open(const std::string new_name, const FileMode::Type new_mode);
    virtual void Save(void);
    virtual void SaveAs(const std::string new_name);
private:
    // private members
    std::fstream      file_stream;
    bool              is_parsed;
    ASCIIParserState* state;
    // private methods
    void Clear(void);
    void OpenASCII(const std::string in_name, const FileMode::Type in_mode);
    void CloseASCII(void);
    void Parse(void);
};

// Template implementations
template <typename IOObj>
const IOObj& ASCIIFile::Read(const std::string data_name)
{
    if ((mode & FileMode::Read)&&(IsOpen()))
    {
        if (!is_parsed)
        {
            Parse();
            is_parsed = true;
        }
        
        return GetData<IOObj>(data_name);
    }
    else
    {
        if (IsOpen())
        {
            LATAN_ERROR(IO,"file '"+name+"' is not opened in read mode");
        }
        else
        {
            LATAN_ERROR(IO,"file not opened");
        }
    }
}

LATAN_END_CPPDECL

#endif
