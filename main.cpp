#include <iostream>

#include "Future.h"
#include "Promise.h"
#include "Async.h"

int testMyPromise( MyPromise<int>& myPromise )
{
    for( int i = 0; i < 100000; i++ ) {
        std::cout << i << "\n";
    }
    myPromise.SetException( std::future_error( std::future_errc::broken_promise ) );
    return 0;
}

int testPromise( std::promise<int>& myPromise )
{
    for( int i = 0; i < 10; i++ ) {
        std::cout << i << "\n";
    }
    myPromise.set_value( 111 );
    return 0;
}


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

int main() {

    int nIterations = 1000000000;
//    auto fut1 = std::async(std::launch::async, foo, true, nIterations);
//    auto fut2 = std::async(std::launch::async, foo, true, nIterations);
    MyAsyncExecutor executor( 20 );
    auto fut1 = executor.Execute<int>( std::bind( foo, true, nIterations), false );
    auto fut2 = executor.Execute<int>( fooN, false );
    auto fut3 = executor.Execute<int>( fooN, false );
    auto fut4 = executor.Execute<int>( fooN, false );
    auto fut5 = executor.Execute<int>( fooN, false );
    auto fut6 = executor.Execute<int>( fooN );
    auto fut7 = executor.Execute<int>( fooN );
    auto fut8 = executor.Execute<int>( fooN );
    auto fut9 = executor.Execute<int>( fooN );
    std::cout << fut1->GetValue() << std::endl;
    std::cout << fut2->GetValue() << std::endl;
    std::cout << fut3->GetValue() << std::endl;
    std::cout << fut4->GetValue() << std::endl;
    std::cout << fut5->GetValue() << std::endl;
    std::cout << fut6->GetValue() << std::endl;
    std::cout << fut7->GetValue() << std::endl;
    std::cout << fut8->GetValue() << std::endl;
    std::cout << fut9->GetValue() << std::endl;
    std::cout << "Finish\n";
    return 0;
}
