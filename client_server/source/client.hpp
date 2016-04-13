#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include "thread_safe_queue.hpp"
#include <iostream>

#define MAX_MSG 1024

using namespace boost::asio;
using namespace std::placeholders;

#define BIND_1( x) std::bind( &Connection::x, shared_from_this())
#define BIND_2( x, y) std::bind( &Connection::x, shared_from_this(), y)
#define BIND_3( x, y, z) std::bind( &Connection::x, shared_from_this(), y, z)
#define BIND_4( x, y, z, a) std::bind( &Connection::x, shared_from_this(), y, z, a)

class Connection;	

class Client
{
	std::string login;

	ip::tcp::endpoint ep;
	//std::shared_ptr<Connection> connection;

	// ports for user interface
	ThreadSafeQueue<std::string> front_to_back_port;
	ThreadSafeQueue<std::string> back_to_front_port;

public:
	io_service service;
	std::shared_ptr<io_service::work> work;

	//  BACKEND SIDE
	const std::string back_pop()
	{
		return front_to_back_port.pop();
	}

	void back_push( std::string& msg)
	{
		back_to_front_port.push( msg);
	}

	std::string& get_login() { return login; }

	//   USER SIDE
	void front_push( std::string& msg)
	{
		front_to_back_port.push( msg);
	}

	const std::string front_pop()
	{
		return back_to_front_port.pop();
	}

	void set_login( std::string& login) { this->login = login; } 

	Client();
	void run();
	void stop();
};


/*
 *			CLASS CONNECTION
 */
class Connection : public std::enable_shared_from_this<Connection>
{
	// copy and assignment not allowed
	void operator=( const Connection&) = delete;
	Connection( const Connection&) = delete;

	Connection( Client& obj) : client( obj),
							   sock( obj.service) {}


	Client& client;
	ip::tcp::socket sock;
	char read_buffer[MAX_MSG];
	char write_buffer[MAX_MSG];

	std::string message_to;
	std::string message_from;

	typedef boost::system::error_code error_code;
	typedef std::shared_ptr<Connection> shrd_ptr;

	void start( ip::tcp::endpoint ep)
	{
		std::cerr << "Here1" << std::endl;
		sock.async_connect( ep, BIND_2( on_connect, _1));
	}
	
	// completions
	size_t read_complete( const error_code& err, size_t bytes);
	void write_to_port_complete();
	void read_complete_from_port();

public:
	static shrd_ptr start( ip::tcp::endpoint ep, Client& client) {
		shrd_ptr new_( new Connection( client));
		new_->start( ep);
		return new_;
	}

	void stop() {
		sock.close();
	}

private:
	/********functions - handlers***********/
	void on_connect( const error_code& err);

	// authentication
	void send_login();
	void check_login( const error_code&, size_t bytes);

	// send stanzas to server
	void read_from_port();
	void on_read_from_port();
	void write_to_server();
	void on_write_to_server( const error_code& err, size_t bytes);

	// read stanzas form server
	void read_from_server();
	void on_read_from_server( const error_code& err, size_t bytes);
	void write_to_port();
	void on_write_to_port();
};