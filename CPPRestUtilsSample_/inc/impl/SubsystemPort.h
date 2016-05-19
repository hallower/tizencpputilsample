#ifndef __CSK_SUBSYSTEMPORT_H__
#define __CSK_SUBSYSTEMPORT_H__

#include<string>

namespace csk
{

class SubsystemPort {
public:
	inline static SubsystemPort& getInstance() {
		static SubsystemPort instance;
		return instance;
	}

	static bool openWebBrowser(const std::string& url);

	static bool openTextReceivingApps(const std::string& url);

	static void getAppResource(const char *edj_file_in, std::string& edj_path_out);

	static std::string getProxyAddress();

private:
	SubsystemPort();
	~SubsystemPort();

	inline explicit SubsystemPort(const SubsystemPort&) {}
	inline SubsystemPort& operator =(const SubsystemPort&) { return *this; }
};

}
#endif /* __CSK_SUBSYSTEMPORT_H__ */
