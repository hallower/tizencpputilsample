#include "RESTInvoker.h"
#include "common_data.h"
#include "StringUtil.h"

namespace csk
{


size_t
RESTInvoker::header_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
	RESTResponseData* result_data = (RESTResponseData*)userdata;
	size_t realsize = size * nitems;

	assert(NULL != result_data);

	if(NULL == result_data->header)
	{
		result_data->header = (char*)malloc(realsize + 1);
	} else {
		result_data->header = (char*)realloc(result_data->header, result_data->headerSize + realsize + 1);
	}

	if(result_data->header == NULL)
	{
		D_EMSG("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(result_data->header[result_data->headerSize]), buffer, realsize);
	result_data->headerSize += realsize;
	result_data->header[result_data->headerSize] = 0;

	D_MSG("H[%d/%d] %s", realsize, result_data->headerSize, buffer);
	return realsize;
}


size_t
RESTInvoker::body_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
	RESTResponseData* result_data = (RESTResponseData*)userdata;
	size_t realsize = size * nitems;

	assert(NULL != result_data);

	if(NULL == result_data->body)
	{
		result_data->body = (char*)malloc(realsize + 1);
	} else {
		result_data->body = (char*)realloc(result_data->body, result_data->bodySize + realsize + 1);
	}

	if(result_data->body == NULL)
	{
		D_EMSG("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(result_data->body[result_data->bodySize]), buffer, realsize);
	result_data->bodySize += realsize;
	result_data->body[result_data->bodySize] = 0;
D_MSG("B[%d/%d] %s", realsize, result_data->bodySize, buffer);
	return realsize;
}


size_t
RESTInvoker::read_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
	std::string* parameter = (std::string*)userdata;

	strncpy(buffer, parameter->c_str(), parameter->length());
	return parameter->length();
}


bool
RESTInvoker::__invoke(const RestMethodType method,
		const std::string& fullUrl,
		const std::string& parameter,
		const std::string& caller,
		IRESTResponseListener& listener,
		std::string& responseBody,
		const bool return_by_responseBody,
		std::string& responseHeader,
		const bool needs_responseHeader
)
{
	CurlRaii curl;
	CURL* ctx = curl.ctx;

	if( NULL == ctx )
	{
		D_MSG("Unable to initialize cURL interface");
		return false;
	}

	responseBody.clear();

	RESTResponseData* resultData = new RESTResponseData(listener);
	resultData->requstMethod = method;
	resultData->caller = caller;
	resultData->url = fullUrl;

	curl_easy_setopt(ctx, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(ctx, CURLOPT_URL,  fullUrl.c_str());
	curl_easy_setopt(ctx, CURLOPT_USERAGENT, CSK_USER_AGENT);
	curl_easy_setopt(ctx, CURLOPT_TIMEOUT_MS, CSK_HTTP_TIMEOUT);

	std::string proxy_addr = SubsystemPort::getProxyAddress();
	if(false == proxy_addr.empty())
	{
		curl_easy_setopt(ctx, CURLOPT_PROXY, proxy_addr.c_str());
	}

	curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION , body_callback);
	curl_easy_setopt(ctx, CURLOPT_WRITEDATA , resultData);

	if(needs_responseHeader)
	{
		curl_easy_setopt(ctx, CURLOPT_HEADERFUNCTION , header_callback);
		curl_easy_setopt(ctx, CURLOPT_HEADERDATA , resultData);
	}

	struct curl_slist* chunk = NULL;
	chunk = curl_slist_append(chunk, "Content-Type: application/json; charset=utf-8");

	switch(method)
	{
	case REST_METHOD_CREATE :
	{
		curl_easy_setopt(ctx, CURLOPT_POST, 1);
		curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, parameter.c_str());
	}
	break;

	case REST_METHOD_READ :
		break;

	case REST_METHOD_UPDATE :
		curl_easy_setopt(ctx, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, parameter.c_str());
		break;

	case REST_METHOD_DELETE :
		curl_easy_setopt(ctx, CURLOPT_CUSTOMREQUEST, "DELETE");
		break;
	}

	curl_easy_setopt(ctx, CURLOPT_HTTPHEADER, chunk);


	const CURLcode rc = curl_easy_perform(ctx);

	D_EMSG("curl_easy_perform DONE!!!");
	if(CURLE_OK != rc)
	{
		D_MSG("Error from cURL: %s", curl_easy_strerror(rc));

		if(needs_responseHeader)
			responseHeader.clear();

		if(return_by_responseBody)
		{
			responseBody.clear();
		} else {
			resultData->responseCode = -1;
			resultData->listener.RESTResponseReceived(false, *resultData);
		}

		delete(resultData);
		curl_slist_free_all(chunk);
		return false;
	}

	long statLong = 0;

	curl_easy_getinfo(ctx , CURLINFO_HTTP_CODE , &statLong);

	if(needs_responseHeader)
	{
		if(resultData->headerSize > 0)
		{
			responseHeader.append(resultData->header);
		}
	}

	bool http_result = true;

	if(statLong < 200 ||
			statLong > 299)
	{
		D_MSG("Error HTTP Response !!! %ld for %s", statLong, fullUrl.c_str());
		http_result = false;
	}

	if(return_by_responseBody)
	{
		if(resultData->bodySize > 0)
		{
			/*
			char* result_body = StringUtil::convertUTF8ToAnsiChar(resultData->body);
			D_EMSG("------------------------");
			D_EMSG("%s", result_body);
			D_EMSG("------------------------");
			responseBody = result_body;
			D_EMSG("result , resultdata = %d, char = %d, std::string = %d", resultData->bodySize, strlen(result_body), responseBody.length());
			*/
			responseBody = resultData->body;
		}
	} else {
		resultData->responseCode = statLong;
		resultData->listener.RESTResponseReceived(http_result, *resultData);
	}

	delete(resultData);
	curl_slist_free_all(chunk);

	return http_result;
}


bool
RESTInvoker::invoke(const RestMethodType method,
		const std::string& fullUrl,
		const std::string& parameter,
		const std::string& caller,
		IRESTResponseListener& listener
)
{
	std::string temp;
	std::string header;
	return __invoke(method,
			fullUrl,
			parameter,
			caller,
			listener,
			temp,
			false,
			header,
			false);
}

class TempRestResponseListener :
		public RESTInvoker::IRESTResponseListener
		{
	void RESTResponseReceived(bool result, const RESTInvoker::RESTResponseData& data) {};
		};

static TempRestResponseListener temp_rest_response_listener;

bool
RESTInvoker::invoke(const RestMethodType method,
		const std::string& fullUrl,
		const std::string& parameter,
		std::string& responseBody
)
{
	std::string temp;
	std::string header;
	return __invoke(method,
			fullUrl,
			parameter,
			temp,
			temp_rest_response_listener,
			responseBody,
			true,
			header,
			false);
}

bool
RESTInvoker::invoke(const RestMethodType method,
		const std::string& fullUrl,
		const std::string& parameter,
		std::string& responseHeader,
		std::string& responseBody
)
{
	std::string temp;
	std::string header;
	return __invoke(method,
			fullUrl,
			parameter,
			temp,
			temp_rest_response_listener,
			responseBody,
			true,
			responseHeader,
			true);
}



}
