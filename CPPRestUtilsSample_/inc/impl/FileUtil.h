#ifndef __CSK_FILE_UTIL_H__
#define __CSK_FILE_UTIL_H__

#include<stdbool.h>
#include<string>

namespace csk
{

class FileUtil {
public:
	static long getFileSize(const char* filePath);

	static bool readFile(const char* filePath, char** fileContent, long* fileSize);

	static bool writeFile(const char* filePath, const char* fileContent, long fileSize);

	static bool addStringToFile(const char* filePath, const char* addString);

	static bool getFileFullPath(const char* filePath, const char* fileName, std::string& fileFullPath);

private:
	static char* getFileFullPath(const char* filePath, const char* fileName);

private:
	FileUtil();

	~FileUtil();

	inline explicit FileUtil(const FileUtil&) {}

	inline FileUtil& operator =(const FileUtil&) { return *this; }
};

}
#endif /* __CSK_FILE_UTIL_H__ */
