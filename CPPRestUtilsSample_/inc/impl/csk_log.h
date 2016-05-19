#ifndef __CSK_LOG_H__
#define __CSK_LOG_H__


#include <dlog.h>

#ifdef  CSK_LOG_TAG
#undef  CSK_LOG_TAG
#endif
#define CSK_LOG_TAG "csk"

static const int DEBUG_ENTER_LEAVE = 0;
static const int DEBUG_DISPLAY_FUNCTION = 1;

#define D_MSG(fmt, args...)       \
		if(DEBUG_DISPLAY_FUNCTION)  dlog_print(DLOG_DEBUG, CSK_LOG_TAG, "[%s %s() #%d ] " fmt "\n", (rindex(__FILE__, '/') ? (rindex(__FILE__, '/')+1) : __FILE__), __FUNCTION__,  __LINE__, ##args); \
		else dlog_print(DLOG_DEBUG, CSK_LOG_TAG, fmt, ##args);

#define D_EMSG(fmt, args...)       \
		dlog_print(DLOG_ERROR, CSK_LOG_TAG, "[ERROR][%s %s() #%d ]" fmt "\n", (rindex(__FILE__, '/') ? (rindex(__FILE__, '/')+1) : __FILE__), __FUNCTION__, __LINE__, ##args);

#define D_ENTER\
		if(DEBUG_ENTER_LEAVE) dlog_print(DLOG_DEBUG, CSK_LOG_TAG, "[ENTER][%s %s(%p) #%d ]>>>>>\n", (rindex(__FILE__, '/') ? (rindex(__FILE__, '/')+1) : __FILE__), __FUNCTION__, __FUNCTION__,  __LINE__);

#define D_LEAVE\
		if(DEBUG_ENTER_LEAVE) dlog_print(DLOG_DEBUG, CSK_LOG_TAG, "[LEAVE][%s %s(%p) #%d ]<<<<<\n", (rindex(__FILE__, '/') ? (rindex(__FILE__, '/')+1) : __FILE__), __FUNCTION__, __FUNCTION__,  __LINE__);

#ifndef RETM_IF
#define RETM_IF(expr, fmt, arg...) \
{ \
    if (expr) { \
    	D_EMSG(fmt, ##arg); \
        return; \
    } \
}
#endif

#ifndef RETVM_IF
#define RETVM_IF(expr, val, fmt, arg...) \
{ \
    if (expr) { \
    	D_EMSG(fmt, ##arg); \
        return (val); \
    } \
}
#endif

#endif /* __CSK_LOG_H__ */
