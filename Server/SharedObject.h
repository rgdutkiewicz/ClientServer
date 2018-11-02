#pragma once
#include <string>

class SharedObject {
public:
	SharedObject();
	SharedObject(int id, std::string name);
	SharedObject(std::string Serialized);
	void setName(std::string name);
	void setId(int id);
	std::string Serialize();
	int getId();
	std::string getName();
private:
	int m_id;
	std::string m_name;
};
