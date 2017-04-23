//
// Created by nismohl on 4/23/17.
//

#pragma once

#include "Future.h"
#include "Promise.h"

#include <iostream>
#include <thread>
#include <vector>

template <typename R>
class MyTask
{
    std::function<R()> functionToExecute;
    MyPromise<R> promiseForResult;
public:
    explicit MyTask( std::function<R()> functionToExecute ) : functionToExecute( functionToExecute ) {}

    void operator()()
    {
        try {
            promiseForResult.SetValue( functionToExecute() );
        } catch( std::exception& e ) {
            promiseForResult.SetException( e );
        }
    }

    std::shared_ptr<MyFuture<R>> GetFuture() { return promiseForResult.GetFuture(); }
};

class MyAsyncExecutor {
public:
    MyAsyncExecutor( unsigned int threadNumber )
            : workerNumber( std::max( 1U, std::min( threadNumber, std::thread::hardware_concurrency() - 1 ) ) ),
              lastLaunchedWorkerIndex( -1 ) {}

    ~MyAsyncExecutor()
    {
        for( int i = 0; i < workers.size(); i++ ) {
            if( workers[i].joinable() ) {
                workers[i].join();
            }
        }
    }

    template<typename R>
    std::shared_ptr<MyFuture<R>> Execute(std::function<R()> functionToExecute, bool launchAsynchronously = true )
    {
        MyTask<R> newTask( functionToExecute );
        std::shared_ptr<MyFuture<R>> taskFuture = newTask.GetFuture();

        if( launchAsynchronously ) {
            // инициализация
            if (workers.size() < workerNumber) {
                // асинхронно
                std::cout << "async new\n";
                workers.emplace_back(std::move(newTask));
                futures.emplace_back(taskFuture);
                lastLaunchedWorkerIndex = workers.size() - 1;
                return taskFuture;
            }

            // асинхронно
            for (int i = 0; i < futures.size(); i++) {
                int currentIndex = (lastLaunchedWorkerIndex + i + 1) % futures.size();
                if (workers[currentIndex].joinable() &&
                        futures[currentIndex]->IsReady()) {
                    workers[currentIndex].join();
                    workers.emplace(workers.begin() + currentIndex, std::move(newTask));
                    futures[currentIndex] = taskFuture;
                    lastLaunchedWorkerIndex = currentIndex;
                    std::cout << "async (" << currentIndex << ")\n";
                    return taskFuture;
                }
            }
        }

        // синхронно
        std::cout << "sync\n";
        newTask();
        return taskFuture;
    }
private:
    const int workerNumber;
    int lastLaunchedWorkerIndex;
    std::vector<std::shared_ptr<IReadyCheckable>> futures;
    std::vector<std::thread> workers;
};
