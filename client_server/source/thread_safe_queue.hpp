#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
	// copy and assignment not allowed
	void operator=( const ThreadSafeQueue&) = delete;
	ThreadSafeQueue( const ThreadSafeQueue&) = delete;

	std::queue<T> queue;
	std::mutex q_lock; // queue lock

	std::mutex cv_mutex; // mutex for condition variable
	std::condition_variable cv;

	std::mutex f_lock; // fullness lock
	int fullness;

public:
	ThreadSafeQueue() : fullness( 0) {};
	void push( T item);
	T pop();
};

template <typename T>
void ThreadSafeQueue<T>::push( T item)
{
	std::lock_guard<std::mutex> lock( q_lock);
	queue.push( item);
	cv.notify_one();
	
	std::lock_guard<std::mutex> lck( f_lock);
	fullness++;
}

template <typename T>
T ThreadSafeQueue<T>::pop()
{
	f_lock.lock();
	if ( fullness != 0)
		fullness--;
	else
	{
		std::unique_lock<std::mutex> lock( cv_mutex);	
		cv.wait( lock);
	}
	f_lock.unlock();

	std::lock_guard<std::mutex> lock( q_lock);
	T temp;
	temp = queue.front();
	queue.pop();
	return temp;
}