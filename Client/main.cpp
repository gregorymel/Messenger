#include "client.hpp"
#include <thread>
#include <iostream>
#include "port.hpp"

int main()
{
	Port port;

	// transport layer
	Client client(&port);
	std::thread th( &Client::run, &client);
	th.detach();

	// user intrface
	QApplication app(argc, argv);
	PortController pc(&port);
    ChatterBox chatterBox(&pc);

    chatterBox.setMinimumWidth(200);
    chatterBox.show();

    return app.exec();
}