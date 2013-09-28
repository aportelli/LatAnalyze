#ifndef LATAN_MATHCOMPILER_HPP_
#define	LATAN_MATHCOMPILER_HPP_

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <stack>
#include <latan/Global.hpp>
#include <latan/ParserState.hpp>

#define MAXIDLENGTH 256

LATAN_BEGIN_CPPDECL

/******************************************************************************
 *                              Parser classes                                *
 ******************************************************************************/
namespace NodeType
{
    typedef enum
    {
        Constant = 0,\
        Operator = 1,\
        Variable = 2
    } Type;
}

class MathNode
{
public:
    // constructor
    MathNode(const std::string& init_str, NodeType::Type init_type);
    MathNode(const std::string& init_str, NodeType::Type init_type,\
             const unsigned int narg, ...);
    // destructor
    virtual ~MathNode();
    // access
    const std::string& String(void) const;
    NodeType::Type     Type(void)   const;
    unsigned int       NArg(void)   const;
    // operators
    const MathNode& operator[](const unsigned int i) const;
private:
    // private members
    std::string            str;
    NodeType::Type         type;
    std::vector<MathNode*> arg;
};

class MathParserState: public ParserState<MathNode*>
{
public:
    // constructor
    explicit MathParserState(std::istream* pt_stream, std::string* name,\
                             MathNode** pt_data);
    // destructor
    virtual ~MathParserState(void);
private:
    // allocation/deallocation functions defined in IOASCIILexer.lpp
    void init_scanner(void);
    void destroy_scanner(void);
};

/******************************************************************************
 *                       Virtual machine code classes                         *
 ******************************************************************************/
typedef std::map<std::string,double> VarTable;

// Abstract base
class Instruction
{
public:
    virtual ~Instruction();
    // instruction execution
    virtual void operator()(std::stack<double>& dstack, VarTable& vtable) = 0;
    friend std::ostream& operator<<(std::ostream& out, const Instruction& ins);
private:
    virtual void Print(std::ostream& out) const;
};

// push and pop
class Push: public Instruction
{
public:
    //constructors
    explicit Push(const double& init_cst);
    explicit Push(const std::string& init_vname);
    // instruction execution
    virtual void operator()(std::stack<double>& dstack, VarTable& vtable);
private:
    typedef enum {Constant = 0, Variable = 1} ArgType;
    ArgType     type;
    double      cst;
    std::string vname;
    virtual void Print(std::ostream& out) const;
};

class Pop: public Instruction
{
public:
    //constructor
    explicit Pop(const std::string& init_vname);
    // instruction execution
    virtual void operator()(std::stack<double>& dstack, VarTable& vtable);
private:
    std::string vname;
    virtual void Print(std::ostream& out) const;
};

// Float operations
#define DECL_OP(name)\
class name: public Instruction\
{\
public:\
    virtual void operator()(std::stack<double>& dstack, VarTable& vtable);\
private:\
    virtual void Print(std::ostream& out) const;\
}

DECL_OP(Neg);
DECL_OP(Add);
DECL_OP(Sub);
DECL_OP(Mul);
DECL_OP(Div);
DECL_OP(Pow);

class VirtualProgram: public std::vector<Instruction*>
{
public:
    VirtualProgram(void);
    friend std::ostream& operator<<(std::ostream& out,        \
                                    const VirtualProgram& prog);
};

/******************************************************************************
 *                               Compiler class                               *
 ******************************************************************************/
class MathCompiler
{
public:
    // constructors
    MathCompiler(void);
    MathCompiler(const std::string& code_str);
    // destructor
    ~MathCompiler(void);
    // initialization
    void Init(const std::string& code_str);
    const VirtualProgram& operator()(void);
private:
    // status flags
    enum
    {
        Initialised = 1 << 0,
        Parsed      = 1 << 1,
        Compiled    = 1 << 2
    };
    // private members
    std::istream*    code;
    std::string      code_name;
    MathParserState* state;
    MathNode*        root;
    VirtualProgram   out;
    unsigned int     status;
    // private methods
    void Parse(void);
    void Compile(const MathNode& N);
    void Reset(void);
};

LATAN_END_CPPDECL

#endif
