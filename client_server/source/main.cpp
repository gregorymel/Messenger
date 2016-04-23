#include "client.hpp"
#include <thread>
#include <iostream>

int main()
{
	Client client;
	std::string login( "login:Grisha");
	std::string msg( "Hello, world!");
	client.set_login( login);

	std::thread th( &Client::run, &client);
	//client.run();
	client.front_push( msg);
	client.front_push( msg );
	//std::cerr << client.front_pop() << std::endl;
	th.join();
}