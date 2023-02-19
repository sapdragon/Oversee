#pragma once

#include <string>

static int writer(char* data, size_t size, size_t nmemb, std::string* buffer)
{
	int result = 0;
	if (buffer != NULL) {
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}