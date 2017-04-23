//
// Created by nismohl on 4/23/17.
//

#pragma once

#include "Future.h"
#include "Promise.h"

#include <iostream>
#include <thread>

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

template <typename R>
class MyTask
{
    std::function<R()> functionToExecute;
    MyPromise<R> promiseForResult;
public:
    explicit MyTask( std::function<R()> functionToExecute ) : functionToExecute( functionToExecute ) {}

    void operator()()
    {
        promiseForResult.SetValue( functionToExecute() );
    }

    MyFuture<R> get_future() { return promiseForResult.GetFuture(); }
};

template<typename R>
MyFuture<R> MyAsync( std::function<R()> functionToExecute )
{
    MyTask<R> newTask( functionToExecute );
    MyFuture<R> taskFuture = newTask.get_future();
    std::thread t1( std::move( newTask ) );
    t1.detach();
    return taskFuture;
}
