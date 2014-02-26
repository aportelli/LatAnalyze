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

#include <latan/MathInterpreter.hpp>
#include <latan/includes.hpp>

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
, name_("")
{}

Push::Push(const string &name)
: type_(ArgType::Variable)
, val_(0.0)
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
            context.dStack.push(context.vTable.at(name_));
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
        out << CODE_MOD << name_;
    }
}

// Pop constructor /////////////////////////////////////////////////////////////
Pop::Pop(const string &name)
: name_(name)
{}

// Pop execution ///////////////////////////////////////////////////////////////
void Pop::operator()(RunContext &context) const
{
    if (!name_.empty())
    {
        context.vTable[name_] = context.dStack.top();
    }
    context.dStack.pop();
    context.insIndex++;
}

// Pop print ///////////////////////////////////////////////////////////////////
void Pop::print(ostream &out) const
{
    out << CODE_MOD << "pop" << CODE_MOD << name_;
}

// Store constructor ///////////////////////////////////////////////////////////
Store::Store(const string &name)
: name_(name)
{}

// Store execution /////////////////////////////////////////////////////////////
void Store::operator()(RunContext &context) const
{
    if (!name_.empty())
    {
        context.vTable[name_] = context.dStack.top();
    }
    context.insIndex++;
}

// Store print /////////////////////////////////////////////////////////////////
void Store::print(ostream &out) const
{
    out << CODE_MOD << "store" << CODE_MOD << name_;
}

// Call constructor ////////////////////////////////////////////////////////////
Call::Call(const string &name)
: name_(name)
{}

// Call execution //////////////////////////////////////////////////////////////
void Call::operator()(RunContext &context) const
{
    try
    {
        context.dStack.push((*context.fTable.at(name_))(context.dStack));
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
    out << CODE_MOD << "call" << CODE_MOD << name_;
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

#define PUSH_INS(program, type, ...) \
program.push_back(unique_ptr<type>(new type(__VA_ARGS__)))

// VarNode compile /////////////////////////////////////////////////////////////
void VarNode::compile(Program &program) const
{
    PUSH_INS(program, Push, getName());
}

// CstNode compile /////////////////////////////////////////////////////////////
void CstNode::compile(Program &program) const
{
    PUSH_INS(program, Push, strTo<double>(getName()));
}

// SemicolonNode compile ///////////////////////////////////////////////////////
void SemicolonNode::compile(Program &program) const
{
    auto &n = *this;
    
    for (Index i = 0; i < getNArg(); ++i)
    {
        bool isAssign     = isDerivedFrom<AssignNode>(&n[i]);
        bool isSemiColumn = isDerivedFrom<SemicolonNode>(&n[i]);
        bool isKeyword    = isDerivedFrom<KeywordNode>(&n[i]);
        
        if (isAssign||isSemiColumn||isKeyword)
        {
            n[i].compile(program);
        }
    }
}

// AssignNode compile //////////////////////////////////////////////////////////
void AssignNode::compile(Program &program) const
{
    auto &n = *this;
    
    if (isDerivedFrom<VarNode>(&n[0]))
    {
        bool hasSemicolonParent = isDerivedFrom<SemicolonNode>(getParent());
        
        n[1].compile(program);
        if (hasSemicolonParent)
        {
            PUSH_INS(program, Pop, n[0].getName());
        }
        else
        {
            PUSH_INS(program, Store, n[0].getName());
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

void MathOpNode::compile(Program &program) const
{
    auto &n = *this;
    
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        n[i].compile(program);
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
void FuncNode::compile(Program &program) const
{
    auto &n = *this;
    
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        n[i].compile(program);
    }
    PUSH_INS(program, Call, getName());
}

// ReturnNode compile ////////////////////////////////////////////////////////////
void ReturnNode::compile(Program &program) const
{
    auto &n = *this;
    
    n[0].compile(program);
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
MathInterpreter::MathInterpreter(void)
: code_(nullptr)
, codeName_("<no_code>")
, state_(nullptr)
, root_(nullptr)
, status_(Status::none)
{}

MathInterpreter::MathInterpreter(const std::string &code)
: code_(nullptr)
, codeName_("<string>")
, state_(nullptr)
, root_(nullptr)
, status_(Status::none)
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
void MathInterpreter::compile(void)
{
    bool gotReturn = false;
    
    if (!(status_ & Status::parsed))
    {
        parse();
        status_ |= Status::parsed;
        status_ -= status_ & Status::compiled;
    }
    if (root_)
    {
        root_->compile(program_);
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
        
    }
    if (!root_||!gotReturn)
    {
        LATAN_ERROR(Syntax, "expected 'return' in program '" + codeName_ + "'");
    }
    status_ |= Status::compiled;
}

// execution ///////////////////////////////////////////////////////////////////
void MathInterpreter::operator()(RunContext &context)
{
    if (!(status_ & Status::compiled))
    {
        compile();
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
