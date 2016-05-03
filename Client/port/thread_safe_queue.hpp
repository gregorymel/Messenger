#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <atomic>


class Semaphore
{
	// copy and assignment not allowed
	void operator=( const Semaphore&) = delete;
	Semaphore( const Semaphore&) = delete;

	std::mutex sem_mutex;
	std::atomic<int>  m_count;
public:
	Semaphore( int start) : m_count( 0) { sem_mutex.lock(); };
	void wait()
	{
		if ( m_count.fetch_sub( 1, std::memory_order_acquire) <= 0)
			sem_mutex.lock();
	}
	void signal()
	{
		if ( m_count.fetch_add( 1, std::memory_order_release) < 0)
			sem_mutex.unlock(); 
	}
};


template <typename T>
class ThreadSafeQueue
{
	// copy and assignment not allowed
	void operator=( const ThreadSafeQueue&) = delete;
	ThreadSafeQueue( const ThreadSafeQueue&) = delete;

	Semaphore m_sem;

	std::queue<T> queue;

public:
	ThreadSafeQueue() : m_sem( 0){};
	void push( T item);
	T pop();
};

template <typename T>
void ThreadSafeQueue<T>::push( T item)
{
	queue.push( item);
	m_sem.signal();
}

template <typename T>
T ThreadSafeQueue<T>::pop()
{
	m_sem.wait();
	T item = std::move( queue.front());
	queue.pop();
	return item;
}