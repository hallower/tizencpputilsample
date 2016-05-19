#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <stdio.h>
#include <Eina.h>
#include <linux/limits.h>
#include<dirent.h>
#include<image_util.h>
#include<Elementary.h>
#include"common_data.h"
#include"csk_log.h"
#include"FileUtil.h"
#include<sys/stat.h>

namespace csk
{

long
FileUtil::getFileSize(const char* filePath)
{
	struct stat file_info;
	if (0 > stat(filePath, &file_info))
		return -1;

	return file_info.st_size;
}

bool
FileUtil::readFile(const char* filePath, char** fileContent, long* fileSize)
{
	FILE *fp;
	fp = fopen(filePath, "rb");
	if(NULL == fp)
	{
		return false;
	}

	char* buff = NULL;
	long bufSize = FileUtil::getFileSize(filePath);
	if(bufSize < 0)
	{
		return false;
	}

	buff = (char*)malloc(bufSize + 1);
	if(NULL == buff)
	{
		fclose(fp);
		return false;
	}

	fread(buff, bufSize, 1, fp);
	buff[bufSize] = '\0';
	fclose(fp);

	*fileContent = buff;
	*fileSize = bufSize;

	return true;
}

bool
FileUtil::writeFile(const char* filePath, const char* fileContent, long fileSize)
{
	FILE *fp;
	fp = fopen(filePath, "wb");
	if(NULL == fp)
		return false;

	size_t file_total = (size_t)fileSize;
	size_t write_size = 0;
	write_size = fwrite(fileContent, 1, file_total, fp);
	while( write_size < file_total )
	{
		size_t write_once = fwrite(&fileContent[write_size], 1, file_total - write_size, fp);
		write_size += write_once;
	}
	fclose(fp);
	return true;
}

bool
FileUtil::addStringToFile(const char* filePath, const char* addString)
{
	FILE *fp;
	fp = fopen(filePath, "at");

	if(NULL == fp)
		return false;

	fprintf(fp, "%s\n", addString);
	fclose(fp);
	return true;
}

static char*
str_replace(char *orig, const char *rep, const char *with)
{
	char *result;
	char *ins;
	char *tmp;
	int lenRep;
	int lenWith;
	int lenFront;
	int count;

	if (NULL == orig ||
			NULL == rep ||
			NULL == with)
		return NULL;

	lenRep = strlen(rep);
	lenWith = strlen(with);

	ins = orig;
	for (count = 0; NULL != (tmp = strstr(ins, rep)); ++count)
		ins = tmp + lenRep;

	int mem_length = strlen(orig) + (lenWith - lenRep) * count + 1;
	tmp = result = (char*)malloc(sizeof(char) * mem_length);
	if (NULL == result)
		return NULL;

	memset(result, 0, sizeof(char) * mem_length);

	while (count--) {
		ins = strstr(orig, rep);
		lenFront = ins - orig;
		tmp = strncpy(tmp, orig, lenFront) + lenFront;
		tmp = strncpy(tmp, with, strlen(with)) + lenWith;
		orig += lenFront + lenRep;
	}
	strncpy(tmp, orig, strlen(orig));
	return result;
}

static const char* const txt_slash = "/";
static const char* const txt_colon = ":";
static const char* const txt_under_score = "_";

char*
FileUtil::getFileFullPath(const char* filePath, const char* fileName)
{
	if(NULL == filePath ||
			NULL == fileName)
	{
		D_EMSG("Invalid arguments");
		return NULL;
	}

	char* replacedIconUrl = str_replace((char*)fileName, txt_slash, txt_under_score);
	char* replacedIconUrlSecond = str_replace(replacedIconUrl, txt_colon, txt_under_score);
	free(replacedIconUrl);
	char* escapedIconUrl = eina_str_escape(replacedIconUrlSecond);
	free(replacedIconUrlSecond);

	int length = strlen(filePath) + strlen(escapedIconUrl) + 1;
	char* fileFullPath = (char*)malloc(sizeof(char) * length);
	if(NULL == fileFullPath)
		return NULL;

	memset(fileFullPath, 0, sizeof(char) * length);
	snprintf(fileFullPath, length, "%s%s", filePath, escapedIconUrl);
	return fileFullPath;
}

bool
FileUtil::getFileFullPath(const char* filePath, const char* fileName, std::string& fileFullPath)
{
	fileFullPath.clear();

	char* fullPath = getFileFullPath(filePath, fileName);
	if(NULL == fullPath)
		return false;

	fileFullPath.assign(fullPath);
	free(fullPath);
	return true;
}

}
