#pragma once
#include <string>

class SharedObject {
public:
	SharedObject();
	SharedObject(int id, std::string name);
	void setName(std::string name);
private:
	int m_id;
	std::string m_name;
};
