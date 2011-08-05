#include "stdafx.h"
#include "VapixFunction.h"


VapixFunction::VapixFunction() {
	name = (char*)malloc(sizeof(char)*200);
	description = (char*)malloc(sizeof(char)*200);
	functionString = (char*)malloc(sizeof(char)*200);
	strcpy(name, "\0");
	strcpy(description, "\0");
	strcpy(functionString, "\0");
	validAttributes = NULL;
	validAttributesSize = 0;
	attributes = NULL;
	attributesSize = 0;
	opener = 0;
	validProtocols = NULL;
	validProtocolsSize = 0;
	protocol = NULL;
}

VapixFunction::VapixFunction(char* name, char* description, char* functionString) {
	//VapixFunction(); //WFT, doesn't work?!?!?!
	this->name = (char*)malloc(sizeof(char)*200);
	this->description = (char*)malloc(sizeof(char)*200);
	this->functionString = (char*)malloc(sizeof(char)*200);
	strcpy(this->name, "\0");
	strcpy(this->description, "\0");
	strcpy(this->functionString, "\0");
	this->validAttributes = NULL;
	this->validAttributesSize = 0;
	this->attributes = NULL;
	this->attributesSize = 0;
	this->opener = 0;
	strcpy(this->name, name);
	strcpy(this->description, description);
	strcpy(this->functionString, functionString);
}

void VapixFunction::setName(char* name) {
	strcpy(this->name, name);
}

char* VapixFunction::getName() {
	return name;
}

void VapixFunction::setDescription(char* description) {
	strcpy(this->description, description);
}

char* VapixFunction::getDescription() {
	return description;
}

/**
 * sets the url string for this function
 */
void VapixFunction::setFunctionString(char* functionString) {
	strcpy(this->functionString, functionString);
}

/**
 * gets the url string for this function
 */
char* VapixFunction::getFunctionString() {
	return functionString;
}

/**
 *check if the attribute is valid (if it is in the valid array)
 */
int VapixFunction::isValidAttribute(VapixAttribute* attribute) {
	for(int i=0; i<validAttributesSize; i++) {
		if(validAttributes[i]->isEqual(attribute)) {
			return i;
		}
	}
	return -1;
}

void VapixFunction::addValidAttribute(VapixAttribute* attribute) {
	//reallocate to fit the new attribute pointer (no, not object, just the pointer, its an array remember?)
	validAttributes = (VapixAttribute**)realloc(validAttributes, (++validAttributesSize)*sizeof(validAttributes));
	//make a clone (deep copy) and point to it
	validAttributes[validAttributesSize-1] = attribute->clone();
}

/**
 * Adds an attribute to the attributes array
 * NOTE: after removing an attribute all following attributes
 * index fall back to index-1, so if attribute[5] was "camera=1"
 * after a deletion before its position it will become attribute[4] afterwards
 */
BOOL VapixFunction::addAttribute(VapixAttribute* attribute) {
	//TODO: check if it already exists???
	int index = isValidAttribute(attribute);
	//check if it is valid and use that index
	if(index>=0) {
		//reallocate to fit the new attribute pointer (no, not object, just the pointer, its an array remember?)
		attributes = (VapixAttribute**)realloc(attributes, (++attributesSize)*sizeof(attributes));
		//set the attribute, since it is valid, it already is in the validAttributes
		//so we only need to point the new pointer to it and set the new value
		attributes[attributesSize-1] = validAttributes[index];
		attributes[attributesSize-1]->setValue(attribute->getValue());
		return TRUE;
	}
	return FALSE;
}

/**
 * Removes an attribute from the attributes array
 * NOTE: after removing an attribute all following attributes
 * index fall back to index-1, so if attribute[5] was "camera=1"
 * after a deletion before its position it will become attribute[4] afterwards
 */
void VapixFunction::removeAttribute(VapixAttribute* attribute) {
	/*
	 * attributes just point to validAttributes with a set value
	 * so they dont need to be deleted, instead just setValue("\0")
	 */

	 //loop through the array and try to find the attribute
	for(int i=0; i<attributesSize; i++) {
		if(attribute->isEqual(attributes[i])) {
			//set value back to nothing ('\0')
			attributes[i]->setValue("\0");
			//loop and change all to pointers to point to i+1
			//this will move the gap to the end
			for(int j=0; j<(attributesSize-1); j++) {
				attributes[j] = attributes[j+1];
			}
			//make the last an unneeded pointer a null-pointer
			attributes[attributesSize-1] = NULL;
			//and remove it
			attributes = (VapixAttribute**)realloc(attributes, (--attributesSize)*sizeof(attributes));
			//sucessfully removed
			return;
		}
	}
	//DEBUG: nothing was removed
	MessageBox(NULL, "Attribute was not removed because it did not exsist!", "Error",MB_ICONEXCLAMATION);
}

/**
 * Returns the validAttributes list (array)
 */
VapixAttribute** VapixFunction::getAttributes(int* aSize) {
	*aSize = attributesSize;
	return attributes;
}

/**
 * Returns the attributes list (array)
 */
VapixAttribute** VapixFunction::getValidAttributes(int* vaSize) {
	*vaSize = validAttributesSize;
	return validAttributes;
}

/**
 * Returns the validAttribute at index
 */
VapixAttribute* VapixFunction::getValidAttribute(int index) {
	return validAttributes[index];
}

/**
 * Returns the attribute at index
 */
VapixAttribute* VapixFunction::getAttribute(int index) {
	return attributes[index];
}

/**
 * Returns the fully qualified http string
 * (including attributes) for this function
 */
char* VapixFunction::getString() {
	char* temp = (char*)malloc(sizeof(char)*512);
	//first get the 'something-cgi/blabla.cgi'
	strcpy(temp, getFunctionString());
	//then for each attrib concat its uri-string '?bla1=muuu&bla2=bההה'
	for(int i=0; i<attributesSize; i++) {
		if(i==0)
			strcat(temp, "?");
		else
			strcat(temp, "&"); //the & sign does not show in STATIC controls.....
		strcat(temp, attributes[i]->getString());
	}
	//last, return it all: 'something-cgi/blabla.cgi?bla1=muuu&bla2=bההה'
	return temp;
}

void VapixFunction::setOpener(int opener) {
	this->opener = opener;
}

/**
 * getOpener() - returns
 *	- 0 if the function is to be sent and results displayed
 *  - 1 if the function is to be opened in a browser
 *  - 2 if the function is to be opened in a video player
 */
int VapixFunction::getOpener() {
	return opener;
}

/**
 * Checks if the validAttribute already exists
 */
int VapixFunction::validAttributeExists(VapixAttribute* attribute) {
	for(int i=0; i<validAttributesSize; i++) {
		if(validAttributes[i]->isEqual(attribute)) {
			return i;
		}
	}
	return -1;
}

/**
 * Checks if the attribute already exists
 */
int VapixFunction::attributeExists(VapixAttribute* attribute) {
	for(int i=0; i<attributesSize; i++) {
		if(attributes[i]->isEqual(attribute)) {
			return i;
		}
	}
	return -1;
}

void VapixFunction::removeAllAttributes() {
	for(int i=0; i<attributesSize; i++) {
		removeAttribute(attributes[i]);
	}
}

/**
 * Adds a valid protocol to the valid protocols array/list
 */
void VapixFunction::addValidProtocol(char** protocol) {
	//reallocate to fit the new protocol (char** or char[2]) pointer
	validProtocols = (char***)realloc(validProtocols, (++validProtocolsSize)*sizeof(char**));
	//allocate memory for the new protocol
	validProtocols[validProtocolsSize-1] = (char**)malloc(sizeof(char*)*2);
	//get length of protocol[0] (the name part)
	int strLen = strlen(protocol[0]);
	//allocate memory for it
	validProtocols[validProtocolsSize-1][0] = (char*)malloc(sizeof(char)*strLen);
	//copy over the name part
	strcpy(validProtocols[validProtocolsSize-1][0], protocol[0]);
	//repeat all for second part (protocol[1] - port part)
	strLen = strlen(protocol[1]);
	validProtocols[validProtocolsSize-1][1] = (char*)malloc(sizeof(char)*strLen);
	strcpy(validProtocols[validProtocolsSize-1][1], protocol[1]);
}

/**
 * Adds a valid protocol to the valid protocols array/list
 */
void VapixFunction::addValidProtocol(char* name, char* port) {
	char** protocol = (char**)malloc(sizeof(char*)*2);
	protocol[0] = (char*)malloc(sizeof(char)*6);
	memset(protocol[0], '\0', 6);
	protocol[1] = (char*)malloc(sizeof(char)*7);
	memset(protocol[1], '\0', 7);
	strcpy(protocol[0], name);
	strcpy(protocol[1], port);
	addValidProtocol(protocol);
}

/**
 * Returns the whole valid protocols array/list
 * and sets the valid protocols size in the passed argument
 */
char ***VapixFunction::getValidProtocols(int* vpSize) {
	*vpSize = validProtocolsSize;
	return validProtocols;
}

/**
 * Returns the valid protocol at index
 */
char **VapixFunction::getValidProtocol(int index) {
	return validProtocols[index];
}

/**
 * Returns the chosen protocol[2] for this function
 *  - protocol[0]: protocol name
 *  - protocol[1]: protocol port
 */
char **VapixFunction::getProtocol() {
	return protocol;
}

/**
 * Returns the selected protocol index in the
 * validProtocols array/list
 */
int VapixFunction::getProtocolIndex() {
	//loop the validProtocols to
	//match the selected protocol
	for(int i=0; i<validProtocolsSize; i++) {
		//comparing pointers
		if(validProtocols[i] == protocol)
			//eureka!
			return i;
	}
	//sorry, no index for you today
	return -1;
}

/**
 * Sets the protocol to be used
 */
BOOL VapixFunction::setProtocol(int index) {
	//check if protocol is in range of valid ones
	if(isValidProtocol(index)) {
		//point the protocol (char[2]) to the
		//desired validProtocols[index] (char[2])
		this->protocol = validProtocols[index];
		return TRUE;
	}
	return FALSE;
}

/**
 * Check if protocol (index) is in range of valid ones
 */
BOOL VapixFunction::isValidProtocol(int index) {
	if(index >= 0 && index <= validProtocolsSize)
		return TRUE;
	return FALSE;
}

int VapixFunction::getValidProtocolsSize() {
	return validProtocolsSize;
}