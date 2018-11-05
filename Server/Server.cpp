// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "SharedObject.h"
#include <map>

using namespace std;
#define BUFFER_SIZE 512

enum STATE {
	NONE = 0,
	REGISTER_ID = 1,
	REGISTER_NAME   = 2,
	CALL_ID = 3,
	CALL_NAME   = 4
};

// Effectively just a wrapper for a map
class Storage {
public:
	Storage() {} // Empty default constructor, nothing to do
	SharedObject retrieveObject(int id) {
		return storageMap.at(id);
	}
	void addObject(SharedObject obj) {
		storageMap.emplace(obj.getId(), obj);
	}
private:
	map<int, SharedObject> storageMap;
};

bool sendMessage(HANDLE pipe, char message[]) {
	DWORD cbToWrite;
	DWORD cbWritten;
	cbToWrite = (lstrlen(message) + 1)*sizeof(char);
	_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, message);

	BOOL fSuccess = WriteFile(
		pipe,                   // pipe handle 
		message,			    // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		DWORD Error = GetLastError();
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return fSuccess;
	}
	cout << "Message sent" << endl;
	return fSuccess;
}

int main()
{
	STATE objectState = NONE;
	std::cout << "Creating an instance of a named pipe..." << endl;

	// Create a pipe to send data
	HANDLE outbound_pipe = CreateNamedPipe(
		"\\\\.\\pipe\\server_pipe", // name of the pipe
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // 2-way pipe, read and write
		PIPE_TYPE_MESSAGE, // Pipe is message-type
		1, // only allow 1 instance of this pipe
		0, // no outbound buffer
		0, // no inbound buffer
		0, // use default wait time
		NULL // use default security attributes
		);

	if (outbound_pipe == NULL || outbound_pipe == INVALID_HANDLE_VALUE) {
		wcout << "Failed to create outbound pipe instance.";
		system("pause");
		return 1;
	}

	wcout << "Waiting for a client to connect to the pipe..." << endl;

	// This call blocks until a client process connects to the pipe
	BOOL result = ConnectNamedPipe(outbound_pipe, NULL);
	if (!result) {
		wcout << "Failed to make connection on named pipe." << endl;
		CloseHandle(outbound_pipe); // close the pipe
		system("pause");
		return 1;
	}

	// Wait for a message from the client
	char buff[BUFFER_SIZE];
	DWORD numRead = 1;
	Storage objectStorage;
	while (1) {
		bool success = ReadFile(outbound_pipe, &buff, BUFFER_SIZE, &numRead, NULL);
		if (success) {
			string decode_string = buff;
			if (decode_string.substr(0,4) == "SOBJ") {
				SharedObject received(decode_string);
				objectStorage.addObject(received);
				sendMessage(outbound_pipe, TEXT("Generic Success Reply"));
			}
			else if (decode_string.substr(0, 3) == "GET") {
				std::string delimiter = " ";
				decode_string.erase(0, decode_string.find(delimiter) + delimiter.length());
				//id
				std::string id_string = decode_string.substr(0, decode_string.find(delimiter));
				int id = std::stoi(id_string);
				decode_string.erase(0, decode_string.find(delimiter) + delimiter.length());
				SharedObject temp = objectStorage.retrieveObject(id);
				char message[BUFFER_SIZE];
				strcpy_s(message, temp.Serialize().c_str());
				sendMessage(outbound_pipe, message);
			}
			else {
				printf("%s\n", buff);
				sendMessage(outbound_pipe, TEXT("Generic Success Reply"));
			}
		}
		else
		{
			_tprintf(TEXT("ReadFile failed. GLE=%d\n"), GetLastError());
			break;
		}
	}

	// Close the pipe (automatically disconnects client too)
	CloseHandle(outbound_pipe);

	cout << "Done" << endl;

    return 0;
}

