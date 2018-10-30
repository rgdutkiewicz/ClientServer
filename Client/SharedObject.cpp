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