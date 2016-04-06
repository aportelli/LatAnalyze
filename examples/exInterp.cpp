#include <LatAnalyze/TabFunction.hpp>

int main(void)
{
    Latan::DVec xs(3);
    xs << -1.0, 0.0, 1.0;
    Latan::DVec ys(3);
    ys << 1.0, 0.0, 1.0;

    auto tab = Latan::TabFunction(xs, ys, Latan::InterpType::QUADRATIC);

    std::cout << "Interpolating naive y = x^2 data using quadratic ";
    std::cout << "interpolation..." << std::endl;
    for (double x = -1.0; x < 1.0; x += 0.1) {
        double y = tab(&x);
        std::cout << "y @ " << x << " = " << y;
        std::cout << " ( " << x * x << " expected)" << std::endl;
    }
    std::cout << std::endl;

    tab = Latan::TabFunction(xs, ys, Latan::InterpType::NEAREST);

    std::cout << "Interpolating naive y = x^2 data using nearest ";
    std::cout << "interpolation..." << std::endl;
    for (double x = -1.0; x < 1.0; x += 0.1) {
        double y = tab(&x);
        std::cout << "y @ " << x << " = " << y;
        double expected = (x > 0.5) ? 1.0 : ((x <= -0.5) ? 1.0 : 0.0);
        std::cout << " ( " << expected << " expected)" << std::endl;
    }
}
