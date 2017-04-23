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
    for( int i = 0; i < 100000; i++ ) {
        std::cout << i << "\n";
    }
    myPromise.set_value( 111 );
    return 0;
}

int main() {

    int nIterations = 1000000000;
//    auto fut1 = std::async(std::launch::async, foo, true, nIterations);
//    auto fut2 = std::async(std::launch::async, foo, true, nIterations);
    auto fut1 = MyAsync<int>( std::bind( foo, true, nIterations) );
    auto fut2 = MyAsync<int>( fooN );
    auto fut3 = MyAsync<int>( fooN );
    int sum = 0;
    for( int i = 0; i < nIterations; ++i ){
        sum++;
    }
    std::cout << fut1.GetValue() << ' ' << fut2.GetValue() << " " << fut3.GetValue() << " " << sum << std::endl;
    return 0;
}
