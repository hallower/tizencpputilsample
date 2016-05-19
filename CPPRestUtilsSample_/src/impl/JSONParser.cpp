#include<JSONParser.h>
#include<common_data.h>


namespace csk
{


JSONParser::JSONParser(const std::string& json)
: jsonRoot(NULL)
{
	parse(json.c_str());
}

JSONParser::JSONParser(const char* json)
: jsonRoot(NULL)
{
	parse(json);
}

JSONParser::~JSONParser()
{
	if(jsonRoot)
		cJSON_Delete(jsonRoot);
}

void
JSONParser::parse(const char* json)
{
	jsonRoot = cJSON_Parse(json);
	if (NULL == jsonRoot)
	{
		D_EMSG("JSON parsing error, %s", json);
		jsonRoot = NULL;
	}
}

bool
JSONParser::isParsed()
{
	return (jsonRoot != NULL);
}

void
JSONParser::print()
{
	if(jsonRoot)
		cJSON_Print(jsonRoot);
}

void
JSONParser::print(cJSON* node)
{
	if(node)
		cJSON_Print(node);
}

const cJSON*
JSONParser::getRootObjectItem() const
{
	return jsonRoot;
}

cJSON*
JSONParser::getObjectItem(const std::string& tag) const
{
	return getObjectItem(tag.c_str());
}

cJSON*
JSONParser::getObjectItem(const char* tag) const
{
	return cJSON_GetObjectItem(jsonRoot, tag);
}

cJSON*
JSONParser::getObjectItem(cJSON* object, const std::string& tag) const
{
	return getObjectItem(object, tag.c_str());
}

cJSON*
JSONParser::getObjectItem(cJSON* object, const char* tag) const
{
	return cJSON_GetObjectItem(object, tag);
}

cJSON*
JSONParser::getArrayItem(cJSON* array, int index) const
{
	assert(index >= 0);
	return cJSON_GetArrayItem(array, index);
}

int
JSONParser::getArraySize(cJSON* array) const
{
	return cJSON_GetArraySize(array);
}

cJSON*
JSONParser::getRootArrayItem(int index) const
{
	assert(index >= 0);
	return cJSON_GetArrayItem(jsonRoot, index);
}

int
JSONParser::getRootArraySize() const
{
	return cJSON_GetArraySize(jsonRoot);
}

}
