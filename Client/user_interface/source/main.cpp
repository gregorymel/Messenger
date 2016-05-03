#include <QApplication>
#include "ChatterBox.h"
#include "port.hpp"
#include "Stanza.hpp"
#include "PortController.h"
#include <iostream>
#include <thread>

Port port;

void function()
{
	while (true)
	{
		Stanza stanza = port.recieve_from_user();
		std::string xml;
		stanza.save(xml);
		std::cerr << xml << std::endl;
	}
}

void sender()
{
	std::this_thread::sleep_for(std::chrono::seconds(10));
    Stanza stanza;
    stanza.setStanzaType(Stanza::IQ);
    stanza.setSubType(Stanza::AVAILABLE);
    port.send_to_user(stanza);
    std::this_thread::sleep_for(std::chrono::seconds(20));

    #if 0
    std::string to_str("")
    JID to;
    to.setNode(to_str);
    #endif

    std::string from_str("denis");
    std::string msg("Hello!");
    JID from;
    from.setNode(from_str);
    Stanza stanza_msg(from, from);
    stanza_msg.setStanzaType(Stanza::MESSAGE);
    stanza_msg.setMSG( Stanza::PRIVATE, msg);
    port.send_to_user( stanza_msg);
    std::cerr << "Send" << std::endl;
}

int main(int argc, char** argv)
{
	std::thread th(&function);
    th.detach();

    std::thread th1(&sender);
    th1.detach();

    QApplication app(argc, argv);
	PortController pc(&port);
    ChatterBox chatterBox(&pc);

    std::string str("Gregory\n228");

    chatterBox.setMinimumWidth(200);
    chatterBox.show();

    return app.exec();
}
