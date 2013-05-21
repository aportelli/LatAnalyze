#include <latan/IO.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

// ASCII data format Bison/Flex parser declaration
int _IOASCII_parse(ASCIIParserState* state);

/******************************************************************************
 *                          File implementation                               *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
File::File(void)
: name(""), mode(FileMode::Null), data()
{}

File::File(const string init_name, const FileMode::Type init_mode)
: name(init_name), mode(init_mode), data()
{}

// destructor //////////////////////////////////////////////////////////////////
File::~File(void)
{}

// access //////////////////////////////////////////////////////////////////////
string File::Name(void) const
{
    return name;
}

FileMode::Type File::Mode(void) const
{
    return mode;
}

// Internal functions //////////////////////////////////////////////////////////
void File::DeleteData(void)
{
    IODataTable::iterator i;
    
    for (i=data.begin();i!=data.end();++i)
    {
        delete i->second;
    }
    data.clear();
}

/******************************************************************************
 *                     ASCIIParserState implementation                        *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
ASCIIParserState::ASCIIParserState(istream* pt_istream, string* name,\
                                   IODataTable* pt_data)
: data(pt_data), stream(pt_istream), stream_name(name)
{
    init_scanner();
}

// destructor //////////////////////////////////////////////////////////////////
ASCIIParserState::~ASCIIParserState(void)
{
    destroy_scanner();
}

/******************************************************************************
 *                        ASCIIFile implementation                            *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
ASCIIFile::ASCIIFile(void)
: File(), file_stream(), is_parsed(false), state(NULL)
{}

ASCIIFile::ASCIIFile(const string in_name, const FileMode::Type in_mode)
{
    OpenASCII(in_name,in_mode);
}

// destructor //////////////////////////////////////////////////////////////////
ASCIIFile::~ASCIIFile(void)
{
    Clear();
}

// tests ///////////////////////////////////////////////////////////////////////
bool ASCIIFile::IsOpen() const
{
    return file_stream.is_open();
}

// IO //////////////////////////////////////////////////////////////////////////
void ASCIIFile::Close(void)
{
    Clear();
}

void ASCIIFile::Open(const string new_name, const FileMode::Type new_mode)
{
    if (IsOpen())
    {
        LATAN_ERROR(IO,"file already opened with name '"+name+"'");
    }
    OpenASCII(new_name,new_mode);
}

void ASCIIFile::Save(void)
{
    LATAN_ERROR(Implementation,"saving ASCII files not implemented yet");
}

void ASCIIFile::SaveAs(const string new_name __dumb)
{
    LATAN_ERROR(Implementation,"saving ASCII files not implemented yet");
}

// Internal functions //////////////////////////////////////////////////////////
void ASCIIFile::Clear()
{
    DeleteData();
    CloseASCII();
}

void ASCIIFile::OpenASCII(const string new_name, const FileMode::Type new_mode)
{
    if (!IsOpen())
    {
        name      = new_name;
        mode      = new_mode;
        is_parsed = false;
        file_stream.open(name.c_str());
        if (mode & FileMode::Read)
        {
            state = new ASCIIParserState(&file_stream,&name,&data);
        }
        else
        {
            state = NULL;
        }
    }
}

void ASCIIFile::CloseASCII(void)
{
    if (state)
    {
        delete state;
        state = NULL;
    }
    if (IsOpen())
    {
        file_stream.close();
    }
}

void ASCIIFile::Parse()
{
    _IOASCII_parse(state);
}
