/****************************** Module Header ******************************\
* Module Name:	CppNamedPipeClient.cpp
* Project:		CppNamedPipeClient
* Copyright (c) Microsoft Corporation.
*
* Named pipe is a mechanism for one-way or bi-directional inter-process
* communication between the pipe server and one or more pipe clients in the
* local machine or across the computers in the intranet:
*
* PIPE_ACCESS_INBOUND:
* Client (GENERIC_WRITE) ---> Server (GENERIC_READ)
*
* PIPE_ACCESS_OUTBOUND:
* Client (GENERIC_READ) <--- Server (GENERIC_WRITE)
*
* PIPE_ACCESS_DUPLEX:
* Client (GENERIC_READ or GENERIC_WRITE, or both) <-->
* Server (GENERIC_READ and GENERIC_WRITE)
*
* This sample demonstrates a named pipe client that attempts to connect to
* the pipe server, \\.\pipe\HelloWorld, with the GENERIC_READ and
* GENERIC_WRITE permissions. The client writes a message to the pipe server
* and receive its response.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 1/11/2009 11:20 PM Jialiang Ge Created
\***************************************************************************/

#pragma once
#pragma region Includes
#include "stdafx.h"

#include <windows.h>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#pragma endregion

#define BUFFER_SIZE	256 // 1 / 4K 

class Pipe
{
private:
	HANDLE hPipe;
	LPTSTR  strPipeName;

public:

	Pipe()
	{
		// Prepare the pipe name
		strPipeName = (LPTSTR)TEXT("\\\\.\\pipe\\orbitPipe"); // need to change
	}

	bool connect()
	{
		DWORD dwMode = PIPE_READMODE_MESSAGE; // Set pipe to message mode

		hPipe = CreateFile(
			strPipeName,            // Pipe name 
			GENERIC_READ |          // Read and write access 
			GENERIC_WRITE,
			0,                      // No sharing 
			NULL,                   // Default security attributes
			OPEN_EXISTING,          // Opens existing pipe 
			0,                      // Default attributes 
			NULL);                  // No template file 

		// Break if the pipe handle is valid.
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_PIPE_BUSY)
			{
				// All pipe instances are busy, so wait for 5 seconds
				if (!WaitNamedPipe(strPipeName, 5000))
				{
					_tprintf(_T("WaitNamedPipe failed w/err 0x%08lx\n"), GetLastError());
					return false;
				}
			}
			else
			{
				_tprintf(_T("Unable to open named pipe %s w/err 0x%08lx\n"),
					strPipeName, GetLastError());
				return false;
			}
		}

		// Set the pipe to message mode
		if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL))
		{
			_tprintf(_T("SetNamedPipeHandleState failed w/err 0x%08lx\n"), GetLastError());
			CloseHandle(hPipe);
			return false;
		}

		_tprintf(_T("The named pipe, %s, is connected in message mode.\n"), strPipeName);
		return true;
	}

	std::string toUTF8(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string str(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
		return str;
	}

	bool sendMessageToGraphics(const char* msg)
	{
		// Convert the char* (ASCII/UTF-8) to a wide string (std::wstring)
		int len = MultiByteToWideChar(CP_UTF8, 0, msg, -1, NULL, 0);
		if (len == 0)
		{
			_tprintf(_T("Failed to convert char* to wstring. Error code: %lu\n"), GetLastError());
			return false;
		}

		std::wstring wide_msg(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, msg, -1, &wide_msg[0], len);

		// Convert the wstring to a char* (for writing to the pipe)
		const wchar_t* wide_char_msg = wide_msg.c_str();
		
		// Calculate the bytes to write (including null-terminator)
		DWORD cbBytesWritten;
		DWORD cbRequestBytes = (wide_msg.size() + 1) * sizeof(wchar_t); // Size in bytes

		// Write the message to the pipe
		BOOL bResult = WriteFile(
			hPipe,                      // Handle of the pipe
			wide_char_msg,               // Message to be written (wide-char version)
			cbRequestBytes,              // Number of bytes to write
			&cbBytesWritten,             // Number of bytes written
			NULL);                       // Not overlapped

		if (!bResult || cbRequestBytes != cbBytesWritten)
		{
			_tprintf(_T("WriteFile failed w/err 0x%08lx\n"), GetLastError());
			return false;
		}

		_tprintf(_T("Sent %ld bytes: %s\n"), cbBytesWritten, msg);
		return true;
	}

	std::string getMessageFromGraphics()
	{
		DWORD cbBytesRead;
		DWORD cbReplyBytes;
		TCHAR chReply[BUFFER_SIZE];		// Server -> Client

		cbReplyBytes = sizeof(TCHAR) * BUFFER_SIZE;
		BOOL bResult = ReadFile(			// Read from the pipe.
			hPipe,					// Handle of the pipe
			chReply,				// Buffer to receive the reply
			cbReplyBytes,			// Size of buffer 
			&cbBytesRead,			// Number of bytes read 
			NULL);					// Not overlapped 

		if (!bResult && GetLastError() != ERROR_MORE_DATA)
		{
			_tprintf(_T("ReadFile failed w/err 0x%08lx\n"), GetLastError());
			return "";
		}

		_tprintf(_T("Receives %ld bytes\n"),
			cbBytesRead, chReply);
		const char* buffer = (char*)chReply;
		std::string s = buffer;
		return s;

	}

	void close()
	{
		CloseHandle(hPipe);
	}


};
