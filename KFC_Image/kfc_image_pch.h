#ifndef kfc_image_pch_h
#define kfc_image_pch_h

#include <KFC_KTL/kfc_ktl_pch.h>
#include <KFC_Common/kfc_common_pch.h>

#ifdef _MSC_VER
	#define XMD_H
	#undef  FAR
	#include <jpeglib.h>
#else // _MSC_VER
	#define XMD_H
	extern "C"
	{
		#include <jpeglib.h>
	}
#endif // _MSC_VER

#include <png.h>
#include <tiffio.h>

#endif // kfc_image_pch_h
