// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <string>
#include "SharedObject.h"

using namespace std;
#define BUFFER_SIZE 512

class Client {
private:
	HANDLE pipe;
	DWORD  cbRead, cbWritten, dwMode;

public:
	Client() {
		pipe = create_pipe_handle("sync");
	}

	Client(string mode) {
		pipe = create_pipe_handle(mode);
	}

	// Method to create the pipe in either OVERLAPPED mode for asynchronous, or Normal mode for Synchronous
	HANDLE create_pipe_handle(string sync_attr) {
		HANDLE pipe;
		if (sync_attr == "async") {
			pipe = CreateFile(
				"\\\\.\\pipe\\server_pipe",
				GENERIC_READ | GENERIC_WRITE, // bidirectional access
				0,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED, // ASYNC operation
				NULL
				);
		}
		else if (sync_attr == "sync")
		{
			pipe = CreateFile(
				"\\\\.\\pipe\\server_pipe",
				GENERIC_READ | GENERIC_WRITE, // bidirectional access
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, // Normal, synchronous operation
				NULL
				);
		}
		// Break if the pipe handle is valid.
		if (pipe != INVALID_HANDLE_VALUE)
		{
			return pipe;
		}
		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return NULL;
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if (!WaitNamedPipe("\\\\.\\pipe\\server_pipe", 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return NULL;
		}
		return NULL;
	}

	bool sendMessage(char message[]) {
		DWORD cbToWrite;
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

	bool isOpen() {
		if (pipe == NULL) {
			return false;
		}
		else
		{
			return true;
		}
	}

	// Close the connection
	void closeConnection() {
		// Close our pipe handle
		CloseHandle(pipe);
		return;
	}
};

int main()
{	
	cout << "Synchronous or Asynchronous? (sync, async)" << endl;
	string sync_attr;
	cin >> sync_attr;
	while (!(sync_attr == "sync" || sync_attr == "async") )
	{
		cout << "invalid connection type, valid conections are sync and async" << endl;
		cin >> sync_attr;
	}
	cout << "Connecting to pipe..." << endl;

	Client client(sync_attr);
	
	if (!client.isOpen())
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	while (1) {
		char message[BUFFER_SIZE];
		string buffer;
		cout << "Enter a message for the server" << endl;
		cin >> buffer;
		if (buffer == "Close") {
			client.closeConnection();
			return 0;
		}
		else if (buffer == "SharedObject") {
			// Create a shared object on the server by passing the appropriate values as messages to the server... Or do we handle this exclusively on the server?
			// Should we use specific commands to create?  I.e. create... call etc.? That could be reasonable...
		}
		else
		{
			strcpy_s(message, buffer.c_str());
			bool messageSuccess = client.sendMessage(message);
			if (!messageSuccess) {
				cout << "Message not sent." << endl;
			}
		}
	}
    return 0;
}

