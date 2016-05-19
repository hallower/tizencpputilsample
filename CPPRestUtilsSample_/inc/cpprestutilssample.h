#ifndef __cpprestutilssample_H__
#define __cpprestutilssample_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "cpprestutilssample"

#if !defined(PACKAGE)
#define PACKAGE "org.example.cpprestutilssample"
#endif

#define EDJ_FILE "edje/cpprestutilssample.edj"
#define GRP_MAIN "main"


#endif /* __cpprestutilssample_H__ */
