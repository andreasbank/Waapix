#include "stdafx.h"
#include "VapixAttribute.h"

VapixAttribute::VapixAttribute() {
	name = (char*)malloc(sizeof(char)*200);
	description = (char*)malloc(sizeof(char)*200);
	value = (char*)malloc(sizeof(char)*200);
	strcpy(name, "\0");
	strcpy(description, "\0");
	validValues = NULL;
	strcpy(value, "\0");
	validValuesSize = 0;
}

/**
 * http://www.youtube.com/watch?v=oavMtUWDBTM
 */
VapixAttribute::VapixAttribute(char* name, char* description) {
	//VapixAttribute();
	this->name = (char*)malloc(sizeof(char)*200);
	this->description = (char*)malloc(sizeof(char)*200);
	this->value = (char*)malloc(sizeof(char)*200);
	strcpy(this->name, "\0");
	strcpy(this->description, "\0");
	this->validValues = NULL;
	strcpy(this->value, "\0");
	this->validValuesSize = 0;
	strcpy(this->name, name);
	strcpy(this->description, description);
}

/**
 * Copies a string to the name field
 */
void VapixAttribute::setName(char* name) {
	strcpy(this->name, name);
}

/**
 * Returns a pointer to the name string
 */
char* VapixAttribute::getName() {
	return name;
}

/**
 * Copies a string to the description field
 */
void VapixAttribute::setDescription(char* description) {
	strcpy(this->description, description);
}

/**
 * Returns a pointer to the description string
 */
char* VapixAttribute::getDescription() {
	return description;
}

/**
 * adds a valid value to the validValues array
 */
void VapixAttribute::addValidValue(char* value) {
	//reallocate to fit the new value pointer
	validValues = (char**)realloc(validValues, (++validValuesSize)*sizeof(validValues));
	//allocate string memory
	validValues[validValuesSize-1] = (char*)malloc(sizeof(char)*(strlen(value)+1));
	//copy the string to our memory
	strcpy(validValues[validValuesSize-1], value);
}

char* VapixAttribute::getString() {
	int sLen = strlen(name) + 2 + strlen(value);
	char* temp = (char*)malloc(sizeof(char)*sLen);
	sprintf(temp, "%s=%s", name, value);
	return temp;
}

/**
 * checks if a attribute value is valid (is in the validValues array)
 * returns the index of the valid value in the array
 */
int VapixAttribute::isValidValue(char* value) {
	for(int i=0; i<validValuesSize; i++) {
		if(strcmp(value, validValues[i]) == 0) {
			return i;
		}
	}
	return -1;
}

/**
 * checks if the value passed is valid and if true
 * references the value field with the validValues[i]
 */
BOOL VapixAttribute::setValue(char* value) {
	int index = isValidValue(value);
	if(index >= 0) {
		this->value = validValues[index];
		return TRUE;
	}
	return FALSE;
}

/**
 * returns a reference to the value field
 * TODO: return a copy inatead of a reference??
 */
char* VapixAttribute::getValue() {
	return value;
}

/**
 * return a pointer to the validValues array
 */
char** VapixAttribute::getValidValues(int* sizePointer) {
	*sizePointer = validValuesSize;
	return validValues;
}


/**
 * DA MIXER FUNCTION!
 * Compares to see if all validValues in the passed
 * attribute are valid in this instance, if they are
 * then the passed attribute is considered equal
 */
BOOL VapixAttribute::isEqual(VapixAttribute* attribute) {
	int vvSize = 0;
	char** vv = attribute->getValidValues(&vvSize);
	//check name first
	if(strcmp(name, attribute->getName()) != 0)
		return FALSE;
	//then check valid values
	for(int i=0; i<vvSize; i++) {
		if(isValidValue(vv[i]) < 0) {
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * Returns a clone (deep copy) of "this"
 * NOTE: might be rendered obsolete in future code changes.
 */
VapixAttribute* VapixAttribute::clone() {
	//create the object to return
	VapixAttribute* cloneAttribute = new VapixAttribute();
	//copy name
	cloneAttribute->setName(this->getName());
	//copy description
	cloneAttribute->setDescription(this->getDescription());
	//size of the validValues array
	int vvSize = 0;
	//the validValues array
	char** vv = this->getValidValues(&vvSize);
	//add all validValues from attribute to cloneAttribute
	for (int i=0; i<vvSize; i++) {
		cloneAttribute->addValidValue(vv[i]);
	}
	//copy value
	cloneAttribute->setValue(this->getValue());
	//return the clone
	return cloneAttribute;
}