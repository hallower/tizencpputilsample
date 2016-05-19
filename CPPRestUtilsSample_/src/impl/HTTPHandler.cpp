#include <ctype.h>
#include "HTTPHandler.h"
#include <stdlib.h>

namespace csk
{

struct WriteBuffer {
	char *memory;
	size_t size;

	WriteBuffer()
	:memory(NULL),
	 size(0)
	{
	}

	~WriteBuffer()
	{
		if(NULL != memory)
		{
			free(memory);
			memory = NULL;
		}
		size = 0;
	}
};


static size_t
write_event_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realSize = size * nmemb;
	struct WriteBuffer *mem = (struct WriteBuffer *)userp;

	//D_MSG("write %d >> %d", size, mem->size);

	mem->memory = (char*)realloc(mem->memory, mem->size + realSize + 1);
	if(mem->memory == NULL)
	{
		/* out of memory! */
		D_MSG("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realSize);
	mem->size += realSize;
	mem->memory[mem->size] = 0;

	return realSize;
}


bool
HTTPHandler::download(const char* downloadUrl, char** body, long* size)
{
	CurlRaii curl;
	CURL* ctx = curl.ctx;

	if(NULL == downloadUrl ||
			NULL == size)
	{
		D_MSG("Invalid arguments");
		return false;
	}

	if( NULL == ctx )
	{
		D_MSG("Unable to initialize cURL interface");
		return false;
	}

	*body = NULL;
	*size = 0;

	struct WriteBuffer chunk;

	curl_easy_setopt(ctx, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(ctx , CURLOPT_URL,  downloadUrl);
	curl_easy_setopt(ctx, CURLOPT_USERAGENT, CSK_USER_AGENT);
	curl_easy_setopt(ctx , CURLOPT_WRITEFUNCTION , write_event_cb);
	curl_easy_setopt(ctx , CURLOPT_WRITEDATA , (void*)&chunk);
	curl_easy_setopt(ctx, CURLOPT_TIMEOUT_MS, CSK_HTTP_TIMEOUT);

	std::string proxy_addr = SubsystemPort::getProxyAddress();
	if(false == proxy_addr.empty())
	{
		curl_easy_setopt(ctx, CURLOPT_PROXY, proxy_addr.c_str());
	}

	//D_MSG("HTTP DOWNLOAD, url = %s", downloadUrl);

	const CURLcode rc = curl_easy_perform(ctx);
	if(CURLE_OK != rc)
	{
		D_MSG("Error from cURL: %s", curl_easy_strerror(rc));
		return false;
	}

	long statLong = 0;
	if(CURLE_OK == curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE, &statLong)) {
		D_MSG("Response code [GET] : %ld, for %s", statLong, downloadUrl);
	}

	if(statLong != 200)
	{
		D_MSG("Error response !!! %ld", statLong);
		return false;
	}

	if(NULL == chunk.memory)
	{
		return false;
	}

	*body = strdup(chunk.memory);
	*size = (long)chunk.size;

	return true;
}


bool
HTTPHandler::downloadAsFile(const char* downloadUrl, const char* fileFullPath)
{
	CurlRaii curl;
	CURL* ctx = curl.ctx;

	if(NULL == downloadUrl ||
			NULL == fileFullPath)
	{
		D_MSG("Invalid arguments");
		return false;
	}

	if( NULL == ctx )
	{
		D_MSG("Unable to initialize cURL interface");
		return false;
	}

	curl_easy_setopt(ctx, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(ctx , CURLOPT_URL,  downloadUrl);
	curl_easy_setopt(ctx, CURLOPT_USERAGENT, CSK_USER_AGENT);

	std::string proxy_addr = SubsystemPort::getProxyAddress();
	if(false == proxy_addr.empty())
	{
		curl_easy_setopt(ctx, CURLOPT_PROXY, proxy_addr.c_str());
	}

	FILE* downloadedFile = fopen(fileFullPath, "wb");
	if(NULL == downloadedFile)
	{
		D_MSG("File open error for downloading - %s", downloadUrl);
		return false;
	}

	curl_easy_setopt(ctx, CURLOPT_WRITEDATA, downloadedFile);
	//D_MSG("HTTP DOWNLOAD, url = %s, filename = %s", downloadUrl, fileFullPath);

	const CURLcode rc = curl_easy_perform(ctx);
	fclose(downloadedFile);
	if(CURLE_OK != rc)
	{
		D_MSG("Error from cURL: %s", curl_easy_strerror(rc));
		return false;
	}

	long statLong = 0;

	if(CURLE_OK == curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE , &statLong)) {
		D_MSG("Response code [GET] : %ld, for %s", statLong, downloadUrl);
	}

	if(statLong != 200)
	{
		D_MSG("Error response !!! %ld", statLong);
		return false;
	}

	return true;
}

bool
HTTPHandler::checkContentInfo(const char* downloadUrl, std::string& mimetype, long* size)
{
	CurlRaii curl;
	CURL* ctx = curl.ctx;

	if(NULL == downloadUrl ||
			NULL == size)
	{
		D_MSG("Invalid arguments");
		return false;
	}

	mimetype.clear();
	*size = 0;

	if( NULL == ctx )
	{
		D_MSG("Unable to initialize cURL interface");
		return false;
	}

	curl_easy_setopt(ctx, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(ctx, CURLOPT_HEADER, 1);
	curl_easy_setopt(ctx, CURLOPT_URL,  downloadUrl);
	curl_easy_setopt(ctx, CURLOPT_USERAGENT, CSK_USER_AGENT);
	curl_easy_setopt(ctx, CURLOPT_NOBODY, 1);
	curl_easy_setopt(ctx, CURLOPT_TIMEOUT_MS, CSK_HTTP_TIMEOUT);

	//D_MSG("Check Content, url = %s", downloadUrl);

	const CURLcode rc = curl_easy_perform(ctx);
	if(CURLE_OK != rc)
	{
		D_MSG("Error from cURL: %s", curl_easy_strerror(rc));
		return false;
	}

	long statLong = 0;
	char* statString = NULL;
	double statDouble = 0;

	if(CURLE_OK == curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE , &statLong)) {
		D_MSG("Response code [HEAD] : %ld, for %s", statLong, downloadUrl);
	}

	if(statLong != 200)
	{
		D_MSG("Error response !!! %ld", statLong);
		return false;
	}

	if(CURLE_OK == curl_easy_getinfo(ctx, CURLINFO_CONTENT_TYPE , &statString)) {
		//D_MSG("Content type: %s", statString);
		if(NULL != statString)
			mimetype.assign(statString);
	}

	if(CURLE_OK == curl_easy_getinfo(ctx, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &statDouble)) {
		long tempLong = (long)statDouble;
		//D_MSG("Download size: %f bytes, %ld bytes", statDouble, tempLong);
		*size = tempLong;
	}

	return true;
}

static char
from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

void
HTTPHandler::decodeURL(const std::string& src, std::string& decoded)
{
	const char* pstr = src.c_str();
	char buf[src.length() + 1];
	char* pbuf = buf;

	while (*pstr)
	{
		if (*pstr == '%') {
			if (pstr[1] && pstr[2]) {
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		} else if (*pstr == '+') {
			*pbuf++ = ' ';
		} else {
			*pbuf++ = *pstr;
		}
		pstr++;
	}
	*pbuf = '\0';
	decoded.append(buf);
}

}
