/*
 * MathInterpreter.hpp, part of LatAnalyze 3
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

#ifndef Latan_MathInterpreter_hpp_
#define	Latan_MathInterpreter_hpp_

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <stack>
#include <latan/Function.hpp>
#include <latan/Global.hpp>
#include <latan/ParserState.hpp>

#define MAXIDLENGTH 256

BEGIN_NAMESPACE

/******************************************************************************
 *                              Parser classes                                *
 ******************************************************************************/
class MathNode
{
public:
    class Type
    {
    public:
        enum
        {
            undef = -1,
            cst   =  0,
            op    =  1,
            var   =  2,
            keyw  =  3,
            func  =  4
        };
    };
public:
    // constructor
    MathNode(const std::string &name, const unsigned int type);
    MathNode(const std::string &name, const unsigned int type,
             const unsigned int nArg, ...);
    // destructor
    virtual ~MathNode();
    // access
    const std::string& getName(void)   const;
    unsigned int       getType(void)   const;
    unsigned int       getNArg(void)   const;
    const MathNode *   getParent(void) const;
    unsigned int       getLevel(void)  const;
    void               setName(const std::string &name);
    void               pushArg(MathNode *node);
    // operator
    const MathNode &operator[](const unsigned int i) const;
private:
    std::string             name_;
    unsigned int            type_;
    std::vector<MathNode *> arg_;
    const MathNode *        parent_;
};

std::ostream &operator<<(std::ostream &out, const MathNode &n);

/******************************************************************************
 *                         Instruction classes                                *
 ******************************************************************************/
typedef std::map<std::string, double>           VarTable;
typedef std::map<std::string, DoubleFunction *> FunctionTable;

struct RunContext
{
    unsigned int       insIndex;
    std::stack<double> dStack;
    VarTable           vTable;
    FunctionTable      fTable;
};

// Abstract base
class Instruction
{
public:
    virtual ~Instruction();
    // instruction execution
    virtual void operator()(RunContext &context) const = 0;
    friend std::ostream& operator<<(std::ostream &out, const Instruction &ins);
private:
    virtual void print(std::ostream &out) const = 0;
};

// Push
class Push: public Instruction
{
private:
    class ArgType
    {
    public:
        enum
        {
            Constant = 0,
            Variable = 1
        };
    };
public:
    //constructors
    explicit Push(const double val);
    explicit Push(const std::string &name);
    // instruction execution
    virtual void operator()(RunContext &context) const;
private:
    virtual void print(std::ostream& out) const;
private:
    unsigned int type_;
    double       val_;
    std::string  name_;
};

// Pop
class Pop: public Instruction
{
public:
    //constructor
    explicit Pop(const std::string &name);
    // instruction execution
    virtual void operator()(RunContext &context) const;
private:
    virtual void print(std::ostream& out) const;
private:
    std::string name_;
};

// Store
class Store: public Instruction
{
public:
    //constructor
    explicit Store(const std::string &name);
    // instruction execution
    virtual void operator()(RunContext &context) const;
private:
    virtual void print(std::ostream& out) const;
private:
    std::string name_;
};

// Call function
class Call: public Instruction
{
public:
    //constructor
    explicit Call(const std::string &name);
    // instruction execution
    virtual void operator()(RunContext &context) const;
private:
    virtual void print(std::ostream& out) const;
private:
    std::string name_;
};

// Floating point operations
#define DECL_OP(name)\
class name: public Instruction\
{\
public:\
virtual void operator()(RunContext &context) const;\
private:\
    virtual void print(std::ostream &out) const;\
}

DECL_OP(Neg);
DECL_OP(Add);
DECL_OP(Sub);
DECL_OP(Mul);
DECL_OP(Div);
DECL_OP(Pow);

/******************************************************************************
 *                               Compiler class                               *
 ******************************************************************************/
class MathInterpreter
{

public:
    // parser state
    class MathParserState: public ParserState<MathNode *>
    {
    public:
        // constructor
        explicit MathParserState(std::istream *stream, std::string *name,
                                 MathNode **data);
        // destructor
        virtual ~MathParserState(void);
    private:
        // allocation/deallocation functions defined in MathLexer.lpp
        virtual void initScanner(void);
        virtual void destroyScanner(void);
    };
private:
    // status flags
    class Status
    {
    public:
        enum
        {
            none        = 0,
            initialised = 1 << 0,
            parsed      = 1 << 1,
            compiled    = 1 << 2
        };
    };
    // instruction container
    typedef std::vector<const Instruction *> InstructionContainer;
public:
    // constructors
    MathInterpreter(void);
    MathInterpreter(const std::string &code);
    // destructor
    ~MathInterpreter(void);
    // access
    const Instruction * operator[](const unsigned int i) const;
    const MathNode * getAST(void) const;
    // initialization
    void setCode(const std::string &code);
    // interpreter
    void compile(void);
    // execution
    void operator()(RunContext &context);
    // IO
    friend std::ostream &operator<<(std::ostream &out,
                                    const MathInterpreter &program);
private:
    // initialization
    void reset(void);
    // access
    void push(const Instruction *i);
    // parser
    void parse(void);
    // interpreter
    void compileNode(const MathNode &node);
    // execution
    void execute(RunContext &context) const;
private:
    std::istream         *code_;
    std::string          codeName_;
    MathParserState      *state_;
    MathNode             *root_;
    bool                 gotReturn_;
    InstructionContainer program_;
    unsigned int         status_;
};

END_NAMESPACE

#endif // Latan_MathInterpreter_hpp_
