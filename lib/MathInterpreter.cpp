/*
 * MathInterpreter.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2014 Antonin Portelli
 *
 * LatAnalyze 3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze 3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze 3.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LatAnalyze/MathInterpreter.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Math.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                            Instruction set                                 *
 ******************************************************************************/
#define CODE_WIDTH 6
#define CODE_MOD   setw(CODE_WIDTH) << left

// Instruction operator ////////////////////////////////////////////////////////
ostream &Latan::operator<<(ostream& out, const Instruction& ins)
{
    ins.print(out);
    
    return out;
}

// Push constructors ///////////////////////////////////////////////////////////
Push::Push(const double val)
: type_(ArgType::Constant)
, val_(val)
, address_(0)
, name_("")
{}

Push::Push(const unsigned int address, const string &name)
: type_(ArgType::Variable)
, val_(0.0)
, address_(address)
, name_(name)
{}

// Push execution //////////////////////////////////////////////////////////////
void Push::operator()(RunContext &context) const
{
    if (type_ == ArgType::Constant)
    {
        context.dStack.push(val_);
    }
    else
    {
        try
        {
            context.dStack.push(context.vMem[address_]);
        }
        catch (out_of_range)
        {
            LATAN_ERROR(Range, "unknown variable '" + name_ + "'");
        }
    }
    context.insIndex++;
}

// Push print //////////////////////////////////////////////////////////////////
void Push::print(ostream &out) const
{
    out << CODE_MOD << "push";
    if (type_ == ArgType::Constant)
    {
        out << CODE_MOD << val_;
    }
    else
    {
        out << CODE_MOD << name_ << " @v" << address_;
    }
}

// Pop constructor /////////////////////////////////////////////////////////////
Pop::Pop(const unsigned int address, const string &name)
: address_(address)
, name_(name)
{}

// Pop execution ///////////////////////////////////////////////////////////////
void Pop::operator()(RunContext &context) const
{
    if (!name_.empty())
    {
        context.vMem[address_] = context.dStack.top();
    }
    context.dStack.pop();
    context.insIndex++;
}

// Pop print ///////////////////////////////////////////////////////////////////
void Pop::print(ostream &out) const
{
    out << CODE_MOD << "pop" << CODE_MOD << name_ << " @v" << address_;
}

// Store constructor ///////////////////////////////////////////////////////////
Store::Store(const unsigned int address, const string &name)
: address_(address)
, name_(name)
{}

// Store execution /////////////////////////////////////////////////////////////
void Store::operator()(RunContext &context) const
{
    if (!name_.empty())
    {
        context.vMem[address_] = context.dStack.top();
    }
    context.insIndex++;
}

// Store print /////////////////////////////////////////////////////////////////
void Store::print(ostream &out) const
{
    out << CODE_MOD << "store" << CODE_MOD << name_ << " @v" << address_;
}

// Call constructor ////////////////////////////////////////////////////////////
Call::Call(const unsigned int address, const string &name)
: address_(address)
, name_(name)
{}

// Call execution //////////////////////////////////////////////////////////////
void Call::operator()(RunContext &context) const
{
    try
    {
        context.dStack.push((*context.fMem[address_])(context.dStack));
    }
    catch (out_of_range)
    {
        LATAN_ERROR(Range, "unknown function '" + name_ + "'");
    }
    context.insIndex++;
}

// Call print //////////////////////////////////////////////////////////////////
void Call::print(ostream &out) const
{
    out << CODE_MOD << "call" << CODE_MOD << name_ << " @f" << address_;
}

// Math operations /////////////////////////////////////////////////////////////
#define DEF_OP(name, nArg, exp, insName)\
void name::operator()(RunContext &context) const\
{\
    double x[nArg];\
    for (int i = 0; i < nArg; ++i)\
    {\
        x[nArg-1-i] = context.dStack.top();\
        context.dStack.pop();\
    }\
    context.dStack.push(exp);\
    context.insIndex++;\
}\
void name::print(ostream &out) const\
{\
    out << CODE_MOD << insName;\
}

DEF_OP(Neg, 1, -x[0],          "neg")
DEF_OP(Add, 2, x[0] + x[1],    "add")
DEF_OP(Sub, 2, x[0] - x[1],    "sub")
DEF_OP(Mul, 2, x[0]*x[1],      "mul")
DEF_OP(Div, 2, x[0]/x[1],      "div")
DEF_OP(Pow, 2, pow(x[0],x[1]), "pow")

/******************************************************************************
 *                        ExprNode implementation                             *
 ******************************************************************************/
// ExprNode constructors ///////////////////////////////////////////////////////
ExprNode::ExprNode(const string &name)
: name_(name)
, parent_(nullptr)
{}

// ExprNode access /////////////////////////////////////////////////////////////
const string &ExprNode::getName(void) const
{
    return name_;
}

Index ExprNode::getNArg(void) const
{
    return static_cast<Index>(arg_.size());
}

const ExprNode * ExprNode::getParent(void) const
{
    return parent_;
}

Index ExprNode::getLevel(void) const
{
    if (getParent())
    {
        return getParent()->getLevel() + 1;
    }
    else
    {
        return 0;
    }
}

void ExprNode::setName(const std::string &name)
{
    name_ = name;
}

void ExprNode::pushArg(ExprNode *node)
{
    if (node)
    {
        node->parent_ = this;
        arg_.push_back(unique_ptr<ExprNode>(node));
    }
}

// ExprNode operators //////////////////////////////////////////////////////////
const ExprNode &ExprNode::operator[](const Index i) const
{
    return *arg_[static_cast<unsigned int>(i)];
}

ostream &Latan::operator<<(ostream &out, const ExprNode &n)
{
    Index level = n.getLevel();
    
    for (Index i = 0; i <= level; ++i)
    {
        if (i == level)
        {
            out << "_";
        }
        else if (i == level - 1)
        {
            out << "|";
        }
        else
        {
            out << "  ";
        }
    }
    out << " " << n.getName() << endl;
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        out << n[i];
    }
    
    return out;
}

#define PUSH_INS(program, type, ...)\
program.push_back(unique_ptr<type>(new type(__VA_ARGS__)))
#define GET_ADDRESS(address, table, name)\
try\
{\
    address = (table).at(name);\
}\
catch (out_of_range)\
{\
    address         = static_cast<unsigned int>((table).size());\
    (table)[(name)] = address;\
}\

// VarNode compile /////////////////////////////////////////////////////////////
void VarNode::compile(Program &program, AddressTable &vTable,
                      AddressTable &fTable __unused) const
{
    unsigned int address;
    
    GET_ADDRESS(address, vTable, getName());
    PUSH_INS(program, Push, address, getName());
    
}

// CstNode compile /////////////////////////////////////////////////////////////
void CstNode::compile(Program &program, AddressTable &nextVAddress __unused,
                      AddressTable &nextFAddress __unused) const
{
    PUSH_INS(program, Push, strTo<double>(getName()));
}

// SemicolonNode compile ///////////////////////////////////////////////////////
void SemicolonNode::compile(Program &program, AddressTable &vTable,
                            AddressTable &fTable) const
{
    auto &n = *this;
    
    for (Index i = 0; i < getNArg(); ++i)
    {
        bool isAssign     = isDerivedFrom<AssignNode>(&n[i]);
        bool isSemiColumn = isDerivedFrom<SemicolonNode>(&n[i]);
        bool isKeyword    = isDerivedFrom<KeywordNode>(&n[i]);
        
        if (isAssign||isSemiColumn||isKeyword)
        {
            n[i].compile(program, vTable, fTable);
        }
    }
}

// AssignNode compile //////////////////////////////////////////////////////////
void AssignNode::compile(Program &program, AddressTable &vTable,
                         AddressTable &fTable) const
{
    auto &n = *this;
    
    if (isDerivedFrom<VarNode>(&n[0]))
    {
        bool hasSemicolonParent = isDerivedFrom<SemicolonNode>(getParent());
        unsigned int address;
        
        n[1].compile(program, vTable, fTable);
        GET_ADDRESS(address, vTable, n[0].getName());
        if (hasSemicolonParent)
        {
            PUSH_INS(program, Pop, address, n[0].getName());
        }
        else
        {
            PUSH_INS(program, Store, address, n[0].getName());
        }
    }
    else
    {
        LATAN_ERROR(Compilation, "invalid LHS for '='");
    }
}

// MathOpNode compile //////////////////////////////////////////////////////////
#define IFNODE(name, nArg) if ((n.getName() == (name))&&(n.getNArg() == nArg))
#define ELIFNODE(name, nArg) else IFNODE(name, nArg)
#define ELSE else

void MathOpNode::compile(Program &program, AddressTable &vTable,
                         AddressTable &fTable) const
{
    auto &n = *this;
    
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        n[i].compile(program, vTable, fTable);
    }
    IFNODE("-", 1)   PUSH_INS(program, Neg,);
    ELIFNODE("+", 2) PUSH_INS(program, Add,);
    ELIFNODE("-", 2) PUSH_INS(program, Sub,);
    ELIFNODE("*", 2) PUSH_INS(program, Mul,);
    ELIFNODE("/", 2) PUSH_INS(program, Div,);
    ELIFNODE("^", 2) PUSH_INS(program, Pow,);
    ELSE LATAN_ERROR(Compilation, "unknown operator '" + getName() + "'");
}

// FuncNode compile ////////////////////////////////////////////////////////////
void FuncNode::compile(Program &program, AddressTable &vTable,
                       AddressTable &fTable) const
{
    auto &n = *this;
    unsigned int address;
    
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        n[i].compile(program, vTable, fTable);
    }
    GET_ADDRESS(address, fTable, getName());
    PUSH_INS(program, Call, address, getName());
}

// ReturnNode compile ////////////////////////////////////////////////////////////
void ReturnNode::compile(Program &program, AddressTable &vTable,
                         AddressTable &fTable) const
{
    auto &n = *this;
    
    n[0].compile(program, vTable, fTable);
    program.push_back(nullptr);
}

/******************************************************************************
 *                       MathInterpreter implementation                       *
 ******************************************************************************/
// MathParserState constructor /////////////////////////////////////////////////
MathInterpreter::MathParserState::MathParserState
(istream *stream, string *name, std::unique_ptr<ExprNode> *data)
: ParserState<std::unique_ptr<ExprNode>>(stream, name, data)
{
    initScanner();
}

// MathParserState destructor //////////////////////////////////////////////////
MathInterpreter::MathParserState::~MathParserState(void)
{
    destroyScanner();
}

// constructors ////////////////////////////////////////////////////////////////
MathInterpreter::MathInterpreter(const std::string &code)
: codeName_("<string>")
{
    setCode(code);
}

// access //////////////////////////////////////////////////////////////////////
const Instruction * MathInterpreter::operator[](const Index i) const
{
    return program_[static_cast<unsigned int>(i)].get();
}

const ExprNode * MathInterpreter::getAST(void) const
{
    return root_.get();
}

void MathInterpreter::push(const Instruction *i)
{
    program_.push_back(unique_ptr<const Instruction>(i));
}

// initialization //////////////////////////////////////////////////////////////
void MathInterpreter::setCode(const std::string &code)
{
    if (status_)
    {
        reset();
    }
    code_.reset(new stringstream(code));
    codeName_ = "<string>";
    state_.reset(new MathParserState(code_.get(), &codeName_, &root_));
    program_.clear();
    status_   = Status::initialised;
}

void MathInterpreter::reset(void)
{
    code_.reset();
    codeName_ = "<no_code>";
    state_.reset();
    root_.reset();
    program_.clear();
    status_ = 0;
}

// parser //////////////////////////////////////////////////////////////////////

// Bison/Flex parser declaration
int _math_parse(MathInterpreter::MathParserState *state);

void MathInterpreter::parse(void)
{
    _math_parse(state_.get());
}

// interpreter /////////////////////////////////////////////////////////////////
#define ADD_FUNC(context, func)\
try\
{\
(context).fMem[(context).fTable.at(#func)] = &STDMATH_NAMESPACE::func;\
}\
catch (out_of_range)\
{}

#define ADD_STDMATH_FUNCS(context)\
ADD_FUNC(context, cos);\
ADD_FUNC(context, sin);\
ADD_FUNC(context, tan);\
ADD_FUNC(context, acos);\
ADD_FUNC(context, asin);\
ADD_FUNC(context, atan);\
ADD_FUNC(context, atan2);\
ADD_FUNC(context, cosh);\
ADD_FUNC(context, sinh);\
ADD_FUNC(context, tanh);\
ADD_FUNC(context, acosh);\
ADD_FUNC(context, asinh);\
ADD_FUNC(context, atanh);\
ADD_FUNC(context, exp);\
ADD_FUNC(context, log);\
ADD_FUNC(context, log10);\
ADD_FUNC(context, exp2);\
ADD_FUNC(context, expm1);\
ADD_FUNC(context, log1p);\
ADD_FUNC(context, log2);\
ADD_FUNC(context, pow);\
ADD_FUNC(context, sqrt);\
ADD_FUNC(context, cbrt);\
ADD_FUNC(context, hypot);\
ADD_FUNC(context, erf);\
ADD_FUNC(context, erfc);\
ADD_FUNC(context, tgamma);\
ADD_FUNC(context, lgamma);\
ADD_FUNC(context, ceil);\
ADD_FUNC(context, floor);\
ADD_FUNC(context, fmod);\
ADD_FUNC(context, trunc);\
ADD_FUNC(context, round);\
ADD_FUNC(context, rint);\
ADD_FUNC(context, nearbyint);\
ADD_FUNC(context, remainder);\
ADD_FUNC(context, fdim);\
ADD_FUNC(context, fmax);\
ADD_FUNC(context, fmin);\
ADD_FUNC(context, fabs);

void MathInterpreter::compile(RunContext &context)
{
    bool gotReturn = false;
    
    if (!(status_ & Status::parsed))
    {
        parse();
        status_ |= Status::parsed;
        status_ -= status_ & Status::compiled;
    }
    if (!(status_ & Status::compiled))
    {
        if (root_)
        {
            context.vTable.clear();
            context.fTable.clear();
            root_->compile(program_, context.vTable, context.fTable);
            for (unsigned int i = 0; i < program_.size(); ++i)
            {
                if (!program_[i])
                {
                    gotReturn = true;
                    program_.resize(i);
                    program_.shrink_to_fit();
                    break;
                }
            }
            context.vMem.resize(context.vTable.size());
            context.fMem.resize(context.fTable.size());
            ADD_STDMATH_FUNCS(context);
        }
        if (!root_||!gotReturn)
        {
            LATAN_ERROR(Syntax, "expected 'return' in program '" + codeName_
                        + "'");
        }
        status_ |= Status::compiled;
    }
}


// execution ///////////////////////////////////////////////////////////////////
void MathInterpreter::operator()(RunContext &context)
{
    if (!(status_ & Status::compiled))
    {
        compile(context);
    }
    execute(context);
}

void MathInterpreter::execute(RunContext &context) const
{
    context.insIndex = 0;
    while (context.insIndex != program_.size())
    {
        (*(program_[context.insIndex]))(context);
    }
}

// IO //////////////////////////////////////////////////////////////////////////
ostream &Latan::operator<<(ostream &out, const MathInterpreter &program)
{
    for (unsigned int i = 0; i < program.program_.size(); ++i)
    {
        out << *(program.program_[i]) << endl;
    }
    
    return out;
}
