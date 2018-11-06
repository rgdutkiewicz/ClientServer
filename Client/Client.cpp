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
	HANDLE pipe = NULL;
	OVERLAPPED overlap;
	DWORD  cbRead, cbWritten, dwMode;

public:
	Client() {
		int attempts = 0;
		while (pipe == NULL && attempts < 10) {
			pipe = create_pipe_handle();
			// Retry every 5 seconds
			if (pipe == NULL) {
				WaitNamedPipe("\\\\.\\pipe\\server_pipe", 5000);
			}
			attempts++;
		}
	}

	// Method to create the pipe in either OVERLAPPED mode for asynchronous, or Normal mode for Synchronous
	HANDLE create_pipe_handle() {
		HANDLE pipe;
		pipe = CreateFile(
			"\\\\.\\pipe\\server_pipe",
			GENERIC_READ | GENERIC_WRITE, // bidirectional access
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED, // ASYNC operation
			NULL
			);
		// Break if the pipe handle is valid.
		if (pipe != INVALID_HANDLE_VALUE)
		{
			// Initialize the OVERLAPPED structure if we succeeded in opening a pipe
			memset(&overlap, 0, sizeof(overlap));
			overlap.Offset = 4096;
			overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			return pipe;
		}
		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return NULL;
		}
		return NULL;
	}

	// Synchronous I/O, wait for a confirmation response
	BOOL sendMessageSync(char message[], string& returnMessage) {
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
		// Expect a return message upon completion of IO
		returnMessage = readMessage();
		return fSuccess;
	}

	// Used for asynchronous IO, do not expect a return message
	BOOL sendMessageAsync(char message[]) {
		DWORD cbToWrite;
		//Remove "ASYNC " from our message
		char actualMessage[BUFFER_SIZE];
		strcpy(actualMessage, &(message[6]));
		cbToWrite = (lstrlen(actualMessage) + 1)*sizeof(char);
		_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, actualMessage);

		BOOL fSuccess = WriteFile(
			pipe,                   // pipe handle 
			actualMessage,			    // message 
			cbToWrite,              // message length 
			&cbWritten,             // bytes written 
			&overlap);              // overlapped
		if (!fSuccess)
		{
			DWORD Error = GetLastError();
			_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
			return fSuccess;
		}
		cout << "Message sending asynchronously." << endl;
		return fSuccess;
	}

	bool checkResult() {
		return GetOverlappedResult(pipe, &overlap, &cbWritten, false);
	}

	string readMessage() {
		DWORD numRead = 1;
		char buff[BUFFER_SIZE];
		bool success = ReadFile(pipe, &buff, BUFFER_SIZE, &numRead, NULL);
		if (!success)
		{
			_tprintf(TEXT("ReadFile failed. GLE=%d\n"), GetLastError());
		}
		return buff;
	}

	// Registers an object to the server
	bool registerObject(SharedObject object) {

		return false;
	}

	bool isPipeOpen() {
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
	Client client;
	
	if (!client.isPipeOpen())
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	bool awaitingResult = false;
	while (1) {
		char message[BUFFER_SIZE] = "";
		string buffer;
		string returnMessage;
		cout << "Enter a message for the server ('Close' to close connection)" << endl;
		cin.getline (message, BUFFER_SIZE);
		if ( ((string)message).substr(0,5) == "Close" ) {
			client.closeConnection();
			return 0;
		}
		else if (((string)message).substr(0, 4) == "HELP") {
			cout <<
				"SOBJ <id> <name> - register a new object" << endl <<
				"RENAME <id> <newname> - change the name of object at given id" << endl <<
				"GET <id> - retrieve stored object" << endl <<
				"Close - close connection" << endl;
		}
		else if (((string)message).substr(0, 5) == "ASYNC") {
			if (awaitingResult == true) {
				cout << "ASYNC I/O already in progress, unable to send another message." << endl;
			}
			else
			{
				awaitingResult = true;
				bool messageSuccess = client.sendMessageAsync(message);
			}
		}
		else
		{
			bool messageSuccess = client.sendMessageSync(message, returnMessage);
			if (!messageSuccess) {
				cout << "Message not sent." << endl;
			}
			cout << returnMessage << endl;
		}

		if (awaitingResult && client.checkResult()) {
			cout << client.readMessage() << endl;
			awaitingResult = false;
		}
		
		/*SharedObject sobj("SOBJ 12 Ross");
		strcpy_s(message, sobj.Serialize().c_str());
		bool messageSuccess = client.sendMessage(message, returnMessage);
		if (!messageSuccess) {
			cout << "Message not sent." << endl;
		}
		// Retrieve object, then wait for a response.
		strcpy_s(message, "GET 12");
		messageSuccess = client.sendMessage(message);
		if (!messageSuccess) {
			cout << "Message not sent." << endl;
		}
		string received = client.readMessage();
		cout << received << endl;*/


	}
    return 0;
}