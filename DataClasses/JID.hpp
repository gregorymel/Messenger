#ifndef JID_HPP
#define JID_HPP

//Standart C++ libraries
#include <string>

class JID
{
    private:
        std::string node;
        std::string domain;
        std::string resource;
    public:
        JID( std::string raw );
        JID ( const JID &obj );
        ~JID();
};

#endif //JID_HPP
