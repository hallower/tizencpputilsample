#ifndef __CSK_HTTP_HANDLER_H__
#define __CSK_HTTP_HANDLER_H__


#include<stdbool.h>
#include<string>
#include"curl/curl.h"
#include"common_data.h"


namespace csk
{

class HTTPHandler {
public :

	inline static HTTPHandler& getInstance()
	 {
		static HTTPHandler instance;
		return instance;
	}

	static bool download(const char* downloadUrl, char** body, long* size);

	static bool downloadAsFile(const char* downloadUrl, const char* fileFullPath);

	static bool checkContentInfo(const char* downloadUrl, std::string& mimetype, long* size);

	static void decodeURL(const std::string& src, std::string& decoded);

private:
	inline explicit HTTPHandler()
	 {
		curl_global_init(CURL_GLOBAL_ALL);
	}

	inline ~HTTPHandler()
	 {
		curl_global_cleanup();
	}

	inline explicit HTTPHandler(const HTTPHandler&) { }

	inline HTTPHandler& operator =(const HTTPHandler&) { return *this; }
};

}

#endif /* __CSK_HTTP_HANDLER_H__ */
