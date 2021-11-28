/*
 * ThreadPool.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2021 Antonin Portelli
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

#include <LatAnalyze/Core/ThreadPool.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                         ThreadPool implementation                          *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
ThreadPool::ThreadPool(void)
: ThreadPool(std::thread::hardware_concurrency())
{}

ThreadPool::ThreadPool(const unsigned int nThreads)
: nThreads_(nThreads)
{
    for (unsigned int t = 0; t < nThreads_; ++t)
    {
        threads_.push_back(thread(&ThreadPool::workerLoop, this));
    }
}

// destructor //////////////////////////////////////////////////////////////////
ThreadPool::~ThreadPool(void)
{
    terminate();
}

// get the number of threads ///////////////////////////////////////////////////
unsigned int ThreadPool::getThreadNum(void) const
{
    return nThreads_;
}

// get the pool mutex for synchronisation //////////////////////////////////////
std::mutex & ThreadPool::getMutex(void)
{
    return mutex_;
}

// worker loop /////////////////////////////////////////////////////////////////
void ThreadPool::workerLoop(void)
{
    while (true)
    {
        Job job;
        {
            unique_lock<mutex> lock(mutex_);

            condition_.wait(lock, [this](){
                return !queue_.empty() || terminatePool_;
            });
            if (terminatePool_ and queue_.empty())
            {
                return;
            }
            job = queue_.front();
            queue_.pop();
        }
        job();
    }
}

// add jobs ////////////////////////////////////////////////////////////////////
void ThreadPool::addJob(Job newJob)
{
    {
        unique_lock<mutex> lock(mutex_);

        queue_.push(newJob);
    }
    condition_.notify_one();
}

// wait for completion /////////////////////////////////////////////////////////
void ThreadPool::terminate(void)
{
    {
        unique_lock<mutex> lock(mutex_);

        terminatePool_ = true;
    }
    condition_.notify_all();
    for (auto &thread: threads_)
    {
        thread.join();
    }
    threads_.clear();
}
