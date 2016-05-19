/*
 * SubsystemPort.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: cskim
 */

#include"SubsystemPort.h"
#include<stdlib.h>
#include<net_connection.h>

#include"common_data.h"

namespace csk
{

SubsystemPort::SubsystemPort()
{

}

SubsystemPort::~SubsystemPort()
{
}

bool
SubsystemPort::openWebBrowser(const std::string& url)
{
	app_control_h appControl;

	int ret = app_control_create(&appControl);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_create() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	ret = app_control_set_operation(appControl, APP_CONTROL_OPERATION_VIEW);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_set_operation() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	ret = app_control_set_uri(appControl, url.c_str());
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_set_uri() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	ret = app_control_send_launch_request(appControl, NULL, NULL);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_send_launch_request() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	ret = app_control_destroy(appControl);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_destroy() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	return true;
}

bool
SubsystemPort::openTextReceivingApps(const std::string& url)
{
	app_control_h appControl;

	int ret = app_control_create(&appControl);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_create() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	ret = app_control_set_operation(appControl, APP_CONTROL_OPERATION_SHARE_TEXT);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_set_operation() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	if(strlen(url.c_str()) > 0)
	{
		D_EMSG("app_control_set_operation() is strlen(url.c_str()) = %d", strlen(url.c_str()));

		ret = app_control_add_extra_data(appControl, APP_CONTROL_DATA_TEXT, url.c_str());
		if(ret != APP_CONTROL_ERROR_NONE)
		{
			D_EMSG("app_control_add_extra_data() is failed. err = %d, %s", ret, get_error_message(ret));
			return false;
		}
	}

	ret = app_control_send_launch_request(appControl, NULL, NULL);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_send_launch_request() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	ret = app_control_destroy(appControl);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		D_EMSG("app_control_destroy() is failed. err = %d, %s", ret, get_error_message(ret));
		return false;
	}

	return true;
}

void
SubsystemPort::getAppResource(const char *edj_file_in, std::string& edj_path_out)
{
	edj_path_out.assign(app_get_resource_path());
	edj_path_out.append(edj_file_in);
}

std::string
SubsystemPort::getProxyAddress()
{
	int ret;
	connection_h connection;
	std::string proxy_addr;
	char* tmp_addr = NULL;

	ret = connection_create(&connection);
	if(CONNECTION_ERROR_NONE != ret)
	{
		D_EMSG("connection_create() is failed. err = %d, %s", ret, get_error_message(ret));
		return proxy_addr;
	}

	ret = connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &tmp_addr);
	if(CONNECTION_ERROR_NONE != ret)
	{
		D_EMSG("connection_get_proxy() is failed. err = %d, %s", ret, get_error_message(ret));
		return proxy_addr;
	}

	D_EMSG("Proxy Addr : %s", tmp_addr);
	proxy_addr.assign(tmp_addr);
	free(tmp_addr);

	connection_destroy(connection);
	return proxy_addr;
}

}
