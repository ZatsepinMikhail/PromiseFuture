#include <iostream>

#include "Future.h"
#include "Promise.h"

void fooMy( MyPromise<int>& myPromise )
{
    for( int i = 0; i < 100000; i++ ) {
        std::cout << i << "\n";
    }
    myPromise.SetException( std::future_error( std::future_errc::broken_promise ) );
}

void foo( std::promise<int>& myPromise )
{
    for( int i = 0; i < 100000; i++ ) {
        std::cout << i << "\n";
    }
    myPromise.set_value( 111 );
}

int main()
{
    MyPromise<int> myPromise;
    MyFuture<int> myFuture = myPromise.GetFuture();

    //std::promise<int> myPromise;
    //std::future<int> myFuture = myPromise.get_future();
    std::thread t1( fooMy, std::ref( myPromise) );

    //std::cout << myFuture.get() << "\n";

    t1.join();
    std::cout << myFuture.GetValue() << "\n";
    return 0;
}
