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
            MAX_STANZA_TYPE
        }
        StanzaType type;
    public:
        Stanza( JID from, JID to );
        Stanza( const Stanza &obj );
        ~Stanza();
        void setLang( std::string language );
        stanzaType getStanzaType();
        JID from();
        JID to();
};

class MessageStanza : public Stanza
{
    private:
        std::string body;
        enum MsgType
        {
            CHAT = 0,
            PRIVATE,
            MAX_MSG_TYPE
        }
        MsgType type;
    public:
        MessageStanza( std::string body );
        MessageStanza( const MessageStanza &obj );
        ~MessageStanza();
        void setType( msgType type );
        msgType getStanzaType();
};

class PresenceStanza : public Stanza
{
    private:
        enum Status
        {
            AVAILABLE = 0,
            UNAVAILABLE,
            MAX_STATUS
        }
        Status status;
    public:
        PresenceStanza( Status status = AVAILABLE );
        PresenceStanza( const PresenceStanza &obj );
        ~PresenceStanza();
        Status getStanzaType();
};

class IQStanza : public Stanza
{
    protected:
        enum IQType
        {
            GET = 0,
            SET,
            RESULT,
            ERROR,
            MAX_IQType
        }
        IQType type;
    public:
        IQType getIQType();
};

class Roster : public IQStanza
{
    private:
        std::list<JID> availabilityList;
    public:
        std::list<JID>& getAvailable();
        void addAvailable( JID newJID );
};

#endif //STANZA_HPP
