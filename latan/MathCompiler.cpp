#include <latan/MathCompiler.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

// Math Bison/Flex parser declaration
int _Math_parse(MathParserState* state);

/******************************************************************************
 *                        MathNode implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
MathNode::MathNode(const string& init_str, NodeType::Type init_type)
: str(init_str), type(init_type)
{}

MathNode::MathNode(const std::string& init_str, NodeType::Type init_type,\
                   const unsigned int narg, ...)
: str(init_str), type(init_type), arg(narg)
{
    va_list va;

    va_start(va,narg);
    for (unsigned int i=0;i<narg;++i)
    {
        arg[i] = va_arg(va,MathNode*);
    }
    va_end(va);
}

// destructor //////////////////////////////////////////////////////////////////
MathNode::~MathNode(void)
{
    vector<MathNode*>::iterator i;

    for (i=arg.begin();i!=arg.end();++i)
    {
        delete *i;
    }
}

// access //////////////////////////////////////////////////////////////////////
const string& MathNode::String(void) const
{
    return str;
}

NodeType::Type MathNode::Type(void) const
{
    return type;
}

unsigned int MathNode::NArg(void) const
{
    return static_cast<unsigned int>(arg.size());
}

// operator ////////////////////////////////////////////////////////////////////
const MathNode& MathNode::operator[](const unsigned int i) const
{
    return *arg[i];
}

/******************************************************************************
 *                    MathParserState implementation                          *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
MathParserState::MathParserState(std::istream* pt_stream, std::string* pt_name,\
                                 MathNode** pt_data)
: ParserState<MathNode*>(pt_stream,pt_name,pt_data)
{
    init_scanner();
}

// destructor //////////////////////////////////////////////////////////////////
MathParserState::~MathParserState(void)
{
    destroy_scanner();
}

/******************************************************************************
 *                            Instruction set                                 *
 ******************************************************************************/
#define CODE_WIDTH 6
#define CODE_MOD   setw(CODE_WIDTH) << left

Instruction::~Instruction(void)
{}

void Instruction::Print(std::ostream &out) const
{
    out << CODE_MOD << "<null>" << CODE_MOD << "ABSTRACT INSTRUCTION!";
}

ostream& Latan::operator<<(ostream& out, const Instruction& ins)
{
    ins.Print(out);
    
    return out;
}

Push::Push(const double& init_cst)
: type(Constant), cst(init_cst), vname("")
{}

Push::Push(const string& init_vname)
: type(Variable), cst(0.0), vname(init_vname)
{}

void Push::operator()(std::stack<double> &dstack, VarTable &vtable)
{
    if (type == Constant)
    {
        dstack.push(cst);
    }
    else
    {
        dstack.push(vtable[vname]);
    }
}

void Push::Print(std::ostream &out) const
{
    out << CODE_MOD << "push";
    if (type == Constant)
    {
        out << CODE_MOD << cst;
    }
    else
    {
        out << CODE_MOD << vname;
    }
}

Pop::Pop(const string& init_vname)
: vname(init_vname)
{}

void Pop::operator()(std::stack<double> &dstack, VarTable &vtable)
{
    vtable[vname] = dstack.top();
    dstack.pop();
}

void Pop::Print(std::ostream &out) const
{
    out << CODE_MOD << "pop" << CODE_MOD << vname;
}

#define DEF_OP(name,narg,exp,code_name)\
void name::operator()(stack<double>& dstack, VarTable& vtable __dumb)\
{\
    double x[narg];\
    for (int i=0;i<narg;++i)\
    {\
        x[narg-1-i] = dstack.top();\
        dstack.pop();\
    }\
    dstack.push(exp);\
}\
\
void name::Print(std::ostream &out) const\
{\
    out << CODE_MOD << code_name;\
}

DEF_OP(Neg,1,-x[0],"neg")
DEF_OP(Add,2,x[0]+x[1],"add")
DEF_OP(Sub,2,x[0]-x[1],"sub")
DEF_OP(Mul,2,x[0]*x[1],"mul")
DEF_OP(Div,2,x[0]/x[1],"div")
DEF_OP(Pow,2,pow(x[0],x[1]),"pow")

VirtualProgram::VirtualProgram(void)
: vector<Instruction*>(0)
{}

ostream& Latan::operator<<(ostream& out,const VirtualProgram& prog)
{
    for (unsigned int i=0;i<prog.size();++i)
    {
        out << *(prog[i]) << endl;
    }
    
    return out;
}

/******************************************************************************
 *                       MathCompiler implementation                          *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
MathCompiler::MathCompiler(void)
: code(NULL), code_name("<no_code>"), state(NULL), root(NULL), out(), status(0)
{}

MathCompiler::MathCompiler(const std::string& code_str)
{
    Init(code_str);
}

// destructor //////////////////////////////////////////////////////////////////
MathCompiler::~MathCompiler(void)
{
    Reset();
}

// public methods //////////////////////////////////////////////////////////////
void MathCompiler::Init(const std::string &code_str)
{
    if (status)
    {
        Reset();
    }
    code      = new stringstream(code_str);
    code_name = "<string>";
    state     = new MathParserState(code,&code_name,&root);
    status   |= Initialised;
}

const VirtualProgram& MathCompiler::operator()(void)
{
    if (!(status & Parsed))
    {
        Parse();
    }
    if (!(status & Compiled))
    {
        Compile(*root);
    }
    
    return out;
}

// private methods /////////////////////////////////////////////////////////////
void MathCompiler::Parse(void)
{
    _Math_parse(state);
    status |= Parsed;
    status -= status & Compiled;
}

#define IFOP(name,narg) if ((N.String() == name)&&(N.NArg() == narg))
#define ELIFOP(name,narg) else IFOP(name,narg)
#define ELSE else
void MathCompiler::Compile(const MathNode& N)
{
    switch (N.Type())
    {
        case NodeType::Constant:
            out.push_back(new Push(ato<double>(N.String())));
            break;
        case NodeType::Variable:
            out.push_back(new Push(N.String()));
            break;
        case NodeType::Operator:
            for (unsigned int i=0;i<N.NArg();++i)
            {
                Compile(N[i]);
            }
            IFOP("-",1)   out.push_back(new Neg);
            ELIFOP("+",2) out.push_back(new Add);
            ELIFOP("-",2) out.push_back(new Sub);
            ELIFOP("*",2) out.push_back(new Mul);
            ELIFOP("/",2) out.push_back(new Div);
            ELIFOP("^",2) out.push_back(new Pow);
            ELSE LATAN_ERROR(Compilation,                              \
                             "unknown operator (node '"+N.String()+"')");
            break;
        default:
            LATAN_ERROR(Compilation,                               \
                        "unknown node type (node '"+N.String()+"')");
            break;
    }
    status |= Compiled;
}

void MathCompiler::Reset(void)
{
    VirtualProgram::iterator i;
    
    delete code;
    code_name = "<no_code>";
    delete state;
    delete root;
    for (i=out.begin();i!=out.end();++i)
    {
        delete *i;
    }
    out.clear();
    status = 0;
}

