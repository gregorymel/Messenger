#include "client.hpp"
#include <thread>
#include "Stanza.hpp"
#include <iostream>
#include "port.hpp"

int main()
{
	Port port;
	Client client(&port);
	std::string login( "Grisha\n228");
	std::string msg( "Hello, world!");
	std::string xml; 

	Stanza stanza;

	stanza.setMSG( PRIVATE, msg);
	stanza.save( xml);

	client.set_login( login);

	std::thread th( &Client::run, &client);
	//client.run();
	client.front_push( xml);
	client.front_push( xml);
	//std::cerr << client.front_pop() << std::endl;
	th.join();
}
