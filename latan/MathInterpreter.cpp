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
 *                        MathNode implementation                             *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
MathNode::MathNode(const string &name, const Type type)
: name_(name)
, type_(type)
, parent_(nullptr)
{}

MathNode::MathNode(const std::string &name, const Type type,\
                   const unsigned int nArg, ...)
: name_(name)
, type_(type)
, arg_(nArg)
, parent_(nullptr)
{
    va_list va;

    va_start(va, nArg);
    for (unsigned int i = 0; i < nArg; ++i)
    {
        arg_[i].reset(va_arg(va, MathNode *));
        arg_[i]->parent_ = this;
    }
    va_end(va);
}

// destructor //////////////////////////////////////////////////////////////////
MathNode::~MathNode(void)
{}

// access //////////////////////////////////////////////////////////////////////
const string &MathNode::getName(void) const
{
    return name_;
}

MathNode::Type MathNode::getType(void) const
{
    return type_;
}

unsigned int MathNode::getNArg(void) const
{
    return static_cast<unsigned int>(arg_.size());
}

const MathNode * MathNode::getParent(void) const
{
    return parent_;
}

unsigned int MathNode::getLevel(void) const
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

void MathNode::setName(const std::string &name)
{
    name_ = name;
}

void MathNode::pushArg(MathNode *node)
{
    arg_.push_back(unique_ptr<MathNode>(node));
}

// operators ///////////////////////////////////////////////////////////////////
const MathNode &MathNode::operator[](const unsigned int i) const
{
    return *arg_[i];
}

ostream &Latan::operator<<(ostream &out, const MathNode &n)
{
    unsigned int level = n.getLevel();
    
    for (unsigned int i = 0; i <= level; ++i)
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
    out << " " << n.getName() << " (type " << static_cast<int>(n.getType())
        << ")" << endl;
    for (unsigned int i = 0; i < n.getNArg(); ++i)
    {
        out << n[i];
    }
    
    return out;
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

Pop::Pop(const string &name)
: name_(name)
{}

void Pop::operator()(RunContext &context) const
{
    if (!name_.empty())
    {
        context.vTable[name_] = context.dStack.top();
    }
    context.dStack.pop();
    context.insIndex++;
}

void Pop::print(ostream &out) const
{
    out << CODE_MOD << "pop" << CODE_MOD << name_;
}

Store::Store(const string &name)
: name_(name)
{}

void Store::operator()(RunContext &context) const
{
    if (!name_.empty())
    {
        context.vTable[name_] = context.dStack.top();
    }
    context.insIndex++;
}

void Store::print(ostream &out) const
{
    out << CODE_MOD << "store" << CODE_MOD << name_;
}

Call::Call(const string &name)
: name_(name)
{}

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

void Call::print(ostream &out) const
{
    out << CODE_MOD << "call" << CODE_MOD << name_;
}

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
 *                       MathInterpreter implementation                       *
 ******************************************************************************/
// MathParserState constructor /////////////////////////////////////////////////
MathInterpreter::MathParserState::MathParserState
(istream *stream, string *name, std::unique_ptr<MathNode> *data)
: ParserState<std::unique_ptr<MathNode>>(stream, name, data)
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
, gotReturn_(false)
, status_(Status::none)
{}

MathInterpreter::MathInterpreter(const std::string &code)
: code_(nullptr)
, codeName_("<string>")
, state_(nullptr)
, root_(nullptr)
, gotReturn_(false)
, status_(Status::none)
{
    setCode(code);
}

// destructor //////////////////////////////////////////////////////////////////
MathInterpreter::~MathInterpreter(void)
{}

// access //////////////////////////////////////////////////////////////////////
const Instruction * MathInterpreter::operator[](const unsigned int i) const
{
    return program_[i].get();
}

const MathNode * MathInterpreter::getAST(void) const
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
    if (!(status_ & Status::parsed))
    {
        parse();
        status_ |= Status::parsed;
        status_ -= status_ & Status::compiled;
    }
    gotReturn_ = false;
    if (root_)
    {
        compileNode(*root_);
    }
    if (!root_||!gotReturn_)
    {
        LATAN_ERROR(Syntax, "expected 'return' in program '" + codeName_ + "'");
    }
    status_ |= Status::compiled;
}

#define IFNODE(name, nArg) if ((n.getName() == (name))&&(n.getNArg() == nArg))
#define ELIFNODE(name, nArg) else IFNODE(name, nArg)
#define ELSE else

#define PUSH_INS(type, ...) \
program_.push_back(unique_ptr<type>(new (type)(__VA_ARGS__)))

void MathInterpreter::compileNode(const MathNode& n)
{
    if (!gotReturn_)
    {
        switch (n.getType())
        {
            case MathNode::Type::cst:
                PUSH_INS(Push, strTo<double>(n.getName()));
                break;
            case MathNode::Type::var:
                PUSH_INS(Push, n.getName());
                break;
            case MathNode::Type::op:
                // semicolon
                if (n.getName() == ";")
                {
                    // compile relevant statements
                    for (unsigned int i = 0; i < n.getNArg(); ++i)
                    {
                        bool isAssign =
                        ((n[i].getType() == MathNode::Type::op)&&
                         (n[i].getName() == "="));
                        bool isSemiColumn =
                        ((n[i].getType() == MathNode::Type::op)&&
                         (n[i].getName() == ";"));
                        bool isKeyword =
                        (n[i].getType() == MathNode::Type::keyw);
                        
                        if (isAssign||isSemiColumn||isKeyword)
                        {
                            compileNode(n[i]);
                        }
                    }
                }
                // assignment
                else if (n.getName() == "=")
                {
                    // variable assignement
                    if (n[0].getType() == MathNode::Type::var)
                    {
                        bool hasSemicolonParent = ((n.getParent() != NULL) &&
                                                   (n.getParent()->getType()
                                                    == MathNode::Type::op)&&
                                                   (n.getParent()->getName()
                                                    == ";"));
                        // compile the RHS
                        compileNode(n[1]);
                        // pop instruction if at the end of a statement
                        if (hasSemicolonParent)
                        {
                            program_.push_back(unique_ptr<Pop>(new Pop(n[0].getName())));
                        }
                        // store instruction else
                        else
                        {
                            PUSH_INS(Store, n[0].getName());
                        }
                    }
                    else
                    {
                        LATAN_ERROR(Compilation, "invalid LHS for '='");
                    }
                }
                // arithmetic operators
                else
                {
                    for (unsigned int i = 0; i < n.getNArg(); ++i)
                    {
                        compileNode(n[i]);
                    }
                    IFNODE("-", 1)   PUSH_INS(Neg,);
                    ELIFNODE("+", 2) PUSH_INS(Add,);
                    ELIFNODE("-", 2) PUSH_INS(Sub,);
                    ELIFNODE("*", 2) PUSH_INS(Mul,);
                    ELIFNODE("/", 2) PUSH_INS(Div,);
                    ELIFNODE("^", 2) PUSH_INS(Pow,);
                    ELSE LATAN_ERROR(Compilation, "unknown operator '"
                                     + n.getName() + "'");
                }
                break;
            case MathNode::Type::keyw:
                if (n.getName() == "return")
                {
                    compileNode(n[0]);
                    gotReturn_ = true;
                }
                else
                {
                    LATAN_ERROR(Compilation, "unknown keyword '" + n.getName()
                                + "'");
                }
                break;
            case MathNode::Type::func:
                for (unsigned int i = 0; i < n.getNArg(); ++i)
                {
                    compileNode(n[i]);
                }
                PUSH_INS(Call, n.getName());
                break;
            default:
                LATAN_ERROR(Compilation,
                            "unknown node type (node named '" + n.getName()
                            + "')");
                break;
        }
    }
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
