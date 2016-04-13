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
	JID(){};        
	JID( std::string raw ){};
        JID ( const JID &obj ){};
        ~JID(){};
	void setNode(std::string str)
	{
		node = str;
	}
	void setDomain(std::string str)
	{
		domain = str;
	}
	void setResource(std::string str)
	{
		resource = str;
	}
	std::string getNode()
	{
		return node;
	}
	std::string getDomain()
	{
		return domain;
	}
	std::string getResource()
	{
		return resource;
	}
	
	JID& operator=(JID copied) 
	{
		node = copied.getNode();
		domain = copied.getDomain();
		resource = copied.getResource();
		return *this;
	}
};

#endif //JID_HPP
