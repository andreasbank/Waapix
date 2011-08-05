/*
 *	Copyright (C) 2011 by Andreas Bank, andreas.mikael.bank@gmail.com
 *	NOTE: I refer to 'pointers to pointers' as arrays, don't get confused!
 */

#include <string.h>
#include <malloc.h>

class VapixAttribute {
	char* name; //name of the attribute
	char* description; //description of the attribute
	char** validValues; //C-style array of valid attribute values
	int validValuesSize; // the size of the above "array"
	char* value; //should point to a char* in *validValues when set
public:
	VapixAttribute();
	VapixAttribute(char* name, char* description);
	void setName(char* name);
	char* getName();
	void setDescription(char* description);
	char* getDescription();
	void addValidValue(char* value);
	char* getString();
	int isValidValue(char* value);
	BOOL setValue(char* value);
	char* getValue();
	char** getValidValues(int* sizePointer);
	BOOL isEqual(VapixAttribute* attribute);
	VapixAttribute* clone();
};