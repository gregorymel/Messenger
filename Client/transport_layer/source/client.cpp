#include "client.hpp"
#include <thread>
#include <iostream>
#include <cstdio>
#include <string.h>

#define LENGTH_BYTES sizeof( uint64_t)

Client::Client(Port* _port) : port(_port)
{
	ep = boost::asio::ip::tcp::endpoint( ip::address::from_string( "192.168.0.106"), 8001);
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
	std::cerr << "Here2" << std::endl;

	if ( !err)
	{
		read_from_port();
		start_read_from_server();
	}
	else
	{
		stop();
		std::cerr << err << std::endl;
	}
}

#if 0 
void Connection::send_login()
{
	std::cerr << "Here3" << std::endl;
	std::string login = client.get_login();
	login += '\n';
	//std::cerr << login << std::endl;
	uint64_t size = static_cast<uint64_t>(login.size());
	std::copy( &size, &size + 8, write_buffer);
	std::copy( login.begin(), login.end(), write_buffer + 8);
	sock.async_write_some( buffer( write_buffer, login.size() + 8),
					       BIND_3( check_login, _1, _2));
		//std::bind( &Connection::check_login, shared_from_this(), _1, _2));
}

void Connection::check_login( const error_code& err, size_t bytes) {
	std::cerr << "Here4" << std::endl;
	read_from_port();
	read_from_server();	
}
#endif

/*********************WRITE BRANCH********************************/

void Connection::read_complete_from_port()
{
	std::cerr << "Read_compl" << std::endl;
	client.port->recieve_from_user().save( xml_str_to);
	client.get_io_service().post( BIND_1( on_read_from_port));
}

void Connection::read_from_port()
{
	std::cerr << "To5" << std::endl;
	

	std::thread th( &Connection::read_complete_from_port, this);
	th.detach();
	//std::copy( msg.begin(), msg.end(), write_buffer);
}

void Connection::on_read_from_port()
{
	std::cerr << "ON_READ_FROM_PORT : " << xml_str_to << std::endl;
	//std::cerr << "SIZE1 == " << xml_str_to.size() << std::endl;
	write_to_server();
}

void Connection::write_to_server()
{
	uint64_t size = static_cast<uint64_t>(xml_str_to.size());
	std::copy( &size, &size + 1, write_buffer);
	std::cerr << "WRITE_TO_SERVER : " << size << " " << xml_str_to << std::endl;
	#if 0
	std::cout << sizeof (uint64_t) << std::endl;

	for ( int i = 0; i < 8; i++)
		printf("%d", write_buffer[i]);
		std::cout << std::endl;

	std::copy( write_buffer, write_buffer + 8, &size1);
	std::cerr << "SIZE == " << static_cast<uint64_t>(size1) << std::endl;
	#endif

	std::copy( xml_str_to.begin(), xml_str_to.end(), write_buffer + LENGTH_BYTES);

	//std::cerr << write_buffer << std::endl;

	sock.async_write_some( buffer( write_buffer, xml_str_to.size() + LENGTH_BYTES),
						   BIND_3( on_write_to_server, _1, _2));
}

void Connection::on_write_to_server( const error_code& err, size_t bytes)
{
	std::cerr << "ON_WRITE_TO_SERVER..." << std::endl;
	if ( !err)
		read_from_port();
	else 
		stop();
}

/*************************READ BRANCH*************************************/

void Connection::start_read_from_server()
{
	Stanza get_request;
	get_request.setStanzaType( Stanza::IQ);
	get_request.setSubType( Stanza::GET);

	std::string xml_request;
	get_request.save( xml_request);
	size = static_cast<uint64_t>(xml_request.size());
	std::cerr << "START READ: " << size << " " << xml_request << std::endl;

	std::copy( &size, &size + 1, request_buffer);
	//memcpy( read_buffer, &size, LENGTH_BYTES);
	std::copy( xml_request.begin(), xml_request.end(), request_buffer + LENGTH_BYTES);

	sock.async_write_some( buffer( request_buffer, size + LENGTH_BYTES),
						   BIND_3( on_start_read, _1, _2));

	#if 0
	async_write( sock,
				 buffer( read_buffer, size + LENGTH_BYTES),
				 BIND_3( start_read_complete, _1, _2),
				 BIND_3( on_start_read, _1, _2));
	#endif
}

size_t Connection::start_read_complete( const error_code& err, size_t bytes)
{
	std::cerr << "BYTES WRITE == " << bytes << std::endl;
	return (bytes == size + LENGTH_BYTES) ? 0 : 1;
}

void Connection::on_start_read( const error_code& err, size_t bytes)
{
	std::cerr << "ON START READ..." << std::endl;

	if ( !err)
	{
		read_xml_length();
	} else
	{
		stop();
	}
}

void Connection::read_xml_length()
{
	std::cerr << "READ_LENGTH..." << std::endl;
	async_read( sock, 
				buffer( read_buffer, LENGTH_BYTES),
				BIND_3( on_read_xml_length, _1, _2));
}

void Connection::on_read_xml_length( const error_code& err, size_t bytes)
{
	std::cerr << "ON_READ_LENGTH..." <<  bytes << std::endl;
	if ( !err)
	{
		memcpy(&xml_length, read_buffer, LENGTH_BYTES);
		//std::copy( read_buffer, read_buffer + LENGTH_BYTES, &xml_length);
		std::cerr << "READ_XML_SIZE " << xml_length << std::endl;
		read_from_server();
	} else
	{
		std::cerr << "error: " << err << std::endl;
		stop();
	}
}

#if 0
size_t Connection::read_complete( const error_code& err, size_t bytes)
{
	if ( err)
		return 0;

	std::cerr << "READ_COMPLETE == " << std::string( read_buffer, bytes) 
									 << std::endl;

	if ( bytes > )


	bool found = std::find( read_buffer, read_buffer + bytes, '\n')
				 < read_buffer + bytes;

	return found ? 0 : 1;
}
#endif

void Connection::read_from_server()
{

	std::cerr << "READ_FROM_SERVER..." << std::endl;
	async_read( sock, 
				buffer( read_buffer, xml_length),
				BIND_3( on_read_from_server, _1, _2));
	//std::cerr << read_buffer << std::endl;
}

void Connection::on_read_from_server( const error_code& err, size_t bytes) 
{
	if ( !err) 
	{
		xml_str_from = std::string( read_buffer, bytes);
		std::cerr << "ON_READ_FROM_SERVER : "<< xml_str_from << std::endl;
		//check stanza, if StanzaType == event 
		Stanza st;
		st.load( xml_str_from);

		if ( st.getStanzaType() == Stanza::EVENT 
			 && st.getSubType() == Stanza::END)
			start_read_from_server();
		else
			write_to_port( st);
	}
	else
	{
		std::cerr << "error: " << err << std::endl;
		stop();
	}
}	

void Connection::write_to_port_complete( Stanza st)
{
	//std::cerr << "From2.1 to_port == " << xml_str_from << std::endl;
	std::cerr << "FUTEX_WRITE\n";
	client.port->send_to_user( st);
	client.get_io_service().post( BIND_1( on_write_to_port));
}

void Connection::write_to_port( Stanza st)
{
	std::cerr << "WRITE_TO_PORT..." << std::endl;
	std::thread th( BIND_2( write_to_port_complete, st));
	th.detach();
	//on_write_to_port();	
}

void Connection::on_write_to_port()
{
	std::cerr << "ON_WRITE_TO_PORT..." << std::endl;
	read_xml_length();
}