#ifndef __CSK_JSON_PARSER_H__
#define __CSK_JSON_PARSER_H__


#include<stdbool.h>
#include<cJSON.h>
#include<string>


namespace csk
{


class JSONParser {
public :
	JSONParser(const std::string& json);
	JSONParser(const char* json);
	~JSONParser();

	const cJSON* getRootObjectItem() const;

	cJSON* getObjectItem(const std::string& tag) const;
	cJSON* getObjectItem(const char* tag) const;

	cJSON* getObjectItem(cJSON* object, const std::string& tag) const;
	cJSON* getObjectItem(cJSON* object, const char* tag) const;

	cJSON* getArrayItem(cJSON* array, int index) const;
	int getArraySize(cJSON* array) const;

	cJSON* getRootArrayItem(int index) const;
	int getRootArraySize() const;

	bool isParsed();

	void print();
	void print(cJSON* node);

private :
	inline explicit JSONParser(JSONParser const&)
	: jsonRoot(NULL) {}

	inline JSONParser& operator=(JSONParser const&) { return *this; }

	void parse(const char* json);

private :

	cJSON* jsonRoot;
};

}

#endif /* __CSK_JSON_PARSER_H__ */
