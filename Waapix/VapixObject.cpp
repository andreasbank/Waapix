#include "stdafx.h"
#include "VapixObject.h"

const char* Vapix::version = VAPIX_VERSION;

Vapix::Vapix() {
	hostname = (char*)malloc(sizeof(char)*200);
	functionsSize = 0;

	//fill object with VAPIX 3.0 stuff
	// TODO: make it load of a resource file instead? one that can be patched

#ifdef _DEBUG_MODE_
	// DONT FORGET TO CHANGE *VAPIX_FUNCTION_COUNT*
	functions = (VapixFunction**)malloc(sizeof(VapixFunction*)*VAPIX_FUNCTIONS_COUNT);
	functions[0] = new VapixFunction();
	functions[0]->setName("Get a jpg snapshot");
	functions[0]->setDescription("Retrieves a jpg snapshot form the DUT");
	functions[0]->setFunctionString("axis-cgi/jpeg/image.cgi");
	functions[0]->setOpener(1);
	functions[0]->addValidProtocol("http", "80");
	functions[0]->addValidProtocol("https", "443");
	functions[0]->setProtocol(0);
	VapixAttribute* validAttribute = new VapixAttribute();
	validAttribute->setName("resolution");
	validAttribute->setDescription("The resolution of the image");
	validAttribute->addValidValue("800x600");
	validAttribute->addValidValue("1280x720");
	functions[0]->addValidAttribute(validAttribute);
	//f2
	functions[1] = new VapixFunction();
	functions[1]->setName("Set resolution");
	functions[1]->setDescription("Sets the resolution used by the DUT");
	functions[1]->setFunctionString("axis-cgi/parameter.cgi?action=update&ImageSizeOrSomething=");
	functions[1]->setOpener(0);
	functions[1]->addValidProtocol("http", "80");
	functions[1]->addValidProtocol("https", "443");
	functions[1]->addValidProtocol("rtsp", "554");
	functions[1]->setProtocol(0);
	validAttribute = new VapixAttribute();
	validAttribute->setName("resolution");
	validAttribute->setDescription("The resolution of the image");
	validAttribute->addValidValue("800x600");
	validAttribute->addValidValue("1280x720");
	functions[1]->addValidAttribute(validAttribute);
	validAttribute = new VapixAttribute();
	validAttribute->setName("camera");
	validAttribute->setDescription("The camera to use");
	validAttribute->addValidValue("1");
	validAttribute->addValidValue("2");
	functions[1]->addValidAttribute(validAttribute);
	//f3
	functions[2] = new VapixFunction();
	functions[2]->setName("Open in VLC");
	functions[2]->setDescription("Opens a stream in VLC");
	functions[2]->setFunctionString("axis-media/media.amp");
	functions[2]->setOpener(2);
	functions[2]->addValidProtocol("http", "80");
	functions[2]->addValidProtocol("https", "443");
	functions[2]->addValidProtocol("rtsp", "554");
	functions[2]->setProtocol(2);
	validAttribute = new VapixAttribute();
	validAttribute->setName("resolution");
	validAttribute->setDescription("The resolution of the image");
	validAttribute->addValidValue("800x600");
	validAttribute->addValidValue("1280x720");
	functions[2]->addValidAttribute(validAttribute);
	validAttribute = new VapixAttribute();
	validAttribute->setName("camera");
	validAttribute->setDescription("The camera to use");
	validAttribute->addValidValue("1");
	validAttribute->addValidValue("2");
	functions[2]->addValidAttribute(validAttribute);
	validAttribute = new VapixAttribute();
	validAttribute->setName("videocodec");
	validAttribute->setDescription("The codec to use (H264 or MJPEG)");
	validAttribute->addValidValue("H264");
	validAttribute->addValidValue("MJPEG");
	functions[2]->addValidAttribute(validAttribute);
	delete validAttribute;
#endif
}

/**
 * Release all resources' memory
 */
Vapix::~Vapix() {
	releaseFunctions();
	free(hostname);
}

/**
 * Release the VapixFunction objects and
 * the **functions pointer's memory
 */
void Vapix::releaseFunctions() {
	//take each function
	for(int i=0; i < VAPIX_FUNCTIONS_COUNT; i++) {
		//and release its memory
		delete functions[i];
	}
	//then release the **functions pointer
	free(functions);
}

void Vapix::getFunctions(char* theArray[]) {
	for(int i=0; i<VAPIX_FUNCTIONS_COUNT; i++) {
		theArray[i] = (char*)malloc(sizeof(char)*50);
		strcpy(theArray[i], functions[i]->getName());
	}
}

VapixFunction* Vapix::getFunction(int functionNumber) {
	return functions[functionNumber];
}

/**
 * makeFunctionString() - builds the string used to send to the DUT
 */
char* Vapix::makeFunctionString(int functionNumber, BOOL browser) {
	char* temp = (char*)malloc(sizeof(char)*512);
	char* portString = (char*)malloc(sizeof(char)*7);
	temp[0] = '\0';

	/*
	 *	NOTE: the browser thing is for future improvements of the application
	 */
	//if creating browser(==TRUE) compatible string
	if	(browser) {
		//hide port when default for protocol
		if((port == 80 && strcmp(getFunction(functionNumber)->getProtocol()[0], "http") == 0) ||
				(port == 443 && strcmp(getFunction(functionNumber)->getProtocol()[0], "https") == 0) ||
				(port == 554 && strcmp(getFunction(functionNumber)->getProtocol()[0], "rtsp") == 0))
				portString[0] = '\0';
			else
				sprintf(portString, ":%d", port);
			sprintf(temp, "%s://%s%s", getFunction(functionNumber)->getProtocol()[0], hostname, portString);
	}

	//create and return string to be sent to DUT
	sprintf(temp, "%s/%s", temp, functions[functionNumber]->getString());
	return temp;
}

char* Vapix::getFunctionString(int functionNumber) {
	return makeFunctionString(functionNumber, FALSE);
}

char* Vapix::getBrowserFunctionString(int functionNumber) {
	return makeFunctionString(functionNumber, TRUE);
}

char* Vapix::getFunctionCode(int functionNumber) {
	return functions[functionNumber]->getString();
}

void Vapix::setHostname (char* hostname) {
	strcpy(this->hostname, hostname);
}

void Vapix::setPort(int functionNumber, int port) {
	if(port > 65535)
		port = 65535;
	this->port = port;
}

void Vapix::setProtocol(int functionNumber, int protocolNumber) {
	//safety check for correct protocol
	//imho the check should be done by the requesting party too keep consistency of error handling
	if(protocolNumber < 0 || protocolNumber > 2)
		MessageBox(NULL, "Erroneous protocol!\nNo protocol was set!\n\nValid protocols are '0:http', '1:https','2:rtsp'.", "Error", MB_ICONEXCLAMATION | MB_OK);
	else
		getFunction(functionNumber)->setProtocol(protocolNumber);
}

void Vapix::setProtocol(int functionNumber, int protocolNumber, int port) {
	setProtocol(functionNumber, protocolNumber);
	setPort(functionNumber, port);
}

void Vapix::getProtocols(int functionNumber, int* vpSize) {
	*vpSize = getFunction(functionNumber)->getValidProtocolsSize();
}

char* Vapix::getProtocol(int functionNumber) {
	return getFunction(functionNumber)->getProtocol()[0];
}

int Vapix::getProtocolPort(int functionNumber) {
	return atoi(getFunction(functionNumber)->getProtocol()[1]);
}

const char* Vapix::getVersion() {
	return version;
}