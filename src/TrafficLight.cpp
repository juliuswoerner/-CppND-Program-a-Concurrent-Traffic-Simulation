#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "TrafficLight.h"
#include "TrafficObject.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> lock(_mutex);
    _condition_variable.wait(lock);
    return _queue.back();

}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lock(_mutex);
    _condition_variable.notify_one();
    _queue.emplace_back(msg);
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{
    delete &_currentPhase;
}

bool TrafficLight::isTrafficLightRed(){
    if(TrafficLight::getCurrentPhase() == green){
        return false;
    }
    else{ return true; }
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(_mq.receive()==TrafficLightPhase::red){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
/* 
TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}
*/
void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    // TODO: second part
    TrafficLight::cycleThroughPhases();
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    while(true){
        srand((unsigned)time(0));
        int phase_time;
        phase_time = (rand()%3) + 4;
        std::this_thread::sleep_for(std::chrono::seconds(phase_time));
        if(getCurrentPhase()==TrafficLightPhase::red){
            TrafficLight::setCurrentPhase(TrafficLightPhase::green);
        }
        else{
            TrafficLight::setCurrentPhase(TrafficLightPhase::red);
        }
        _mq.send(getCurrentPhase());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}