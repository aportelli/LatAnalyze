#include <LatAnalyze/Io/Io.hpp>
#include <LatAnalyze/Functional/CompiledFunction.hpp>
#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/Statistics/Random.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>

using namespace std;
using namespace Latan;

constexpr Index  size          = 1000;
constexpr Index  nSample       = 2000;

int main(void)
{
    random_device rd;
    DMat          var(size, size);
    DVec          mean(size);
    DMatSample    sample(nSample, size, 1), sample2(nSample, size, 1);

    cout << "-- generating " << nSample << " Gaussian random vectors..." << endl;
    var   = DMat::Random(size, size);
    var  *= var.adjoint();
    mean  = DVec::Random(size); 
    RandomNormal mgauss(mean, var, rd());
    sample[central] = mgauss();
    FOR_STAT_ARRAY(sample, s)
    {
        sample[s] = mgauss();
    }
    sample2[central] = mgauss();
    FOR_STAT_ARRAY(sample, s)
    {
        sample2[s] = mgauss();
    }

    cout << "-- check new routines" << endl;

    DMat v, vo;

    cout << "var" << endl;
    auto start = chrono::high_resolution_clock::now();
    vo = sample.varianceOld();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    cout << "time " << diff.count() << endl;
    start = chrono::high_resolution_clock::now();
    v = sample.variance();
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    cout << "diff " << (v - vo).norm() << endl;

    cout << "cov" << endl;
    start = chrono::high_resolution_clock::now();
    vo = sample.covarianceOld(sample2);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    start = chrono::high_resolution_clock::now();
    v = sample.covariance(sample2);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    cout << "diff " << (v - vo).norm() << endl;

    cout << "mean" << endl;
    start = chrono::high_resolution_clock::now();
    vo = sample.meanOld(3, 5);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    start = chrono::high_resolution_clock::now();
    v = sample.mean(3, 5);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    cout << "diff " << (v - vo).norm() << endl;

    cout << "varmat" << endl;
    start = chrono::high_resolution_clock::now();
    vo = sample.varianceMatrixOld();
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    start = chrono::high_resolution_clock::now();
    v = sample.varianceMatrix();
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    cout << "diff " << (v - vo).norm() << endl;

    cout << "covarmat" << endl;
    start = chrono::high_resolution_clock::now();
    vo = sample.covarianceMatrixOld(sample2);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    start = chrono::high_resolution_clock::now();
    v = sample.covarianceMatrix(sample2);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    cout << "diff " << (v - vo).norm() << endl;

    cout << "corrmat" << endl;
    start = chrono::high_resolution_clock::now();
    vo = sample.correlationMatrixOld();
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    start = chrono::high_resolution_clock::now();
    v = sample.correlationMatrix();
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "time " << diff.count() << endl;
    cout << "diff " << (v - vo).norm() << endl;

    return EXIT_SUCCESS;
}
