#include "stdafx.h"
#include "HttpReader.h"

const char* HttpReader::version = HTTPREADER_VERSION;

HttpReader::HttpReader() {

}

const char* HttpReader::getVersion() {
	return version;
}