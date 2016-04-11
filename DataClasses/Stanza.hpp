#ifndef STANZA_HPP
#define STANZA_HPP

//Standart C++ libraries
#include <string>
#include <list>
//Local headers
#include <JID.hpp>

class Stanza
{
    protected:
        JID from;
        JID to;
        enum StanzaType
        {
            MESSAGE = 0,
            PRESENCE,
            IQ,
	    ROASTER,
            MAX_STANZA_TYPE
        }
        StanzaType type;
	enum SubType
	{
	    CHAT = 0,
            PRIVATE,
            AVAILABLE,
            UNAVAILABLE,
            GET,
            SET,
            RESULT,
            ERROR,
            MAX_SUBTYPE
	}
	SubType subType;
	std::string body;
	std::list<JID> availabilityList;     
    public:
        Stanza( JID from, JID to );
        Stanza( const Stanza &obj );
        ~Stanza();
        void setLang( std::string language );
        stanzaType getStanzaType();
	SubType getSubType();
        JID from();
        JID to();
	void load(const std::string &filename);
	void save(const std::string &filename);
	std::list<JID>& getAvailable();
        void addAvailable( JID newJID );
};

#endif //STANZA_HPP
