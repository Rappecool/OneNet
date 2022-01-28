#pragma once
#include <string>


class IPEndPoint
{
public:
	IPEndPoint() = default;
	IPEndPoint(const std::string& aAdress, const unsigned int aPort);
	IPEndPoint(const IPEndPoint& anIpEndpoint) = default;
	~IPEndPoint() = default;

	inline const std::string& GetAdress() const { return myAddress; }
	inline const unsigned int GetPort() const { return myPort; }

	inline bool operator == (const IPEndPoint& aIPEndPoint) { return this->myAddress == aIPEndPoint.myAddress && this->myPort == aIPEndPoint.myPort; }
	inline bool operator == (const IPEndPoint& aIPEndPoint) const { return this->myAddress == aIPEndPoint.myAddress && this->myPort == aIPEndPoint.myPort; }
private:
	std::string myAddress = "";
	unsigned int myPort = 0;

};
