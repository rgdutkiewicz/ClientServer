#pragma once
#include <string>

class SharedObject {
public:
	SharedObject();
	SharedObject(int id, std::string name);
	void setName(std::string name);
	void setId(int id);
	void Serialize(char& buffer);
	int getId();
	std::string getName();
private:
	int m_id;
	std::string m_name;
};
