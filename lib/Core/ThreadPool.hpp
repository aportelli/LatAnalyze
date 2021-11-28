/*
 * ThreadPool.hpp, part of LatAnalyze 3
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

#ifndef Latan_ThreadPool_hpp_
#define Latan_ThreadPool_hpp_

#include <LatAnalyze/Global.hpp>

class ThreadPool
{
public:
    typedef std::function<void(void)> Job;
public:
    // constructors/destructor
    ThreadPool(void);
    ThreadPool(const unsigned int nThreads);
    virtual ~ThreadPool(void);
    // get the number of threads
    unsigned int getThreadNum(void) const;
    // get the pool mutex for synchronisation
    std::mutex & getMutex(void);
    // add jobs
    void addJob(Job newJob);
    // wait for completion and terminate
    void terminate(void);
private:
    // worker loop
    void workerLoop(void);
private:
    unsigned int             nThreads_;
    std::condition_variable  condition_;
    std::vector<std::thread> threads_;
    bool                     terminatePool_{false};
    std::queue<Job>          queue_;
    std::mutex               mutex_;
};

#endif
