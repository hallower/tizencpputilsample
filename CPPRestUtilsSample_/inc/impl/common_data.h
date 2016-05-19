#ifndef __CSK_COMMON_DATA_H__
#define __CSK_COMMON_DATA_H__

#include<assert.h>
#include<vector>
#include<string>
#include"curl/curl.h"
#include"csk_log.h"
#include"csk_impl.h"
#include"SubsystemPort.h"



namespace csk
{

static const char* CSK_USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36";
static const long CSK_HTTP_TIMEOUT = 10000L;


class CurlRaii {
public :
	CurlRaii()
	 {
		ctx = curl_easy_init() ;
	}
	~CurlRaii()
	 {
		if(NULL != ctx)
			curl_easy_cleanup(ctx) ;
	}

	CURL* ctx;
};


}

// launcher's common function prototype

#ifdef __cplusplus
extern "C" {
#endif

int
__csk_launch_standalone_webapp(const char* csk_app_url);

void*
csk_get_user_data();

#ifdef __cplusplus
}
#endif



#endif /* __CSK_COMMON_DATA_H__ */
