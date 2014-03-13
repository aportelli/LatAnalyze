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
    interpreter.compile();
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
    StdMath::addStdMathFunc(context.fTable);
    interpreter(context);
    if (!context.dStack.empty())
    {
        cout << "-- Result: " << context.dStack.top() << endl;
    }
    
    return EXIT_SUCCESS;
}
