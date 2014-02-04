#ifndef LATAN_MATHCOMPILER_HPP_
#define	LATAN_MATHCOMPILER_HPP_

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <stack>
#include <latan/Function.hpp>
#include <latan/Global.hpp>
#include <latan/ParserState.hpp>

#define MAXIDLENGTH 256

LATAN_BEGIN_CPPDECL

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
    // IO
    std::ostream &print(std::ostream &out) const;
private:
    std::string             name_;
    unsigned int            type_;
    std::vector<MathNode *> arg_;
    const MathNode *        parent_;
};

std::ostream &operator<<(std::ostream &out, const MathNode &n);

/******************************************************************************
 *                       Virtual machine code classes                         *
 ******************************************************************************/
typedef std::map<std::string, double>           VarTable;
typedef std::map<std::string, DoubleFunction *> FunctionTable;

// Abstract base
class Instruction
{
public:
    virtual ~Instruction();
    // instruction execution
    virtual void operator()(std::stack<double> &dStack, VarTable &vTable,
                            FunctionTable &fTable) = 0;
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
    virtual void operator()(std::stack<double> &dStack, VarTable &vTable,
                            FunctionTable &fTable);
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
    virtual void operator()(std::stack<double> &dStack, VarTable &vTable,
                            FunctionTable &fTable);
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
    virtual void operator()(std::stack<double> &dStack, VarTable &vTable,
                            FunctionTable &fTable);
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
    virtual void operator()(std::stack<double> &dStack, VarTable &vTable,
                            FunctionTable &fTable);
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
virtual void operator()(std::stack<double> &dStack, VarTable &vTable,\
                        FunctionTable &fTable);\
private:\
    virtual void print(std::ostream &out) const;\
}

DECL_OP(Neg);
DECL_OP(Add);
DECL_OP(Sub);
DECL_OP(Mul);
DECL_OP(Div);
DECL_OP(Pow);

// Virtual program type
typedef std::vector<Instruction *> VirtualProgram;

std::ostream &operator<<(std::ostream &out, const VirtualProgram &program);

/******************************************************************************
 *                               Compiler class                               *
 ******************************************************************************/
class MathCompiler
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
public:
    // constructors
    MathCompiler(void);
    MathCompiler(const std::string &code);
    // destructor
    ~MathCompiler(void);
    // access
    const MathNode * getAST(void) const;
    // initialization
    void init(const std::string &code);
    // compilation
    const VirtualProgram &operator()(void);
private:
    void parse(void);
    void compile(const MathNode &node);
    void reset(void);
private:
    std::istream    *code_;
    std::string     codeName_;
    MathParserState *state_;
    MathNode        *root_;
    bool            gotReturn_;
    VirtualProgram  out_;
    unsigned int    status_;
};

LATAN_END_CPPDECL

#endif
