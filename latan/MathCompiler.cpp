#include <latan/MathCompiler.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

// Math Bison/Flex parser declaration
int _math_parse(MathCompiler::MathParserState* state);

/******************************************************************************
 *                        MathNode implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
MathNode::MathNode(const string &name, const unsigned int type)
: name_(name)
, type_(type)
, parent_(NULL)
{}

MathNode::MathNode(const std::string &name, const unsigned int type,\
                   const unsigned int nArg, ...)
: name_(name)
, type_(type)
, arg_(nArg)
, parent_(NULL)
{
    va_list va;

    va_start(va, nArg);
    for (unsigned int i = 0; i < nArg; ++i)
    {
        arg_[i] = va_arg(va, MathNode*);
        arg_[i]->parent_ = this;
    }
    va_end(va);
}

// destructor //////////////////////////////////////////////////////////////////
MathNode::~MathNode(void)
{
    vector<MathNode *>::iterator i;

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

// test ////////////////////////////////////////////////////////////////////////
bool MathNode::isRoot(void) const
{
    return (parent_ == NULL);
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
    if (!name_.empty())
    {
        vTable[name_] = dStack.top();
    }
    dStack.pop();
}

void Pop::print(std::ostream &out) const
{
    out << CODE_MOD << "pop" << CODE_MOD << name_;
}

Store::Store(const string &name)
: name_(name)
{}


void Store::operator()(std::stack<double> &dStack, VarTable &vTable)
{
    if (!name_.empty())
    {
        vTable[name_] = dStack.top();
    }
}

void Store::print(std::ostream &out) const
{
    out << CODE_MOD << "store" << CODE_MOD << name_;
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
DEF_OP(Add, 2, x[0] + x[1],    "add")
DEF_OP(Sub, 2, x[0] - x[1],    "sub")
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
// MathParserState constructor /////////////////////////////////////////////////
MathCompiler::MathParserState::MathParserState(istream *stream, string *name,
                                               vector<MathNode *> *data)
: ParserState<vector<MathNode *> >(stream, name, data)
{
    initScanner();
}

// MathParserState destructor //////////////////////////////////////////////////
MathCompiler::MathParserState::~MathParserState(void)
{
    destroyScanner();
}

// constructors ////////////////////////////////////////////////////////////////
MathCompiler::MathCompiler(void)
: code_(NULL)
, codeName_("<no_code>")
, state_(NULL)
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
    state_    = new MathParserState(code_, &codeName_, &expr_);
    status_   = Status::initialised;
}

const VirtualProgram& MathCompiler::operator()(void)
{
    if (!(status_ & Status::parsed))
    {
        parse();
        status_ |= Status::parsed;
        status_ -= status_ & Status::compiled;
    }
    if (!(status_ & Status::compiled))
    {
        compile(expr_);
        status_ |= Status::compiled;
    }
    
    return out_;
}

// private methods /////////////////////////////////////////////////////////////
void MathCompiler::parse(void)
{
    _math_parse(state_);
}

void MathCompiler::compile(const vector<MathNode *> &expr)
{
    string lastNode;
    
    for (unsigned int i = 0; i < expr.size(); ++i)
    {
        lastNode = expr[i]->getName();
        if ((lastNode == "=")||(lastNode == "return"))
        {
            compile(*expr[i]);
            if (lastNode == "return")
            {
                break;
            }
        }
    }
    
    if (lastNode != "return")
    {
        LATAN_ERROR(Syntax, "expected 'return' in program '" + codeName_ + "'");
    }
}

#define IFNODE(name, nArg) if ((n.getName() == (name))&&(n.getNArg() == nArg))
#define ELIFNODE(name, nArg) else IFNODE(name, nArg)
#define ELSE else
void MathCompiler::compile(const MathNode& n)
{
    switch (n.getType())
    {
        case MathNode::Type::cst:
            out_.push_back(new Push(strTo<double>(n.getName())));
            break;
        case MathNode::Type::var:
            out_.push_back(new Push(n.getName()));
            break;
        case MathNode::Type::op:
            if (n.getName() == "=")
            {
                compile(n[1]);
                if (n.isRoot())
                {
                    out_.push_back(new Pop(n[0].getName()));
                }
                else
                {
                    out_.push_back(new Store(n[0].getName()));
                }
            }
            else
            {
                for (unsigned int i = 0; i < n.getNArg(); ++i)
                {
                    compile(n[i]);
                }
                  IFNODE("-", 1) out_.push_back(new Neg);
                ELIFNODE("+", 2) out_.push_back(new Add);
                ELIFNODE("-", 2) out_.push_back(new Sub);
                ELIFNODE("*", 2) out_.push_back(new Mul);
                ELIFNODE("/", 2) out_.push_back(new Div);
                ELIFNODE("^", 2) out_.push_back(new Pow);
                ELSE LATAN_ERROR(Compilation, "unknown operator (node '"
                                 + n.getName() + "')");
            }
            break;
        case MathNode::Type::keyw: 
            for (unsigned int i = 0; i < n.getNArg(); ++i)
            {
                compile(n[i]);
            }
            break;
        default:
            LATAN_ERROR(Compilation,
                        "unknown node type (node '" + n.getName() + "')");
            break;
    }
}

void MathCompiler::reset(void)
{
    delete code_;
    codeName_ = "<no_code>";
    delete state_;
    for (vector<MathNode *>::iterator i = expr_.begin(); i != expr_.end(); ++i)
    {
        delete *i;
    }
    expr_.clear();
    for (VirtualProgram::iterator i = out_.begin(); i != out_.end(); ++i)
    {
        delete *i;
    }
    out_.clear();
    status_ = 0;
}

