#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
// Q: why do we need this class ?
// A: this class is made to make sure we init the WSA and free it at the end
// we will have a WSA initialized as long as an instance of this class exist
// once the instance's scope end the d-tor will be called and the WSA will be free
// alternatively we could use scope guard. 
// (if you are not familiar with scope guard please read about it.) 
class WSAInitializer
{
public:
	WSAInitializer();
	~WSAInitializer();
};
