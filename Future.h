//
// Created by nismohl on 4/23/17.
//

#pragma once

#include <condition_variable>
#include<future>

template<typename T> class MyPromise;

class IReadyCheckable {
public:
    virtual bool IsReady() = 0;
};

template<typename T>
class MyFuture : public IReadyCheckable {
public:
    friend class MyPromise<T>;

    T GetValue();
    bool TryGetValue( T& newValue );

    bool IsReady();

private:
    struct SharedState {
        bool isReady;
        bool isException;
        T storedValue;
        std::exception storedException;
    };

    bool isValid;
    std::shared_ptr<std::mutex> criticalSectionLock;
    std::shared_ptr<std::condition_variable> isReadyCV;
    std::shared_ptr<SharedState> sharedState;

    MyFuture( std::shared_ptr<SharedState> promiseSharedState,
              std::shared_ptr<std::condition_variable> isReadyCV,
              std::shared_ptr<std::mutex> criticalSectionLock );

    void checkValidity();
    void checkException();
};

template<typename T>
T MyFuture<T>::GetValue()
{
    checkValidity();
    std::unique_lock<std::mutex> lock( *criticalSectionLock );
    while( !sharedState->isReady ) {
        isReadyCV->wait( lock );
    }
    isValid = false;
    checkException();

    T value = sharedState->storedValue;

    // удаление sharedState
    sharedState.reset();
    return value;
}

template<typename T>
bool MyFuture<T>::TryGetValue( T& newValue )
{
    checkValidity();
    std::unique_lock<std::mutex> lock( *criticalSectionLock );
    if( sharedState->isReady ) {
        isValid = false;
        checkException();
        newValue = sharedState->storedValue;
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool MyFuture<T>::IsReady()
{
    checkValidity();
    std::unique_lock<std::mutex> lock( *criticalSectionLock );
    return sharedState->isReady;
}

template<typename T>
MyFuture<T>::MyFuture( std::shared_ptr<SharedState> promiseSharedState,
                       std::shared_ptr<std::condition_variable> isReadyCV,
                       std::shared_ptr<std::mutex> criticalSectionLock )
        : sharedState( promiseSharedState ),
          isReadyCV( isReadyCV ),
          criticalSectionLock( criticalSectionLock),
          isValid( true) {}

template<typename T>
void MyFuture<T>::checkValidity()
{
    if( !isValid ) {
        throw std::future_error( std::future_errc::no_state );
    }
}

template<typename T>
void MyFuture<T>::checkException()
{
    if( sharedState->isException ) {
        throw sharedState->storedException;
    }
}
