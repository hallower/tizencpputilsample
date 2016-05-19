#ifndef __CSK_STRING_UTIL_H__
#define __CSK_STRING_UTIL_H__

#include <string>
#include <vector>

namespace csk
{

class StringUtil {
public:
	static void
	changeLineFeed(const std::string& source, std::string& output);

	static bool
	encodeTextToBase64(const char *source, int numBytes, std::string& encodedText);

	static void
	encode(std::string& data);

	static char*
	convertUTF8ToAnsiChar(const char *utf8_text);

	static std::string
	replaceAll(std::string source, const std::string& old_string, const std::string& new_string);

	static void
	printAllString(const std::string& src);


private:
	StringUtil();
	~StringUtil();

	inline explicit StringUtil(const StringUtil&) {}
	inline StringUtil& operator =(const StringUtil&) { return *this; }
};

}

#endif // __CSK_STRING_UTIL_H__
