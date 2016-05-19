#include"common_data.h"
#include"StringUtil.h"

#include <algorithm>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <utils_i18n.h>

namespace csk
{

void
StringUtil::changeLineFeed(const std::string& source, std::string& unescaped)
{
	std::string output(source);

	std::string::size_type pos = 0;
	char lf = 0x0A;
	while ((pos = output.find(lf, pos)) != std::string::npos)
	{
		output.insert(pos+1, "<br/>");
		pos += 5;
	}

	unescaped = output;
}

static const char StringMachingTable[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P','Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

bool
StringUtil::encodeTextToBase64(const char *source, int numBytes, std::string& encodedText)
{
	unsigned char input[3]  = {0, 0, 0};
	unsigned char output[4] = {0, 0, 0, 0};
	int i, j;
	char const *p = NULL, *plen = NULL;

	plen = source + numBytes - 1;
	int size = (4 * (numBytes / 3)) + ((numBytes % 3)? 4 : 0) + 1;
	char* _text = (char*)malloc(size);
	if(NULL == _text)
		return false;

	for(i = 0, j = 0, p = source;p <= plen; i++, p++) {
		int index = i % 3;
		input[index] = *p;

		if (index == 2 || p == plen)
		{
			output[0] = ((input[0] & 0xFC) >> 2);
			output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
			output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
			output[3] = (input[2] & 0x3F);

			_text[j++] = StringMachingTable[output[0]];
			_text[j++] = StringMachingTable[output[1]];
			_text[j++] = index == 0? '=' : StringMachingTable[output[2]];
			_text[j++] = index <  2? '=' : StringMachingTable[output[3]];

			input[0] = input[1] = input[2] = 0;
		}
	}
	_text[j] = '\0';

	encodedText.assign(_text);
	free(_text);
	return true;
}

char*
StringUtil::convertUTF8ToAnsiChar(const char *utf8)
{
	int unicode_len = strlen(utf8) + 1;
	i18n_uchar unicode[strlen(utf8) + 1];
	char *ansi = (char*) malloc( (strlen(utf8)+1)*sizeof(char) );
	if(NULL == ansi)
	{
		return NULL;
	}

	int unicode_actual_len = 0;
	i18n_error_code_e error_code = I18N_ERROR_NONE;
	i18n_ustring_from_UTF8(unicode, unicode_len, &unicode_actual_len, utf8, strlen(utf8), &error_code);
	D_EMSG("unicode_len = %d, actual_len = %d, error code = %04X", unicode_len, unicode_actual_len, error_code);

	//i18n_ustring_copy_ua(unicode, utf8);

	i18n_ustring_copy_au(ansi, unicode);
	D_EMSG("ansi = %d, unicode = %d", strlen(ansi), i18n_ustring_get_length(unicode));

	return ansi;
}

std::string
StringUtil::replaceAll(std::string src, const std::string& from, const std::string& to)
{
	int start_pos = 0;
	while((start_pos = src.find(from, start_pos)) != std::string::npos)
	{
		src.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return src;
}

void
StringUtil::printAllString(const std::string& src)
{
	static char tmp[802] = {0,};
	const char* tmp_ptr = src.c_str();
	D_EMSG("sizeof src = %d, %d", src.length(), src.length() / 800);
	for(int i = 0; i <= (int)(src.length() / 800); i++)
	{
		strncpy(tmp, &tmp_ptr[i*800], 800);
		D_EMSG("%s", tmp);
	}
}

}
