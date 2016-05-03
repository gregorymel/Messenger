#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include "thread_safe_queue.hpp"
#include "Stanza.hpp"
#include "port.hpp"
#include <iostream>
#include <future>

#define MAX_MSG 2048

using namespace boost::asio;
using namespace std::placeholders;

#define BIND_1( x) std::bind( &Connection::x, shared_from_this())
#define BIND_2( x, y) std::bind( &Connection::x, shared_from_this(), y)
#define BIND_3( x, y, z) std::bind( &Connection::x, shared_from_this(), y, z)
#define BIND_4( x, y, z, a) std::bind( &Connection::x, shared_from_this(), y, z, a)

class Connection;	

class Client
{
	ip::tcp::endpoint ep;
	//std::shared_ptr<Connection> connection;

	io_service service;
public:
	// ports for user interface
	Port* port;

	io_service& get_io_service() { return service; }

	std::shared_ptr<io_service::work> work;

	Client( Port* _port);
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
							   sock( obj.get_io_service()) {};


	Client& client;

	ip::tcp::socket sock;
	char read_buffer[MAX_MSG];
	char write_buffer[MAX_MSG];
	char request_buffer[MAX_MSG];

	uint64_t xml_length;
	std::string xml_str_to;
	std::string xml_str_from;
	uint64_t size;

	typedef boost::system::error_code error_code;
	typedef std::shared_ptr<Connection> shrd_ptr;

	void start( ip::tcp::endpoint ep)
	{
		std::cerr << "Here1" << std::endl;
		sock.async_connect( ep, BIND_2( on_connect, _1));
	}
	
	// completions
	size_t read_complete( const error_code& err, size_t bytes);
	void write_to_port_complete( Stanza st);
	void read_complete_from_port();
	size_t start_read_complete( const error_code& err, size_t bytes);

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

	#if 0
	// authentication
	void send_login();
	void check_login( const error_code&, size_t bytes);
	#endif

	// send stanzas to server
	void read_from_port();
	void on_read_from_port();
	void write_to_server();
	void on_write_to_server( const error_code& err, size_t bytes);

	// read stanzas form server
	void start_read_from_server();
	void on_start_read( const error_code& err, size_t bytes);
	void read_xml_length();
	void on_read_xml_length( const error_code& err, size_t bytes);
	void read_from_server();
	void on_read_from_server( const error_code& err, size_t bytes);
	void write_to_port( Stanza st);
	void on_write_to_port();
};