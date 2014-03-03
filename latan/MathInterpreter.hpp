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
    // destructor
    virtual ~Instruction(void) = default;
    // instruction execution
    virtual void operator()(RunContext &context) const = 0;
    friend std::ostream & operator<<(std::ostream &out, const Instruction &ins);
private:
    virtual void print(std::ostream &out) const = 0;
};

std::ostream & operator<<(std::ostream &out, const Instruction &ins);

// Instruction container
typedef std::vector<std::unique_ptr<const Instruction>> Program;

// Push
class Push: public Instruction
{
private:
    enum class ArgType
    {
        Constant = 0,
        Variable = 1
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
    ArgType     type_;
    double      val_;
    std::string name_;
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
 *                            Expression node classes                         *
 ******************************************************************************/
class ExprNode
{
public:
    // constructors
    explicit ExprNode(const std::string &name);
    // destructor
    virtual ~ExprNode() = default;
    // access
    const std::string& getName(void)   const;
    Index              getNArg(void)   const;
    const ExprNode *   getParent(void) const;
    Index              getLevel(void)  const;
    void               setName(const std::string &name);
    void               pushArg(ExprNode *node);
    // operator
    const ExprNode &operator[](const Index i) const;
    // compile
    virtual void compile(Program &program) const = 0;
private:
    std::string                             name_;
    std::vector<std::unique_ptr<ExprNode>>  arg_;
    const ExprNode *                        parent_;
};

std::ostream &operator<<(std::ostream &out, const ExprNode &n);

#define DECL_NODE(base, name) \
class name: public base\
{\
public:\
    using base::base;\
    virtual void compile(Program &program) const;\
}

DECL_NODE(ExprNode, VarNode);
DECL_NODE(ExprNode, CstNode);
DECL_NODE(ExprNode, SemicolonNode);
DECL_NODE(ExprNode, AssignNode);
DECL_NODE(ExprNode, MathOpNode);
DECL_NODE(ExprNode, FuncNode);

class KeywordNode: public ExprNode
{
public:
    using ExprNode::ExprNode;
    virtual void compile(Program &program) const = 0;
};

DECL_NODE(KeywordNode, ReturnNode);

/******************************************************************************
 *                            Interpreter class                               *
 ******************************************************************************/
class MathInterpreter
{
public:
    // parser state
    class MathParserState: public ParserState<std::unique_ptr<ExprNode>>
    {
    public:
        // constructor
        explicit MathParserState(std::istream *stream, std::string *name,
                                 std::unique_ptr<ExprNode> *data);
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
public:
    // constructors
    MathInterpreter(void) = default;
    MathInterpreter(const std::string &code);
    // destructor
    ~MathInterpreter(void) = default;
    // access
    const Instruction * operator[](const Index i) const;
    const ExprNode * getAST(void) const;
    // initialization
    void setCode(const std::string &code);
    // interpreter
    void compile(void);
    // execution
    void operator()(RunContext &context);
    // IO
    friend std::ostream & operator<<(std::ostream &out,
                                     const MathInterpreter &program);
private:
    // initialization
    void reset(void);
    // access
    void push(const Instruction *i);
    // parser
    void parse(void);
    // interpreter
    void compileNode(const ExprNode &node);
    // execution
    void execute(RunContext &context) const;
private:
    std::unique_ptr<std::istream>    code_{nullptr};
    std::string                      codeName_{"<no_code>"};
    std::unique_ptr<MathParserState> state_{nullptr};
    std::unique_ptr<ExprNode>        root_{nullptr};
    Program                          program_;
    unsigned int                     status_{Status::none};
};

std::ostream & operator<<(std::ostream &out, const MathInterpreter &program);

END_NAMESPACE

#endif // Latan_MathInterpreter_hpp_
