#include <latan/MathCompiler.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

// Math Bison/Flex parser declaration
int _math_parse(MathParserState* state);

/******************************************************************************
 *                        MathNode implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
MathNode::MathNode(const string &name, const unsigned int type)
: name_(name)
, type_(type)
{}

MathNode::MathNode(const std::string &name, const unsigned int type,\
                   const unsigned int nArg, ...)
: name_(name)
, type_(type)
, arg_(nArg)
{
    va_list va;

    va_start(va, nArg);
    for (unsigned int i = 0; i < nArg; ++i)
    {
        arg_[i] = va_arg(va, MathNode*);
    }
    va_end(va);
}

// destructor //////////////////////////////////////////////////////////////////
MathNode::~MathNode(void)
{
    vector<MathNode*>::iterator i;

    for (i = arg_.begin(); i != arg_.end(); ++i)
    {
        delete *i;
    }
}

// access //////////////////////////////////////////////////////////////////////
const string &MathNode::getName(void) const
{
    return name_;
}

unsigned int MathNode::getType(void) const
{
    return type_;
}

unsigned int MathNode::getNArg(void) const
{
    return static_cast<unsigned int>(arg_.size());
}

// operator ////////////////////////////////////////////////////////////////////
const MathNode &MathNode::operator[](const unsigned int i) const
{
    return *arg_[i];
}

/******************************************************************************
 *                    MathParserState implementation                          *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
MathParserState::MathParserState(std::istream *stream, std::string *name,
                                 MathNode **data)
: ParserState<MathNode *>(stream, name, data)
{
    initScanner();
}

// destructor //////////////////////////////////////////////////////////////////
MathParserState::~MathParserState(void)
{
    destroyScanner();
}

/******************************************************************************
 *                            Instruction set                                 *
 ******************************************************************************/
#define CODE_WIDTH 6
#define CODE_MOD   setw(CODE_WIDTH) << left

Instruction::~Instruction(void)
{}

ostream &Latan::operator<<(ostream& out, const Instruction& ins)
{
    ins.print(out);
    
    return out;
}

Push::Push(const double val)
: type_(ArgType::Constant)
, val_(val)
, name_("")
{}

Push::Push(const string &name)
: type_(ArgType::Variable)
, val_(0.0)
, name_(name)
{}

void Push::operator()(std::stack<double> &dStack, VarTable &vTable)
{
    if (type_ == ArgType::Constant)
    {
        dStack.push(val_);
    }
    else
    {
        if (keyExists(name_, vTable))
        {
            dStack.push(vTable[name_]);
        }
        else
        {
            LATAN_ERROR(Range, "unknown variable '" + name_ + "'");
        }
    }
}

void Push::print(std::ostream &out) const
{
    out << CODE_MOD << "push";
    if (type_ == ArgType::Constant)
    {
        out << CODE_MOD << val_;
    }
    else
    {
        out << CODE_MOD << name_;
    }
}

Pop::Pop(const string &name)
: name_(name)
{}

void Pop::operator()(std::stack<double> &dStack, VarTable &vTable)
{
    vTable[name_] = dStack.top();
    dStack.pop();
}

void Pop::print(std::ostream &out) const
{
    out << CODE_MOD << "pop" << CODE_MOD << name_;
}

#define DEF_OP(name, nArg, exp, insName)\
void name::operator()(stack<double> &dStack, VarTable &vTable __dumb)\
{\
    double x[nArg];\
    for (int i = 0; i < nArg; ++i)\
    {\
        x[nArg-1-i] = dStack.top();\
        dStack.pop();\
    }\
    dStack.push(exp);\
}\
void name::print(std::ostream &out) const\
{\
    out << CODE_MOD << insName;\
}

DEF_OP(Neg, 1, -x[0],          "neg")
DEF_OP(Add, 2, x[0]+x[1],      "add")
DEF_OP(Sub, 2, x[0]-x[1],      "sub")
DEF_OP(Mul, 2, x[0]*x[1],      "mul")
DEF_OP(Div, 2, x[0]/x[1],      "div")
DEF_OP(Pow, 2, pow(x[0],x[1]), "pow")

VirtualProgram::VirtualProgram(void)
: vector<Instruction *>(0)
{}

ostream &Latan::operator<<(ostream &out, const VirtualProgram &prog)
{
    for (unsigned int i = 0; i < prog.size(); ++i)
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
: code_(NULL)
, codeName_("<no_code>")
, state_(NULL)
, root_(NULL)
, out_()
, status_(Status::none)
{}

MathCompiler::MathCompiler(const std::string &code)
{
    init(code);
}

// destructor //////////////////////////////////////////////////////////////////
MathCompiler::~MathCompiler(void)
{
    reset();
}

// public methods //////////////////////////////////////////////////////////////
void MathCompiler::init(const std::string &code)
{
    if (status_)
    {
        reset();
    }
    code_     = new stringstream(code);
    codeName_ = "<string>";
    state_    = new MathParserState(code_, &codeName_, &root_);
    status_  |= Status::initialised;
}

const VirtualProgram& MathCompiler::operator()(void)
{
    if (!(status_ & Status::parsed))
    {
        parse();
    }
    if (!(status_ & Status::compiled))
    {
        compile(*root_);
    }
    
    return out_;
}

// private methods /////////////////////////////////////////////////////////////
void MathCompiler::parse(void)
{
    _math_parse(state_);
    status_ |= Status::parsed;
    status_ -= status_ & Status::compiled;
}

#define IFOP(name, nArg) if ((n.getName() == (name))&&(n.getNArg() == nArg))
#define ELIFOP(name, nArg) else IFOP(name, nArg)
#define ELSE else
void MathCompiler::compile(const MathNode& n)
{
    switch (n.getType())
    {
        case MathNode::Type::Constant:
            out_.push_back(new Push(strTo<double>(n.getName())));
            break;
        case MathNode::Type::Variable:
            out_.push_back(new Push(n.getName()));
            break;
        case MathNode::Type::Operator:
            for (unsigned int i = 0; i < n.getNArg(); ++i)
            {
                compile(n[i]);
            }
            IFOP("-",1)   out_.push_back(new Neg);
            ELIFOP("+",2) out_.push_back(new Add);
            ELIFOP("-",2) out_.push_back(new Sub);
            ELIFOP("*",2) out_.push_back(new Mul);
            ELIFOP("/",2) out_.push_back(new Div);
            ELIFOP("^",2) out_.push_back(new Pow);
            ELSE LATAN_ERROR(Compilation,
                             "unknown operator (node '" + n.getName() + "')");
            break;
        default:
            LATAN_ERROR(Compilation,
                        "unknown node type (node '" + n.getName() + "')");
            break;
    }
    status_ |= Status::compiled;
}

void MathCompiler::reset(void)
{
    VirtualProgram::iterator i;
    
    delete code_;
    codeName_ = "<no_code>";
    delete state_;
    delete root_;
    for (i = out_.begin(); i != out_.end(); ++i)
    {
        delete *i;
    }
    out_.clear();
    status_ = 0;
}

