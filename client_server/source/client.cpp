#include "client.hpp"
#include <thread>
#include <iostream>

Client::Client()
{
	ep = boost::asio::ip::tcp::endpoint( ip::address::from_string( "192.168.0.103"), 8001);
	std::cerr << "Here0" << std::endl;
}

void Client::run()
{
	Connection::start( ep, *this);
	work = std::make_shared<io_service::work>(service);
	service.run();
}

void Client::stop()
{
	work.reset();
}

void Connection::on_connect( const error_code& err)
{
	std::cerr << client.get_login() << std::endl;
	std::cerr << "Here2" << std::endl;

	if ( !err)
	{
		send_login( );
	}
	else
	{
		stop();
		std::cerr << err << std::endl;
	}
}

void Connection::send_login()
{
	std::cerr << "Here3" << std::endl;
	std::string login = client.get_login();
	login += '\n';
	//std::cerr << login << std::endl;
	std::copy( login.begin(), login.end(), write_buffer);
	sock.async_write_some( buffer( write_buffer, login.size()),
					       BIND_3( check_login, _1, _2));
		//std::bind( &Connection::check_login, shared_from_this(), _1, _2));
}

void Connection::check_login( const error_code& err, size_t bytes) {
	std::cerr << "Here4" << std::endl;
	read_from_port();
	read_from_server();	
}

void Connection::read_complete_from_port()
{
	std::cerr << "Read_compl" << std::endl;
	message_to = client.back_pop();
	client.service.post( BIND_1( on_read_from_port));
}

void Connection::read_from_port()
{
	std::cerr << "To5" << std::endl;
	//std::thread th( BIND_1( read_complete_from_port));
	std::thread th( &Connection::read_complete_from_port, this);
	th.detach();
	//std::copy( msg.begin(), msg.end(), write_buffer);
}

void Connection::on_read_from_port()
{
	std::cerr << "To6 from_port == " << message_to << std::endl;
	write_to_server();
}

void Connection::write_to_server()
{
	std::cerr << "To7 to_server == " << message_to << std::endl;
	message_to += '\n'; 
	std::copy( message_to.begin(), message_to.end(), write_buffer);
	sock.async_write_some( buffer( write_buffer, message_to.size()),
						   BIND_3( on_write_to_server, _1, _2));
}

void Connection::on_write_to_server( const error_code& err, size_t bytes)
{
	std::cerr << "To8" << std::endl;
	if ( !err)
		read_from_port();
	else stop();
}


size_t Connection::read_complete( const error_code& err, size_t bytes)
{
	if ( err)
		return 0;

	std::cerr << "READ_COMPLETE == " << std::string( read_buffer, bytes) 
									 << std::endl;

	bool found = std::find( read_buffer, read_buffer + bytes, '\n')
				 < read_buffer + bytes;

	return found ? 0 : 1;
}

void Connection::read_from_server()
{
	std::cerr << "From1" << std::endl;
	async_read( sock, 
				buffer( read_buffer),
				BIND_3( read_complete, _1, _2),
				BIND_3( on_read_from_server, _1, _2));
	//std::cerr << read_buffer << std::endl;
}

void Connection::on_read_from_server( const error_code& err, size_t bytes) {
	if ( !err) {
		message_from = std::string( read_buffer, bytes - 1);
		std::cerr << "From2 from_server == "<< message_from << std::endl;
		write_to_port();
	}
	else
	{
		std::cerr << "error: " << err << std::endl;
		stop();
	}
}	

void Connection::write_to_port_complete()
{
	//std::cerr << "From2.1 to_port == " << message_from << std::endl; 
	client.back_push( message_from);
	client.service.post( BIND_1( on_write_to_port));
}
void Connection::write_to_port()
{
	std::cerr << "From3" << std::endl;
	std::thread th( BIND_1( write_to_port_complete));
	th.detach();
	//on_write_to_port();	
}

void Connection::on_write_to_port()
{
	std::cerr << "From4" << std::endl;
	read_from_server();
}