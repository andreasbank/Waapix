/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 by Andreas Bank, andreas.mikael.bank@gmail.com
 * ....will do this when I have shitloads of free time.
 * Feel free to do it yourself, it is supposed to be a http/xml parser.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define HTTPREADER_VERSION "N/A"

class HttpReader {
	static const char* version;
public:
	HttpReader();
	const char* getVersion();
};