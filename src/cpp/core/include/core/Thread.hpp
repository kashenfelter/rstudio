/*
 * Thread.hpp
 *
 * Copyright (C) 2009-12 by RStudio, Inc.
 *
 * Unless you have received this program directly from RStudio pursuant
 * to the terms of a commercial license agreement with RStudio, then
 * this program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */

#ifndef CORE_THREAD_HPP
#define CORE_THREAD_HPP

#include <queue>

#include <boost/utility.hpp>
#include <boost/function.hpp>

#include <core/BoostErrors.hpp>
#include <core/BoostThread.hpp>
#include <core/Error.hpp>
#include <core/Log.hpp>



#define LOCK_MUTEX(m) try { \
   boost::lock_guard<boost::mutex> lock(m); 

#define END_LOCK_MUTEX } \
   catch(const boost::thread_resource_error& e) \
   { \
      Error threadError(boost::thread_error::ec_from_exception(e), \
                        ERROR_LOCATION) ; \
      LOG_ERROR(threadError); \
   }

namespace core {
namespace thread {
      
template <typename T>
class ThreadsafeValue : boost::noncopyable
{
public:
   ThreadsafeValue(const T& value) : value_(value) {}
   virtual ~ThreadsafeValue() {}
   
   T get()
   {
      LOCK_MUTEX(mutex_)
      {
         return value_;
      }
      END_LOCK_MUTEX
      
      // keep compiler happy
      return T();
   }
   
   void set(const T& value)
   {
      LOCK_MUTEX(mutex_)
      {
         value_ = value;
      }
      END_LOCK_MUTEX
   }
   
private:
   boost::mutex mutex_;
   T value_;
};

template <typename T>
class ThreadsafeQueue : boost::noncopyable
{
public:
   explicit ThreadsafeQueue(bool freeSyncObjects = false)
      :  pMutex_(new boost::mutex()),
         pWaitCondition_(new boost::condition()),
         freeSyncObjects_(freeSyncObjects)
   {
   }

   virtual ~ThreadsafeQueue()
   {
      try
      {
         if (freeSyncObjects_)
         {
            delete pMutex_;
            delete pWaitCondition_;
         }
      }
      catch(...)
      {
      }
   }

   // COPYING: boost::noncopyable

public:

   void enque(const T& val)
   {
      LOCK_MUTEX(*pMutex_)
      {
         // enque
         queue_.push(val);
      }
      END_LOCK_MUTEX

      pWaitCondition_->notify_all();
   }

   bool deque(T* pVal)
   {
      LOCK_MUTEX(*pMutex_)
      {
         if (!queue_.empty())
         {
            // remove it
            *pVal = queue_.front();
            queue_.pop();

            // return true
            return true;
         }
         else
         {
            return false;
         }
      }
      END_LOCK_MUTEX

      // keep compiler happy
      return false;
   }

   bool isEmpty()
   {
      LOCK_MUTEX(*pMutex_)
      {
         return queue_.empty();
      }
      END_LOCK_MUTEX

      // keep compiler happy
      return true;
   }

   bool deque(T* pVal, const boost::posix_time::time_duration& waitDuration)
   {
      // first see if we already have one
      if (deque(pVal))
         return true;

      // now wait the specified interval for one to materialize
      if (wait(waitDuration))
         return deque(pVal);
      else
         return false;
   }

   bool wait(const boost::posix_time::time_duration& waitDuration =
                boost::posix_time::time_duration(boost::posix_time::not_a_date_time))
   {
      using namespace boost;
      try
      {
         unique_lock<mutex> lock(*pMutex_);
         if (waitDuration.is_not_a_date_time())
         {
            pWaitCondition_->wait(lock);
            return true;
         }
         else
         {
            system_time timeoutTime = get_system_time() + waitDuration;
            return pWaitCondition_->timed_wait(lock, timeoutTime);
         }
      }
      catch(const thread_resource_error& e)
      {
         Error waitError(boost::thread_error::ec_from_exception(e), ERROR_LOCATION) ;
         LOG_ERROR(waitError);
         return false ;
      }
   }


private:
   // synchronization objects. heap based so that we can control whether
   // they are destroyed or not (boost has been known to crash if a mutex
   // is being destroyed while it is being waited on so sometimes it is
   // better to simply never delete these objects
   boost::mutex* pMutex_ ;
   boost::condition* pWaitCondition_ ;

   // instance data
   const bool freeSyncObjects_;
   std::queue<T> queue_;
};

void safeLaunchThread(boost::function<void()> threadMain,
                      boost::thread* pThread = NULL);
      
} // namespace thread
} // namespace core

#endif // CORE_THREAD_HPP

