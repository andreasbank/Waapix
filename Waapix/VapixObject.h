/*
 *	Copyright (C) 2011 ny Andreas Bank, andreas.mikael.bank@gmail.com
 *	Copyright (C) 2011 by Andreas Bank, andreas.mikael.bank@gmail.com
 *	VAPIX - Copyright (C) by Axis Communications AB
 */
#include "VapixFunction.h"

#define VAPIX_VERSION			"3.0" //specifies which VAPIX version this model uses/is compatible with
#define VAPIX_FUNCTIONS_COUNT	3
class Vapix {
	//bla bla
	static const char* version;
	VapixFunction** functions;
	int functionsSize;
	char* hostname;
	int protocolNumber, port;
public:
	Vapix();
	~Vapix();
	void releaseFunctions(); //free() & delete all allocated VapixFunction objects
	void getFunctions(char* theArray[]);
	VapixFunction* getFunction(int functionNumber);
	char* getFunctionCode(int functionNumber);
	char* makeFunctionString(int functionNumber, BOOL browser);
	char* getFunctionString(int functionNumber);
	char* getBrowserFunctionString(int functionNumber);
	void setHostname(char* hostname);
	void setPort(int functionNumber, int port);
	void setProtocol(int functionNumber, int protocol);
	void setProtocol(int functionNumber, int protocol, int port);
	void getProtocols(char* theArray[]);
	void getProtocols(int functionNumber, int* vpSize);
	char* getProtocol(int protocolNumber);
	int getProtocolPort(int protocolNumber);
	const char* getVersion();
};