/*
 *	Copyright (C) 2011 by Andreas Bank, andreas.mikael.bank@gmail.com
 *	NOTE: I refer to 'pointers to pointers' as arrays, don't get confused!
 */

#include "VapixAttribute.h"

class VapixFunction {
	char* name; //name of the function
	char* description; //description of the function
	char* functionString; //the code for the url
	VapixAttribute** validAttributes; //the valid attributes for this function
	int validAttributesSize; //the size of the attributes array
	VapixAttribute** attributes; //the chosen attributes
	int attributesSize; //the size of the chosen attributes array
	int opener; //what action to take when activating the function [0=send, 1=browser, 2=player]
	char ***validProtocols; //the valid protocols (list) for this function (pointer to arrays of char[2] = {name, port})
	int validProtocolsSize; //the size of the validProtocols array
	char **protocol; //the selected protocol (pointer to a char[2] = {name, port})
public:
	VapixFunction();
	VapixFunction(char* name, char* description, char* functionString);
	void setName(char* name);
	char *getName();
	void setDescription(char* description);
	char *getDescription();
	void setFunctionString(char* functionDescription);
	char *getFunctionString();
	int isValidAttribute(VapixAttribute* attribute);
	void addValidAttribute(VapixAttribute* attribute);
	BOOL addAttribute(VapixAttribute* attribute);
	void removeAttribute(VapixAttribute* attribute);
	VapixAttribute** getAttributes(int* aSize);
	VapixAttribute** getValidAttributes(int* vaSize);
	VapixAttribute* getValidAttribute(int index);
	VapixAttribute* getAttribute(int index);
	char* getString();
	void setOpener(int opener);
	int getOpener();
	BOOL validAttributeExists(VapixAttribute* attribute);
	BOOL attributeExists(VapixAttribute* attribute);
	void removeAllAttributes();
	void addValidProtocol(char** protocol);
	void addValidProtocol(char* name, char* port);
	char ***getValidProtocols(int* vpSize);
	char **getValidProtocol(int index);
	char **getProtocol();
	int getProtocolIndex();
	BOOL setProtocol(int index);
	BOOL isValidProtocol(int index);
	int getValidProtocolsSize();
};