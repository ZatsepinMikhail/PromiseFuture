//
// Created by nismohl on 4/23/17.
//

#pragma once

#include "Future.h"
#include "Promise.h"

#include <iostream>
#include <thread>
#include <vector>

int foo( bool b, int nIterations )
{
    if( !b ){
        return 0;
    }
    int sum = 0;
    for( int i = 0; i < nIterations; ++i ){
        sum++;
    }
    return sum;
}

int fooN()
{
    int sum = 0;
    for( int i = 0; i < 1000000000; ++i ){
        sum++;
    }
    return sum;
}

class TaskPool {

};

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

    MyFuture<R> get_future() { return promiseForResult.GetFuture(); }
};

class MyAsyncExecutor {
public:
    MyAsyncExecutor( unsigned int threadNumber )
            : workerNumber( std::max( 1U, std::min( threadNumber, std::thread::hardware_concurrency() ) ) ) {}

    template<typename R> MyFuture<R> Execute(std::function<R()> functionToExecute)
    {
        MyTask<R> newTask( functionToExecute );
        MyFuture<R> taskFuture = newTask.get_future();

        // инициализация
        if( workers.size() < workerNumber ) {
            // асинхронно
            std::cout << "async new\n";
            workers.emplace_back( std::move( newTask ) );
            return taskFuture;
        }

        // асинхронно
        for( auto it = workers.begin(); it != workers.end(); it++ ) {
            if( it->joinable() ) {
                it->join();
                workers.emplace( it, std::move( newTask ) );
                std::cout << "async\n";
                return taskFuture;
            }
        }

        // синхронно
        std::cout << "sync\n";
        newTask();
        return taskFuture;
    }
private:
    const int workerNumber;
    std::vector<std::thread> workers;
};
