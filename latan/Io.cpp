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
, mode_(FileMode::null)
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
    IoDataTable::iterator i;
    
    for (i=data_.begin();i!=data_.end();++i)
    {
        delete i->second;
    }
    data_.clear();
}

/******************************************************************************
 *                     AsciiParserState implementation                        *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
AsciiFile::AsciiParserState::AsciiParserState(istream* stream, string* name,
                                              IoDataTable* data)
: ParserState<IoDataTable>(stream, name, data)
{
    initScanner();
}

// destructor //////////////////////////////////////////////////////////////////
AsciiFile::AsciiParserState::~AsciiParserState(void)
{
    destroyScanner();
}

/******************************************************************************
 *                        AsciiFile implementation                            *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
AsciiFile::AsciiFile(void)
: File(), fileStream_()
, isParsed_(false)
, state_(NULL)
{}

AsciiFile::AsciiFile(const string &name, const unsigned int mode)
{
    openAscii(name, mode);
}

// destructor //////////////////////////////////////////////////////////////////
AsciiFile::~AsciiFile(void)
{
    closeAscii();
}

// tests ///////////////////////////////////////////////////////////////////////
bool AsciiFile::isOpen() const
{
    return fileStream_.is_open();
}

// IO //////////////////////////////////////////////////////////////////////////
void AsciiFile::close(void)
{
    closeAscii();
    deleteData();
}

void AsciiFile::open(const string &name, const unsigned int mode)
{
    if (isOpen())
    {
        LATAN_ERROR(Io, "file already opened with name '" + name_ + "'");
    }
    openAscii(name, mode);
}

void AsciiFile::save(void)
{
    LATAN_ERROR(Implementation, "saving Ascii files not implemented yet");
}

void AsciiFile::saveAs(const string &name __dumb)
{
    LATAN_ERROR(Implementation, "saving Ascii files not implemented yet");
}

void AsciiFile::openAscii(const string &name, const unsigned int mode)
{
    if (!isOpen())
    {
        name_     = name;
        mode_     = mode;
        isParsed_ = false;
        fileStream_.open(name_.c_str());
        if (mode_ & FileMode::read)
        {
            state_ = new AsciiParserState(&fileStream_, &name_, &data_);
        }
        else
        {
            state_ = NULL;
        }
    }
}

void AsciiFile::closeAscii(void)
{
    if (state_)
    {
        delete state_;
        state_ = NULL;
    }
    if (isOpen())
    {
        fileStream_.close();
    }
    name_     = "";
    mode_     = FileMode::null;
    isParsed_ = false;
}

void AsciiFile::load(const string &name __dumb)
{
    if ((mode_ & FileMode::read)&&(isOpen()))
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
int _ioAscii_parse(AsciiFile::AsciiParserState* state);

void AsciiFile::parse()
{
    _ioAscii_parse(state_);
    isParsed_ = true;
}
