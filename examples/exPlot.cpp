#include <iostream>
#include <latan/Plot.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    Plot p;
    
    p << PlotCommand("x**2") << PlotCommand("x**3") << PlotCommand("x**4");
    cout << p << endl;
    p.display();
    
    return EXIT_SUCCESS;
}
