//
// Created by nismohl on 4/23/17.
//

#pragma once
#include "Future.h"

template<typename T>
class MyPromise {
public:
    typedef typename MyFuture<T>::SharedState SharedState ;

    MyPromise();

    std::shared_ptr<MyFuture<T>> GetFuture();
    void SetValue( const T& newValue );
    void SetException( const std::exception& newException );

private:
    bool isFutureRetrieved;
    std::shared_ptr<std::mutex> criticalSectionLock;
    std::shared_ptr<std::condition_variable> isReadyCV;
    std::shared_ptr<SharedState> sharedState;
};

template<typename T>
MyPromise<T>::MyPromise()
        : sharedState( new SharedState() ),
          criticalSectionLock( new std::mutex() ),
          isReadyCV( new std::condition_variable() ),
          isFutureRetrieved( false ) {

    sharedState->isReady = false;
    sharedState->isException = false;
}

template<typename T>
std::shared_ptr<MyFuture<T>> MyPromise<T>::GetFuture()
{
    if( !isFutureRetrieved ) {
        isFutureRetrieved = true;
        return std::shared_ptr<MyFuture<T>>( new MyFuture<T>( sharedState, isReadyCV, criticalSectionLock ) );
    } else {
        throw std::future_errc( std::future_errc::future_already_retrieved );
    }
}

template<typename T>
void MyPromise<T>::SetValue( const T& newValue )
{
    std::unique_lock<std::mutex> lock( *criticalSectionLock );
    if( sharedState->isReady ) {
        throw std::future_errc( std::future_errc::promise_already_satisfied );
    }
    sharedState->isReady = true;
    sharedState->storedValue = newValue;
    isReadyCV->notify_one();
}

template<typename T>
void MyPromise<T>::SetException( const std::exception& newException )
{
    std::unique_lock<std::mutex> lock( *criticalSectionLock );
    if( sharedState->isReady ) {
        throw std::future_errc( std::future_errc::promise_already_satisfied );
    }
    sharedState->isReady = true;
    sharedState->isException = true;
    sharedState->storedException = newException;
    isReadyCV->notify_one();
}
