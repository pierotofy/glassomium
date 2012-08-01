/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 The Glassomium Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

/** ConcurrentQueue.h 
http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
*/
#ifndef CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_H

#include "stdafx.h"

template<typename Data>
class ConcurrentQueue
{
private:
    std::queue<Data> queue;
	mutable sf::Mutex mutex;
public:
    void push(Data const& data){
		mutex.lock();
		queue.push(data);
		mutex.unlock();
	}

    bool empty() const{
		mutex.lock();
		bool empty = queue.empty();
		mutex.unlock();
		return empty;
	}

    bool pop(Data& data){
		if(queue.empty())
		{
			return false;
		}
		mutex.lock();        
		data=queue.front();
		queue.pop();
		mutex.unlock();
		return true;
	}
};


#endif