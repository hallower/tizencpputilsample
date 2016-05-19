#include "ImageUtil.h"

#include<string>
#include <string.h>
#include <stdlib.h>

#include<Elementary.h>
#include<assert.h>
#include"csk_log.h"


namespace csk
{

typedef unsigned char uchar;

#define BRGA_BPP 4
#define RGB24_BPP 3

#define RGB32_CHANEL_BIT_SIZE 8
#define RGB32_DEFAULT_ALPHA 255

#define RGB16_R_SIZE 5
#define RGB16_G_SIZE 6
#define RGB16_B_SIZE 5

#define RGB16_B_SHIFT 0
#define RGB16_G_SHIFT (RGB16_B_SHIFT + RGB16_B_SIZE)
#define RGB16_R_SHIFT (RGB16_G_SHIFT + RGB16_G_SHIFT)


bool
ImageUtil::createResizedImage(
		const int srcWidth,
		const int srcHeight,
		const uchar *src,
		const int destWidth,
		const int destHeight,
		uchar *dest)
{
	if (srcWidth <= 0 ||
			srcHeight <= 0 ||
			NULL == src ||
			destWidth <= 0 ||
			destHeight <= 0 ||
			NULL == dest)
		return false;

	float t, u, coef;
	t = u = coef = 0.0;
	float c1, c2, c3, c4;
	c1 = c2 = c3 = c4 = 0.0;
	u_int32_t pixel1, pixel2, pixel3, pixel4;
	pixel1 = pixel2 = pixel3 = pixel4 = 0;
	u_int32_t red, green, blue, alpha;
	red = green = blue = alpha = 0;

	for (int j = 0; j < destHeight; j++) {
		coef = (float) (j) / (float) (destHeight - 1) * (srcHeight - 1);
		int h = (int) floor(coef);
		if (h < 0)
		{
			h = 0;
		} else {
			if (h >= srcHeight - 1)
			{
				h = srcHeight - 2;
			}
		}
		u = coef - h;

		for (int i = 0; i < destWidth; i++) {
			coef = (float) (i) / (float) (destWidth - 1) * (srcWidth - 1);
			int w = (int) floor(coef);
			if (w < 0)
			{
				w = 0;
			} else {
				if (w >= srcWidth - 1)
				{
					w = srcWidth - 2;
				}
			}
			t = coef - w;

			c1 = (1 - t) * (1 - u);
			c2 = t * (1 - u);
			c3 = t * u;
			c4 = (1 - t) * u;

			pixel1 = *((u_int32_t*) (src + BRGA_BPP * (h * srcWidth + w)));
			pixel2 = *((u_int32_t*) (src + BRGA_BPP * (h * srcWidth + w + 1)));
			pixel3 = *((u_int32_t*) (src + BRGA_BPP * ((h + 1) * srcWidth + w + 1)));
			pixel4 = *((u_int32_t*) (src + BRGA_BPP * ((h + 1) * srcWidth + w)));

			blue = (uchar) pixel1 * c1 + (uchar) pixel2 * c2
					+ (uchar) pixel3 * c3 + (uchar) pixel4 * c4;
			green = (uchar) (pixel1 >> 8) * c1
					+ (uchar) (pixel2 >> 8) * c2
					+ (uchar) (pixel3 >> 8) * c3
					+ (uchar) (pixel4 >> 8) * c4;
			red = (uchar) (pixel1 >> 16) * c1
					+ (uchar) (pixel2 >> 16) * c2
					+ (uchar) (pixel3 >> 16) * c3
					+ (uchar) (pixel4 >> 16) * c4;
			alpha = (uchar) (pixel1 >> 24) * c1
					+ (uchar) (pixel2 >> 24) * c2
					+ (uchar) (pixel3 >> 24) * c3
					+ (uchar) (pixel4 >> 24) * c4;

			u_int32_t *pixel_res = NULL;
			pixel_res = (u_int32_t*)(dest + BRGA_BPP * (i + j * destWidth));
			*pixel_res = ((u_int32_t) alpha << 24) | ((u_int32_t) red << 16)
                    								| ((u_int32_t) green << 8) | (blue);
		}
	}
	return true;
}

bool
ImageUtil::createGrayscaleImageFile(const char *fullPath, Evas_Object* windowObject)
{
	Evas *canvas = evas_object_evas_get(windowObject);
	Evas_Object *img = evas_object_image_add(canvas);

	if(NULL == img)
		return false;

	evas_object_image_file_set(img, fullPath, NULL);

	int colorSpace = 0;
	colorSpace = evas_object_image_colorspace_get(img);
	if(colorSpace != 0)
	{ //EVAS_COLORSPACE_ARGB8888
		evas_object_del(img);
		return false;
	}

	unsigned char *imgSrc = (unsigned char*)evas_object_image_data_get(img, EINA_TRUE);
	unsigned int x, y;
	int w, h, result;

	evas_object_image_size_get(img, &w, &h);

	if(h < 0)
	{
		evas_object_del(img);
		return false;
	}

	for (y = 0; y < (unsigned int)h; ++y)
	{
		unsigned char* src_pixel = &(imgSrc[y * w * 4]);
		for (x = 0; x < (unsigned int)w; ++x)
		{
			double gFixel = 0.299 * (double)src_pixel[BRGA_BPP * x+1] + 0.587 * (double)src_pixel[BRGA_BPP * x + 2] + 0.114 * (double)src_pixel[BRGA_BPP * x];
			result = (int)floor(gFixel);
			src_pixel[BRGA_BPP * x] = result;
			src_pixel[BRGA_BPP * x + 1] = result;
			src_pixel[BRGA_BPP * x + 2] = result;
			//			src_pixel[BRGA_BPP * x + 3] = RGB32_DEFAULT_ALPHA;
		}
	}

	evas_object_image_data_update_add(img, 0, 0, w, h);

	std::string grayPath(fullPath);
	grayPath += ".gray.png";

	evas_object_image_save(img, grayPath.c_str(), NULL, "quality=100 compress=0");
	evas_object_del(img);
	return true;
}

}
