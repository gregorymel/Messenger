#pragma once
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
    JID( const JID& obj ) :
        node( obj.getNode()),
        domain( obj.getDomain()),
        resource( obj.getResource())
    {};
    JID& operator=(const JID& copied)
	{
		node = copied.getNode();
		domain = copied.getDomain();
		resource = copied.getResource();
		return *this;
	}

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
	std::string getNode() const
	{
		return node;
	}
	std::string getDomain() const
	{
		return domain;
	}
	std::string getResource() const
	{
		return resource;
	}
};
