#include "SharedObject.h"

SharedObject::SharedObject() {
	m_id = 1;
	m_name = "Default";
}

SharedObject::SharedObject(int id, std::string name) {
	m_id = id;
	m_name = name;
}

void SharedObject::setName(std::string name) {
	m_name = name;
}

void SharedObject::setId(int id) {
	m_id = id;
}

void SharedObject::Serialize(char& buffer) {
	char SEPARATOR = '\0';
	size_t id_size = sizeof(m_id);
	size_t name_size = sizeof(m_name);
	buffer = *('SOBJ' + SEPARATOR + id_size + SEPARATOR + m_id + SEPARATOR + name_size + SEPARATOR + m_name.c_str());
}

int SharedObject::getId() {
	return m_id;
}

std::string SharedObject::getName() {
	return m_name;
}