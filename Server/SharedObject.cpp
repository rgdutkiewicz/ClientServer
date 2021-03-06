#include "SharedObject.h"

SharedObject::SharedObject() {
	m_id = 1;
	m_name = "Default";
}

SharedObject::SharedObject(int id, std::string name) {
	m_id = id;
	m_name = name;
}

SharedObject::SharedObject(std::string serialized) {
	//SOBJ
	std::string delimiter = " ";
	serialized.erase(0, serialized.find(delimiter) + delimiter.length());
	//id
	std::string id_string = serialized.substr(0, serialized.find(delimiter));
	m_id = std::stoi(id_string);
	serialized.erase(0, serialized.find(delimiter) + delimiter.length());
	//name
	m_name = serialized.substr(0, serialized.find(delimiter));
	serialized.erase(0, serialized.find(delimiter) + delimiter.length());
}

void SharedObject::setName(std::string name) {
	m_name = name;
}

void SharedObject::setId(int id) {
	m_id = id;
}

std::string SharedObject::Serialize() {
	std::string id_string = std::to_string(m_id);
	std::string name_string = m_name;
	std::string prefix = "SOBJ";
	return prefix + " " + id_string + " " + name_string;
}

int SharedObject::getId() {
	return m_id;
}

std::string SharedObject::getName() {
	return m_name;
}