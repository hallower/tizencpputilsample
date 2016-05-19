#ifndef __CSK_IMAGE_UTIL_H__
#define __CSK_IMAGE_UTIL_H__


#include<Evas.h>
#include<string>

namespace csk
{

class ImageUtil {
public:
	static bool
	createResizedImage(const int srcWidth,
			const int srcHeight,
			const unsigned char *src,
			const int destWidth,
			const int destHeight,
			unsigned char *dest);

	static bool
	createGrayscaleImageFile(const char *fullPath,
			Evas_Object* windowObject);

private:
	ImageUtil();
	~ImageUtil();

	inline explicit ImageUtil(const ImageUtil&) {}

	inline ImageUtil& operator =(const ImageUtil&) { return *this; }
};

}

#endif // __CSK_IMAGE_UTIL_H__
