#include <LatAnalyze/Core/ThreadPool.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    ThreadPool pool;

    cout << "Using " << pool.getThreadNum() << " threads" << endl;
    for (unsigned int i = 1; i <= 20; ++i)
    {
        pool.addJob([i, &pool](void)
        {
            {
                unique_lock<mutex> lock(pool.getMutex());
                cout << "job " << i << " wait for " << i*100 << " ms" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(i*100));
            {
                unique_lock<mutex> lock(pool.getMutex());
                cout << "job " << i << " done" << endl;
            }
        });
    }
    pool.terminate();

    return EXIT_SUCCESS;
}
