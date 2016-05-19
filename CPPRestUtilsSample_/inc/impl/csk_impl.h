#ifndef __CSK_LAUNCHER_H__
#define __CSK_LAUNCHER_H__


#include <app.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __EXPORT_API__
#define __EXPORT_API__	__attribute__((visibility("default")))
#endif


int __EXPORT_API__
csk_initialize(void* elm_window, void* user_data);

void __EXPORT_API__
csk_terminate();

/*
void* __EXPORT_API__
csk_get_user_data();
*/

struct WEATHER_INFO;
typedef void (*on_weather_response_called)(void *data, const struct WEATHER_INFO* info);

struct WEATHER_INFO
{
	bool result;
	const char* title;
	const char* date;
	const char* temp;
	const char* desc;
	const char* forecasts;
	on_weather_response_called callback;
};



int __EXPORT_API__
csk_get_weather_of_here(on_weather_response_called callback);

#ifdef __cplusplus
}
#endif

#endif /* __CSK_LAUNCHER_H__ */


