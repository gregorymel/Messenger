#pragma once

#include "thread_safe_queue.hpp"
#include "Stanza.hpp"

//typedef std::string Stanza;

class Port
{
	// copy and assignment not allowed
	void operator=( const Port&) = delete;
	Port( const Port&) = delete;

	ThreadSafeQueue<Stanza> first_stream;	 
	ThreadSafeQueue<Stanza> second_stream;
public:
	Port(){};

	void send_to_user(Stanza _stanza)
	{
		first_stream.push(_stanza);
	}

	void send_to_server(Stanza _stanza)
	{
		second_stream.push(_stanza);
	}

	Stanza recieve_from_user()
	{
		return second_stream.pop();
	}

	Stanza recieve_from_server()
	{
		return first_stream.pop();
	}
};
