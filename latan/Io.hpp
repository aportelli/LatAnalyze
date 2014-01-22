#ifndef LATAN_IO_HPP_
#define	LATAN_IO_HPP_

#include <fstream>
#include <map>
#include <stack>
#include <sstream>
#include <string>
#include <latan/Global.hpp>
#include <latan/IoObject.hpp>
#include <latan/Mat.hpp>
#include <latan/ParserState.hpp>
#include <latan/Sample.hpp>

LATAN_BEGIN_CPPDECL

/******************************************************************************
 *                          Generic datafile class                            *
 ******************************************************************************/
typedef std::map<std::string, IoObject*> IoDataTable;

class File
{
public:
    class FileMode
    {
    public:
        enum
        {
            null   = 0,
            write  = 1 << 0,
            read   = 1 << 1,
            append = 1 << 2
        };
    };
public:
    // constructors
    File(void);
    File(const std::string name, const unsigned int mode);
    // destructor
    virtual ~File(void);
    // access
    std::string  getName(void) const;
    unsigned int getMode(void) const;
    // tests
    virtual bool isOpen(void) const = 0;
    // Io
    virtual void close(void)                                           = 0;
    virtual void open(const std::string name, const unsigned int mode) = 0;
    virtual void save(void)                                            = 0;
    virtual void saveAs(const std::string name)                        = 0;
protected:
    // data access
    void         deleteData(void);
    template <typename IoObj>
    const IoObj& getData(const std::string dataName);
protected:
    std::string  name_;
    unsigned int mode_;
    IoDataTable  data_;
};

// Template implementations
template <typename IoObj>
const IoObj& File::getData(const std::string dataName)
{
    if (data_.find(dataName) != data_.end())
    {
        return dynamic_cast<const IoObj&>(*(data_[dataName]));
    }
    else
    {
        LATAN_ERROR(Range, "no data with name '" + dataName + "'");
    }
}

/******************************************************************************
 *                          ASCII datafile class                              *
 ******************************************************************************/
class AsciiParserState: public ParserState<IoDataTable>
{
public:
    // constructor
    explicit AsciiParserState(std::istream* stream, std::string* name,\
                              IoDataTable* data);
    // destructor
    virtual ~AsciiParserState(void);
    // public members
    std::stack<DMat>   dMatBuf;
    std::stack<double> doubleBuf;
private:
    // allocation/deallocation functions defined in IoAsciiLexer.lpp
    virtual void initScanner(void);
    virtual void destroyScanner(void);
};

class AsciiFile: public File
{
public:
    // constructors
    AsciiFile(void);
    AsciiFile(const std::string name, const unsigned int mode);
    // destructor
    virtual ~AsciiFile(void);
    // access
    template <typename IoObj>
    const IoObj& read(const std::string name);
    // tests
    virtual bool isOpen(void) const;
    // Io
    virtual void close(void);
    virtual void open(const std::string name, const unsigned int mode);
    virtual void save(void);
    virtual void saveAs(const std::string name);
private:
    void clear(void);
    void openAscii(const std::string name, const unsigned int mode);
    void closeAscii(void);
    void parse(void);
private:
    std::fstream      fileStream_;
    bool              isParsed_;
    AsciiParserState* state_;
};

// Template implementations
template <typename IoObj>
const IoObj& AsciiFile::read(const std::string dataName)
{
    if ((mode_ & FileMode::read)&&(isOpen()))
    {
        if (!isParsed_)
        {
            parse();
        }
        
        return getData<IoObj>(dataName);
    }
    else
    {
        if (isOpen())
        {
            LATAN_ERROR(Io,"file '" + name_ + "' is not opened in read mode");
        }
        else
        {
            LATAN_ERROR(Io,"file not opened");
        }
    }
}

LATAN_END_CPPDECL

#endif
