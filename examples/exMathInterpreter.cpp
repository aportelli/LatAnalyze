#include <iostream>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/MathInterpreter.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    string source;
    
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <program>" << endl;
        
        return EXIT_FAILURE;
    }
    source = argv[1];
    
    MathInterpreter interpreter(source);
    RunContext context;
    
    cout << "-- Source code:" << endl << source << endl << endl;
    interpreter.compile(context);
    cout << "-- Abstract Syntax Tree:" << endl;
    if (interpreter.getAST())
    {
        cout << *interpreter.getAST() << endl;
    }
    else
    {
        cout << "<null>" << endl << endl;
    }
    cout << "-- Program:" << endl << interpreter << endl;
    cout << "-- Variable table:" << endl;
    for (auto &v: context.getVariableTable())
    {
        cout << "'" << v.first << "': " << v.second << endl;
    }
    cout << endl;
    cout << "-- Function table:" << endl;
    for (auto &f: context.getFunctionTable())
    {
        cout << "'" << f.first << "': " << f.second << endl;
    }
    cout << endl;
    interpreter(context);
    if (!context.stack().empty())
    {
        cout << "-- Result: " << context.stack().top() << endl;
    }
    
    return EXIT_SUCCESS;
}
