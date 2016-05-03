#include "client.hpp"
#include <thread>
#include "Stanza.hpp"
#include <iostream>
#include "port.hpp"

Port port;

int getter()
{
	while(true)
	{
		Stanza stanza = port.recieve_from_server();
		std::string str;

		stanza.save(str);
		std::cerr << str << std::endl;
	}
}

int main()
{
	Client client(&port);
	std::string login;
	std::cin >> login;
	std::string pas( "\n228");
	login += pas;
	std::string to_str;
	std::cin >> to_str;
	std::string msg( "Hello, world!");
	//std::string xml; 

	JID to;
	to.setNode(to_str);
	Stanza stanza_log;	

	stanza_log.setStanzaType(Stanza::IQ);
	stanza_log.setMSG( Stanza::SIGNUP, login);
	port.send_to_server(stanza_log);

	stanza_log.setStanzaType(Stanza::IQ);
	stanza_log.setMSG( Stanza::SIGNIN, login);
	port.send_to_server(stanza_log);

	std::thread th( &Client::run, &client);
	std::thread th1( &getter);
	//client.run();

	std::this_thread::sleep_for(std::chrono::seconds(10));

	Stanza stanza_msg(to, to);
	stanza_msg.setStanzaType(Stanza::MESSAGE);
	stanza_msg.setMSG( Stanza::PRIVATE, msg);

	for (int i = 0; i < 10; i++)
		port.send_to_server( stanza_msg);
	//port.send_to_server( stanza_msg);
	//std::cerr << client.front_pop() << std::endl;
	th.join();
	th1.join();
}