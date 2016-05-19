#ifndef __CSK_REST_INVOKER_H__
#define __CSK_REST_INVOKER_H__


#include <stdlib.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <string>
#include "common_data.h"


namespace csk
{

class RESTInvoker {
public :
	typedef enum {
		REST_METHOD_CREATE,
		REST_METHOD_READ,
		REST_METHOD_UPDATE,
		REST_METHOD_DELETE
	}RestMethodType;

	class RESTResponseData;

	struct IRESTResponseListener {
		virtual ~IRESTResponseListener() {};
		virtual void RESTResponseReceived(bool result, const RESTResponseData& data) = 0;
	};

	class RESTResponseData {
	public :
		RESTResponseData(IRESTResponseListener& listener)
		: listener(listener),
		  requstMethod(REST_METHOD_READ),
		  responseCode(0),
		  body(NULL),
		  bodySize(0),
		  header(NULL),
		  headerSize(0)
		 {
		}

		virtual ~RESTResponseData()
		 {
			if(body)	free(body);
			if(header)	free(header);
		}

		IRESTResponseListener& listener;
		RestMethodType requstMethod;
		int responseCode;
		std::string caller;
		std::string url;
		char* body;
		int bodySize;
		char* header;
		int headerSize;
	};

public :

	static bool
	invoke(const RestMethodType method,
			const std::string& fullUrl,
			const std::string& parameter,
			const std::string& caller,
			IRESTResponseListener& listener
	);

	static bool
	invoke(const RestMethodType method,
			const std::string& fullUrl,
			const std::string& parameter,
			std::string& responseBody
	);

	static bool
	invoke(const RestMethodType method,
			const std::string& fullUrl,
			const std::string& parameter,
			std::string& responseHeader,
			std::string& responseBody
	);

protected:
	static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata);

	static size_t body_callback(char* buffer, size_t size, size_t nitems, void* userdata);

	static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata);

private:
	static bool
	__invoke(const RestMethodType method,
			const std::string& fullUrl,
			const std::string& parameter,
			const std::string& caller,
			IRESTResponseListener& listener,
			std::string& responseBody,
			const bool returnByResponseBody,
			std::string& responseHeader,
			const bool needsResponseHeader
	);

private:
	inline explicit RESTInvoker()
	 {
		curl_global_init(CURL_GLOBAL_ALL);
	}

	inline ~RESTInvoker()
	 {
		curl_global_cleanup();
	}

	inline explicit RESTInvoker(const RESTInvoker&)
	 {
	}

	inline RESTInvoker& operator =(const RESTInvoker&) { return *this; }
};

}

#endif /* __CSK_REST_INVOKER_H__ */
