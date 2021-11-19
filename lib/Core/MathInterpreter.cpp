/*
 * MathInterpreter.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#include <LatAnalyze/Core/MathInterpreter.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Core/Math.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                       RunContext implementation                            *
 ******************************************************************************/
// access //////////////////////////////////////////////////////////////////////
unsigned int RunContext::addFunction(const string &name, DoubleFunction *init)
{
    try
    {
        setFunction(name, init);
        
        return getFunctionAddress(name);
    }
    catch (Exceptions::Definition)
    {
        unsigned int address = fTable_.size();
        
        fMem_.push_back(init);
        fTable_[name] = address;
        
        return address;
    }
}

unsigned int RunContext::addVariable(const string &name, double init)
{
    try
    {
        setVariable(name, init);
        
        return getVariableAddress(name);
    }
    catch (Exceptions::Definition)
    {
        unsigned int address = vTable_.size();
        
        vMem_.push_back(init);
        vTable_[name] = address;
        
        return address;
    }
}

DoubleFunction * RunContext::getFunction(const string &name) const
{
    return getFunction(getFunctionAddress(name));
}

DoubleFunction * RunContext::getFunction(const unsigned int address) const
{
    if (address >= fTable_.size())
    {
        LATAN_ERROR(Range, "function address " + strFrom(address)
                    + " out of range");
        
        return nullptr;
    }
    else
    {
        return fMem_[address];
    }
}

unsigned int RunContext::getFunctionAddress(const string &name) const
{
    try
    {
        return fTable_.at(name);
    }
    catch (out_of_range)
    {
        LATAN_ERROR(Definition, "undefined function '" + name + "'");
    }
}

const RunContext::AddressTable & RunContext::getFunctionTable(void) const
{
    return fTable_;
}

unsigned int RunContext::getInsIndex(void) const
{
    return insIndex_;
}

double RunContext::getVariable(const string &name) const
{
    return getVariable(getVariableAddress(name));
}

double RunContext::getVariable(const unsigned int address) const
{
    if (address >= vTable_.size())
    {
        LATAN_ERROR(Range, "variable address " + strFrom(address)
                    + " out of range");
        
        return 0.;
    }
    else
    {
        return vMem_[address];
    }
}

const RunContext::AddressTable & RunContext::getVariableTable(void) const
{
    return vTable_;
}

unsigned int RunContext::getVariableAddress(const string &name) const
{
    try
    {
        return vTable_.at(name);
    }
    catch (out_of_range)
    {
        LATAN_ERROR(Definition, "undefined variable '" + name + "'");
    }
}

void RunContext::incrementInsIndex(const unsigned int inc)
{
    setInsIndex(getInsIndex() + inc);
}

void RunContext::setFunction(const string &name, DoubleFunction *f)
{

    setFunction(getFunctionAddress(name), f);
}

void RunContext::setFunction(const unsigned int address, DoubleFunction *f)
{
    if (address >= fTable_.size())
    {
        LATAN_ERROR(Range, "function address " + strFrom(address)
                    + " out of range");
    }
    else
    {
        fMem_[address] = f;
    }
}

void RunContext::setInsIndex(const unsigned index)
{
    insIndex_ = index;
}

void RunContext::setVariable(const string &name, const double value)
{
    setVariable(getVariableAddress(name), value);
}

void RunContext::setVariable(const unsigned int address, const double value)
{
    if (address >= vTable_.size())
    {
        LATAN_ERROR(Range, "variable address " + strFrom(address)
                    + " out of range");
    }
    else
    {
        vMem_[address] = value;
    }
}

stack<double> & RunContext::stack(void)
{
    return dStack_;
}

// reset ///////////////////////////////////////////////////////////////////////
void RunContext::reset(void)
{
    insIndex_ = 0;
    while (!dStack_.empty())
    {
        dStack_.pop();
    }
    vMem_.clear();
    fMem_.clear();
    vTable_.clear();
    fTable_.clear();
}

/******************************************************************************
 *                            Instruction set                                 *
 ******************************************************************************/
#define CODE_WIDTH 6
#define CODE_MOD   setw(CODE_WIDTH) << left

auto readConstant(Program::const_iterator ip)
    -> std::tuple<double, Program::const_iterator>
{
    double value = 0.0;
    std::copy(ip, ip + sizeof(double), reinterpret_cast<std::uint8_t*>(&value));

    return std::make_tuple(value, ip + sizeof(double));
}

auto readAddress(Program::const_iterator ip)
    -> std::tuple<unsigned int, Program::const_iterator>
{
    unsigned int address = 0.0;
    const auto end = ip + sizeof(unsigned int);
    std::copy(ip, end, reinterpret_cast<std::uint8_t*>(&address));

    return std::make_tuple(address, end);
}

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
    return *arg_[i];
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

// Bytecode helper functions ///////////////////////////////////////////////////
void pushInstruction(Program &program, const Instruction instruction) {
    program.push_back(static_cast<std::uint8_t>(instruction));
}

void pushAddress(Program &program, const unsigned int address) {
    const auto address_ptr = reinterpret_cast<const std::uint8_t*>(&address);
    const auto size = sizeof(unsigned int);
    program.insert(program.end(), address_ptr, address_ptr + size);
}

void pushConstant(Program &program, const double value) {
    const auto value_ptr = reinterpret_cast<const std::uint8_t*>(&value);
    const auto size = sizeof(double);
    program.insert(program.end(), value_ptr, value_ptr + size);
}

// VarNode compile /////////////////////////////////////////////////////////////
void VarNode::compile(Program &program, RunContext &context) const
{
    pushInstruction(program, Instruction::LOAD);
    pushAddress(program, context.getVariableAddress(getName()));
}

// CstNode compile /////////////////////////////////////////////////////////////
void CstNode::compile(Program &program, RunContext &context __dumb) const
{
    pushInstruction(program, Instruction::CONST);
    pushConstant(program, strTo<double>(getName()));
}

// SemicolonNode compile ///////////////////////////////////////////////////////
void SemicolonNode::compile(Program &program, RunContext &context) const
{
    auto &n = *this;
    
    for (Index i = 0; i < getNArg(); ++i)
    {
        bool isAssign     = isDerivedFrom<AssignNode>(&n[i]);
        bool isSemiColumn = isDerivedFrom<SemicolonNode>(&n[i]);
        bool isKeyword    = isDerivedFrom<KeywordNode>(&n[i]);
        
        if (isAssign or isSemiColumn or isKeyword)
        {
            n[i].compile(program, context);
        }
        // Where a variable has just been assigned, pop it off the stack.
        if (isAssign) {
            pushInstruction(program, Instruction::POP);
        }
    }
}

// AssignNode compile //////////////////////////////////////////////////////////
void AssignNode::compile(Program &program, RunContext &context) const
{
    auto &n = *this;
    
    if (isDerivedFrom<VarNode>(&n[0]))
    {
        n[1].compile(program, context);
        const unsigned int address = context.addVariable(n[0].getName());
        pushInstruction(program, Instruction::STORE);
        pushAddress(program, address);
    }
    else
    {
        LATAN_ERROR(Compilation, "invalid LHS for '='");
    }
}

// MathOpNode compile //////////////////////////////////////////////////////////
#define IFNODE(name, nArg) if ((n.getName() == (name)) and (n.getNArg() == nArg))
#define ELIFNODE(name, nArg) else IFNODE(name, nArg)
#define ELSE else

void MathOpNode::compile(Program &program, RunContext &context) const
{
#define PUSH_BINARY_OP(op, instruction) \
    case op: \
        pushInstruction(program, Instruction::instruction); \
        break;

    auto &n = *this;
    
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        n[i].compile(program, context);
    }
    if (n.getName() == "-" and n.getNArg() == 1) {
        pushInstruction(program, Instruction::NEG);
        return;
    }

    if (getNArg() != 2) {
        LATAN_ERROR(Compilation, "unknown operator '" + getName() + "'");
    }

    switch (getName()[0]) {
    PUSH_BINARY_OP('+', ADD)
    PUSH_BINARY_OP('-', SUB)
    PUSH_BINARY_OP('*', MUL)
    PUSH_BINARY_OP('/', DIV)
    PUSH_BINARY_OP('^', POW)
    default:
        LATAN_ERROR(Compilation, "unknown operator '" + getName() + "'");
    }

#undef PUSH_BINARY_OP
}

// FuncNode compile ////////////////////////////////////////////////////////////
void FuncNode::compile(Program &program, RunContext &context) const
{
    auto &n = *this;
    
    for (Index i = 0; i < n.getNArg(); ++i)
    {
        n[i].compile(program, context);
    }
    pushInstruction(program, Instruction::CALL);
    pushAddress(program, context.getFunctionAddress(getName()));
}

// ReturnNode compile ////////////////////////////////////////////////////////////
void ReturnNode::compile(Program &program, RunContext &context) const
{
    auto &n = *this;
    
    n[0].compile(program, context);
    pushInstruction(program, Instruction::RET);
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
    return reinterpret_cast<const Instruction*>(&program_[i]);
}

const ExprNode * MathInterpreter::getAST(void) const
{
    return root_.get();
}

void MathInterpreter::push(const Instruction *i)
{
    pushInstruction(program_, *i);
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
(context).addFunction(#func, &STDMATH_NAMESPACE::func);\

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
            context.addVariable("pi",  Math::pi);
            context.addVariable("inf", Math::inf);
            ADD_STDMATH_FUNCS(context);
            root_->compile(program_, context);
            for (unsigned int i = 0; i < program_.size(); ++i)
            {
                if (static_cast<Instruction>(program_[i]) == Instruction::RET)
                {
                    gotReturn = true;
                    program_.resize(i);
                    program_.shrink_to_fit();
                    break;
                }
            }
        }
        if (!root_ or !gotReturn)
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
#define BINARY_OP_CASE(instruction, expr) \
        case Instruction::instruction: { \
            const auto second = context.stack().top(); \
            context.stack().pop(); \
            const auto first = context.stack().top(); \
            context.stack().pop(); \
            context.stack().push(expr); \
            break; \
        }
    auto ip = program_.begin();

    while (ip != program_.end()) {
        const auto instruction = static_cast<Instruction>(*ip);
        ip++;

        switch (instruction) {
        BINARY_OP_CASE(ADD, first + second)
        BINARY_OP_CASE(SUB, first - second)
        BINARY_OP_CASE(MUL, first * second)
        BINARY_OP_CASE(DIV, first / second)
        BINARY_OP_CASE(POW, std::pow(first, second))
        case Instruction::NEG: {
            const auto operand = context.stack().top();
            context.stack().pop();
            context.stack().push(-operand);
            break;
        }
        case Instruction::CONST: {
            double value = 0.0;
            std::tie(value, ip) = readConstant(ip);
            context.stack().push(value);
            break;
        }
        case Instruction::POP:
            context.stack().pop();
            break;
        case Instruction::LOAD: {
            unsigned int address = 0;
            std::tie(address, ip) = readAddress(ip);
            context.stack().push(context.getVariable(address));
            break;
        }
        case Instruction::STORE: {
            unsigned int address = 0;
            std::tie(address, ip) = readAddress(ip);
            context.setVariable(address, context.stack().top());
            break;
        }
        case Instruction::CALL: {
            unsigned int address = 0;
            std::tie(address, ip) = readAddress(ip);
            auto& stack = context.stack();
            stack.push((*context.getFunction(address))(stack));
            break;
        }
        case Instruction::RET:
            break;
        }
    }

#undef BINARY_OP_CASE
}

Program::const_iterator instructionToStream(
    ostream &out, Program::const_iterator ip)
{
    const auto instruction = static_cast<Instruction>(*ip);
    ip++;

    switch (instruction) {
    case Instruction::ADD:
        out << "ADD";
        break;
    case Instruction::SUB:
        out << "SUB";
        break;
    case Instruction::MUL:
        out << "MUL";
        break;
    case Instruction::DIV:
        out << "DIV";
        break;
    case Instruction::POW:
        out << "POW";
        break;
    case Instruction::NEG:
        out << "NEG";
        break;
    case Instruction::CONST: {
        double value = 0.0;
        std::tie(value, ip) = readConstant(ip);
        out << CODE_MOD << setfill(' ') << "CONST" << value;
        break;
    }
    case Instruction::POP:
        out << "POP";
        break;
    case Instruction::LOAD: {
        unsigned int address = 0;
        std::tie(address, ip) = readAddress(ip);
        out << CODE_MOD << setfill(' ') << "LOAD" << address;
        break;
    }
    case Instruction::STORE: {
        unsigned int address = 0;
        std::tie(address, ip) = readAddress(ip);
        out << CODE_MOD << setfill(' ') << "STORE" << address;
        break;
    }
    case Instruction::CALL: {
        unsigned int address = 0;
        std::tie(address, ip) = readAddress(ip);
        out << CODE_MOD << setfill(' ') << "CALL" << address;
        break;
    }
    case Instruction::RET:
        out << "RET";
        break;
    }

    return ip;
}

ostream &programToStream(ostream &out, const Program &program)
{
    auto ip = program.begin();

    while (ip != program.end()) {
        const auto i = std::distance(program.begin(), ip);
        cout << setw(4) << setfill('0') << right << i << "  ";
        ip = instructionToStream(out, ip);
        out << '\n';
    }

    return out;
}

// IO //////////////////////////////////////////////////////////////////////////
ostream &Latan::operator<<(ostream &out, const MathInterpreter &program)
{
    return programToStream(out, program.program_);
}
