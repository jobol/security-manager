/*
 *  Copyright (c) 2000 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Rafal Krypa <r.krypa@samsung.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        service-thread.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Implementation of threads.
 */

#ifndef _SECURITY_MANAGER_SERVICE_THREAD_
#define _SECURITY_MANAGER_SERVICE_THREAD_

#include <cassert>
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>

#include <cstdio>

#include <dpl/exception.h>

#include "generic-event.h"

#define DEFINE_THREAD_EVENT(eventType)                                \
    void Event(const eventType &event) {                              \
        SecurityManager::ServiceThread<ParentClassName>::              \
            Event(event,                                              \
                  this,                                               \
                  &ParentClassName::EventInternal##eventType);        \
    }                                                                 \
    void EventInternal##eventType(const eventType &event)

#define DECLARE_THREAD_EVENT(eventType, methodName)                   \
    void Event(const eventType &event) {                              \
        SecurityManager::ServiceThread<ParentClassName>::              \
            Event(event,                                              \
                  this,                                               \
                  &ParentClassName::methodName);                      \
    }

namespace SecurityManager {

template <class Service>
class ServiceThread {
public:
    typedef Service ParentClassName;
    enum class State {
        NoThread,
        Work,
    };

    ServiceThread()
      : m_state(State::NoThread)
      , m_quit(false)
    {}

    void Create() {
        assert(m_state == State::NoThread);
        m_thread = std::thread(ThreadLoopStatic, this);
        m_state = State::Work;
    }

    void Join() {
        assert(m_state != State::NoThread);
        {
            std::lock_guard<std::mutex> lock(m_eventQueueMutex);
            m_quit = true;
            m_waitCondition.notify_one();
        }
        m_thread.join();
        m_state = State::NoThread;
    }

    virtual ~ServiceThread()
    {
        if (m_state != State::NoThread)
            Join();
        while (!m_eventQueue.empty()){
            auto front = m_eventQueue.front();
            delete front;
            m_eventQueue.pop();
        }
    }

    template <class T>
    void Event(const T &event,
               Service *servicePtr,
               void (Service::*serviceFunction)(const T &))
    {
        EventCallerBase *ec = new EventCaller<T>(event, servicePtr, serviceFunction);
        {
            std::lock_guard<std::mutex> lock(m_eventQueueMutex);
            m_eventQueue.push(ec);
        }
        m_waitCondition.notify_one();
    }

protected:

    struct EventCallerBase {
       virtual void fire() = 0;
       virtual ~EventCallerBase() {}
    };

    template <class T>
    struct EventCaller : public EventCallerBase {
        T *event; Service *target; void (Service::*function)(const T&);
        EventCaller(const T &e, Service *c, void (Service::*f)(const T&)) : event(new T(e)), target(c), function(f) {}
	~EventCaller() { delete event; }
	void fire() { (target->*function)(*event); }
    };

    static void ThreadLoopStatic(ServiceThread *ptr) {
        ptr->ThreadLoop();
    }

    void ThreadLoop(){
        for (;;) {
            EventCallerBase *ec = NULL;
            {
                std::unique_lock<std::mutex> ulock(m_eventQueueMutex);
                if (m_quit)
                    return;
                if (!m_eventQueue.empty()) {
                    ec = m_eventQueue.front();
                    m_eventQueue.pop();
                } else {
                    m_waitCondition.wait(ulock);
                }
            }

            if (ec != NULL) {
                UNHANDLED_EXCEPTION_HANDLER_BEGIN
                {
                    ec->fire();
                }
                UNHANDLED_EXCEPTION_HANDLER_END
                delete ec;
            }
        }
    }

    std::thread m_thread;
    std::mutex m_eventQueueMutex;
    std::queue<EventCallerBase*> m_eventQueue;
    std::condition_variable m_waitCondition;

    State m_state;
    bool m_quit;
};

} // namespace SecurityManager

#endif // _SECURITY_MANAGER_SERVICE_THREAD_
