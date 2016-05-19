#include"csk_impl.h"
#include<string>
#include<stdio.h>
#include<string.h>

#include<Ecore.h>

#include"common_data.h"
#include"csk_log.h"
#include"JSONParser.h"
#include"HTTPHandler.h"
#include"RESTInvoker.h"
#include"SubsystemPort.h"
#include"StringUtil.h"
#include"FileUtil.h"


static void* user_data;
static void* main_window;

using namespace csk;

void*
csk_get_user_data()
{
	return user_data;
}

int
csk_initialize(void *elm_window, void* data)
{
	if(NULL == elm_window)
	{
		D_EMSG("invalid arguments");
		return 0;
	}

	D_MSG("CSK impl is initializing!!!");

	user_data = data;
	main_window = elm_window;

	return 1;
}


void
csk_terminate()
{
	user_data = NULL;
	main_window = NULL;

	D_MSG("CSK impl is terminating!!!");
}

static void
_long_function(void *data, Ecore_Thread *thread)
{
	WEATHER_INFO* winfo = (WEATHER_INFO*)data;

	// 0. get current location
	//std::string lat("37.241448");
	//std::string lon("131.865005");

	// openweathermap provides weather information of location, but it not free.
	// 1. invoke weather API
	// api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}

	std::string weather_rest_uri("https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22nome%2C%20ak%22)&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys");
#if 0
	/*
	 * API response example :
	 * {"query":{"count":1,"created":"2016-05-17T08:20:45Z","lang":"en-US","results":{"channel":{"units":{"distance":"mi","pressure":"in","speed":"mph","temperature":"F"},"title":"Yahoo! Weather - Nome, AK, US","link":"http://us.rd.yahoo.com/dailynews/rss/weather/Country__Country/*https://weather.yahoo.com/country/state/city-2460286/","description":"Yahoo! Weather for Nome, AK, US","language":"en-us","lastBuildDate":"Tue, 17 May 2016 12:20 AM AKDT","ttl":"60","location":{"city":"Nome","country":"United States","region":" AK"},"wind":{"chill":"46","direction":"293","speed":"11"},"atmosphere":{"humidity":"80","pressure":"1008.0","rising":"0","visibility":"16.1"},"astronomy":{"sunrise":"0:25 am","sunset":"5:32 am"},"image":{"title":"Yahoo! Weather","width":"142","height":"18","link":"http://weather.yahoo.com","url":"http://l.yimg.com/a/i/brand/purplelogo//uh/us/news-wea.gif"},"item":{"title":"Conditions for Nome, AK, US at 11:00 PM AKDT","lat":"64.499474","long":"-165.405792","link":"http://us.rd.yahoo.com/dailynews/rss/weather/Country__Country/*https://weather.yahoo.com/country/state/city-2460286/","pubDate":"Mon, 16 May 2016 11:00 PM AKDT","condition":{"code":"29","date":"Mon, 16 May 2016 11:00 PM AKDT","temp":"49","text":"Partly Cloudy"},"forecast":[{"code":"34","date":"17 May 2016","day":"Tue","high":"51","low":"44","text":"Mostly Sunny"},{"code":"34","date":"18 May 2016","day":"Wed","high":"44","low":"39","text":"Mostly Sunny"},{"code":"28","date":"19 May 2016","day":"Thu","high":"44","low":"36","text":"Mostly Cloudy"},{"code":"12","date":"20 May 2016","day":"Fri","high":"42","low":"39","text":"Rain"},{"code":"5","date":"21 May 2016","day":"Sat","high":"39","low":"36","text":"Rain And Snow"},{"code":"39","date":"22 May 2016","day":"Sun","high":"38","low":"33","text":"Scattered Showers"},{"code":"39","date":"23 May 2016","day":"Mon","high":"39","low":"37","text":"Scattered Showers"},{"code":"28","date":"24 May 2016","day":"Tue","high":"38","low":"35","text":"Mostly Cloudy"},{"code":"28","date":"25 May 2016","day":"Wed","high":"40","low":"35","text":"Mostly Cloudy"},{"code":"28","date":"26 May 2016","day":"Thu","high":"40","low":"36","text":"Mostly Cloudy"}],"description":"<![CDATA[<img src=\"http://l.yimg.com/a/i/us/we/52/29.gif\"/>\n<BR />\n<b>Current Conditions:</b>\n<BR />Partly Cloudy\n<BR />\n<BR />\n<b>Forecast:</b>\n<BR /> Tue - Mostly Sunny. High: 51Low: 4\n<BR /> Wed - Mostly Sunny. High: 44Low: 39\n<BR /> Thu - Mostly Cloudy. High: 44Low: 36\n<BR /> Fri - Rain. High: 42Low: 39\n<BR /> Sat - Rain And Snow. High: 39Low: 36\n<BR />\n<BR />\n<a href=\"http://us.rd.yahoo.com/dailynews/rss/weather/Country__Country/*https://weather.yahoo.com/country/state/city-2460286/\">Full Forecast at Yahoo! Weather</a>\n<BR />\n<BR />\n(provided by <a href=\"http://www.weather.com\" >The Weather Channel</a>)\n<BR />\n]]>","guid":{"isPermaLink":"false"}}}}}}
	 */
#endif

	std::string responseHeader;
	std::string responseBody;

	if(false == RESTInvoker::invoke(RESTInvoker::REST_METHOD_READ, weather_rest_uri,
			std::string(), responseHeader, responseBody))
	{
		D_EMSG("Invalid Network!!!");
		return;
	}

	// 2. parse JSON result
	JSONParser jp(responseBody);
	//StringUtil::printAllString(responseBody);

	if(false == jp.isParsed())
	{
		D_EMSG("JSON Parsing error!!!");
		return;
	}

	cJSON* query = jp.getObjectItem("query");
	if(NULL == query)
	{
		jp.print();
		D_EMSG("Invalid Response, no query!!!");
		return;
	}

	cJSON* results = jp.getObjectItem(query, "results");
	if(NULL == results)
	{
		jp.print();
		D_EMSG("Invalid Response, no results!!!");
		return;
	}

	cJSON* channel = jp.getObjectItem(results, "channel");
	if(NULL == channel)
	{
		D_EMSG("Invalid channel!!!");
		return;
	}

	cJSON* title = jp.getObjectItem(channel, "title");
	winfo->title = strdup(title->valuestring);

	cJSON* item = jp.getObjectItem(channel, "item");
	if(item)
	{
		cJSON* condition = jp.getObjectItem(item, "condition");
		if(condition)
		{
			cJSON* date = jp.getObjectItem(condition, "date");
			cJSON* temp = jp.getObjectItem(condition, "temp");
			cJSON* text = jp.getObjectItem(condition, "text");

			/*
			D_EMSG("date = %s", date?date->valuestring:"-");
			D_EMSG("temp = %s", temp?temp->valuestring:"-");
			D_EMSG("text = %s", text?text->valuestring:"-");
			 */

			winfo->date = strdup(date->valuestring);
			winfo->temp = strdup(temp->valuestring);
			winfo->desc = strdup(text->valuestring);
		}

		cJSON* forecast = jp.getObjectItem(item, "forecast");
		if(forecast)
		{
			std::string forcasts_result("[");
			int size = jp.getArraySize(forecast);

			for(int i = 0; i < size; i++)
			{
				cJSON* array_item = jp.getArrayItem(forecast, i);
				if(NULL == array_item)
				{
					continue;
				}
				cJSON* date = jp.getObjectItem(array_item, "date");
				cJSON* high = jp.getObjectItem(array_item, "high");
				cJSON* low = jp.getObjectItem(array_item, "low");
				cJSON* text = jp.getObjectItem(array_item, "text");

				/*
				D_EMSG("%s, %s ~ %s, %s", date->valuestring?date->valuestring:"-",
						high->valuestring?high->valuestring:"-",
								low->valuestring?low->valuestring:"-",
										text->valuestring?text->valuestring:"-");
				 */

				forcasts_result += "[";
				forcasts_result += date->valuestring;
				forcasts_result += "] ";
				forcasts_result += low->valuestring;
				forcasts_result += " ~ ";
				forcasts_result += high->valuestring;
				forcasts_result += ", ";
				forcasts_result += text->valuestring;
				forcasts_result += "<br>";
			}
			winfo->forecasts = strdup(forcasts_result.c_str());
		}
	}

	winfo->result = true;
}

static void
_end_func(void *data, Ecore_Thread *thread)
{
	WEATHER_INFO* winfo = (WEATHER_INFO*)data;

	winfo->callback(user_data, winfo);

	free((void*)winfo->title);
	free((void*)winfo->desc);
	free((void*)winfo->date);
	free((void*)winfo->temp);
	free((void*)winfo->forecasts);
	free(winfo);
}

int
csk_get_weather_of_here(on_weather_response_called callback)
{
	if(NULL == callback)
	{
		D_EMSG("Invalid Argument!!!");
		return 0;
	}

	WEATHER_INFO* winfo = (WEATHER_INFO*)malloc(sizeof(WEATHER_INFO));
	memset(winfo, 0, sizeof(WEATHER_INFO));
	winfo->callback = callback;
	ecore_thread_run(_long_function, _end_func, _end_func, winfo);

	return 1;
}
