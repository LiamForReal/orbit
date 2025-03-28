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
    Pipe(string nameEnding)
    {
        string pipeName = "\\\\.\\pipe\\orbitPipe_" + nameEnding;
        std::cout << "pipe name is: " << pipeName << "\n";
        std::wstring widePipeName(pipeName.begin(), pipeName.end());
        strPipeName = (LPTSTR)widePipeName.c_str();
    }

    bool connect()
    {
        DWORD dwMode = PIPE_READMODE_MESSAGE;  // Set pipe to message mode

        hPipe = CreateFile(
            strPipeName,            // Pipe name
            GENERIC_READ |          // Read and write access
            GENERIC_WRITE,
            0,                      // No sharing
            NULL,                   // Default security attributes
            OPEN_EXISTING,          // Opens existing pipe
            0,                      // Default attributes
            NULL);                  // No template file

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_PIPE_BUSY)
            {
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

        if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL))
        {
            _tprintf(_T("SetNamedPipeHandleState failed w/err 0x%08lx\n"), GetLastError());
            CloseHandle(hPipe);
            return false;
        }

        _tprintf(_T("The named pipe, %s, is connected in message mode.\n"), strPipeName);
        return true;
    }

    bool sendMessageToGraphics(char* msg, int bytesLength)
    {
        //char ea[] = "SSS";
        char* chRequest = msg;	// Client -> Server
        DWORD cbBytesWritten, cbRequestBytes;

        // Send one message to the pipe.
        cbRequestBytes = bytesLength;

        BOOL bResult = WriteFile(			// Write to the pipe.
            hPipe,						// Handle of the pipe
            chRequest,					// Message to be written
            cbRequestBytes,				// Number of bytes to write
            &cbBytesWritten,			// Number of bytes written
            NULL);						// Not overlapped 

        if (!bResult/*Failed*/ || cbRequestBytes != cbBytesWritten/*Failed*/)
        {
            _tprintf(_T("WriteFile failed w/err 0x%08lx\n"), GetLastError());
            return false;
        }

        _tprintf(_T("Sends %ld bytes; Message: \"%s\"\n"),
            cbBytesWritten, chRequest);

        return true;

    }

    std::string getMessageFromGraphics()
    {
        DWORD cbBytesRead;
        DWORD cbReplyBytes;
        TCHAR chReply[BUFFER_SIZE];		// Server -> Client

        cbReplyBytes = sizeof(TCHAR) * BUFFER_SIZE;
        BOOL bResult = ReadFile(			// Read from the pipe.
            hPipe,					// Pipe handle
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
            cbBytesRead);
        const char* buffer = (char*)chReply;
        std::string s = buffer;
        return s;
    }

    void close()
    {
        CloseHandle(hPipe);
    }
};

