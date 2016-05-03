#include "port.hpp"
#include <iostream>
#include <thread>

Port port;

void reader_and_write()
{
	std::this_thread::sleep_for(std::chrono::seconds(10));
	std::string msg("Yo!");
	port.send_to_user(msg);
	std::cerr << port.recive_from_user() << std::endl;	
}

int main()
{
	std::string str("Hello!");
	std::thread th(&reader_and_write);
	std::cerr << port.recive_from_server() << std::endl;
	port.send_to_server(str);
	th.join();
}
