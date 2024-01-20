/*
 * MathInterpreter.hpp, part of LatAnalyze 3
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

#ifndef Latan_MathInterpreter_hpp_
#define	Latan_MathInterpreter_hpp_

#include <cstdint>

#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Core/ParserState.hpp>

#define MAXIDLENGTH 256

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                       Class for runtime context                            *
 ******************************************************************************/
class RunContext
{
public:
    typedef std::map<std::string, unsigned int> AddressTable;
public:
    // constructor
    RunContext(void) = default;
    // destructor
    ~RunContext(void) = default;
    // access
    unsigned int         addFunction(const std::string &name,
                                     DoubleFunction *init = nullptr);
    unsigned int         addVariable(const std::string &name,
                                     const double init = 0.);
    DoubleFunction *     getFunction(const std::string &name) const;
    DoubleFunction *     getFunction(const unsigned int address) const;
    unsigned int         getFunctionAddress(const std::string &name) const;
    const AddressTable & getFunctionTable(void) const;
    unsigned int         getInsIndex(void) const;
    double               getVariable(const std::string &name) const;
    double               getVariable(const unsigned int address) const;
    unsigned int         getVariableAddress(const std::string &name) const;
    const AddressTable & getVariableTable(void) const;
    void                 incrementInsIndex(const unsigned int inc = 1);
    void                 setFunction(const std::string &name,
                                     DoubleFunction *f);
    void                 setFunction(const unsigned int address,
                                     DoubleFunction *f);
    void                 setInsIndex(const unsigned index);
    void                 setVariable(const std::string &name,
                                     const double value);
    void                 setVariable(const unsigned int address,
                                     const double value);
    std::stack<double> & stack(void);
    // reset
    void                 reset(void);
private:
    unsigned int                  insIndex_;
    std::stack<double>            dStack_;
    std::vector<double>           vMem_;
    std::vector<DoubleFunction *> fMem_;
    AddressTable                  vTable_, fTable_;
};

/******************************************************************************
 *                         Instruction classes                                *
 ******************************************************************************/
// Abstract base

enum class Instruction : std::uint8_t {
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    NEG,
    CONST,
    POP,
    LOAD,
    STORE,
    CALL,
    RET,
};

std::ostream & operator<<(std::ostream &out, const Instruction &ins);

// Instruction container
typedef std::vector<std::uint8_t> Program;

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
    virtual void compile(Program &program, RunContext &context) const = 0;
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
    virtual void compile(Program &program, RunContext &context) const;\
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
    virtual void compile(Program &program, RunContext &context) const = 0;
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
    void compile(RunContext &context);
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

END_LATAN_NAMESPACE

#endif // Latan_MathInterpreter_hpp_
