#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <png.h>
#include <pnginfo.h>

#include <jni.h>
#include <android/asset_manager.h>

#include "libpng_android.h"



/* void callback_read(png_structp pStruct, */
/*         png_bytep pData, png_size_t pSize) { */

/* 	unsigned char** p = (unsigned char**)png_get_io_ptr(pStruct); */
/* 	    memcpy(pData, *p, pSize); */
/* 	    *p += (int)pSize; */
/*     } */


/* int loadPngImage(AAssetManager* mgr, char *filename, int* outWidth, int* outHeight, int* outHasAlpha, GLubyte **outData) { */

/* 	png_structp png_ptr; */
/*     png_infop info_ptr; */
/*     unsigned int sig_read = 0; */
/*     png_int_32 depth, color_type; */
/*     GLint format; */
/*     FILE *fp; */
/*     png_byte png_header[8]; */

/* 	// ファイルをオープンする */
/* 	AAsset* assetFile = AAssetManager_open(mgr, filename, AASSET_MODE_RANDOM); */
/* 	AAsset_read(mgr, png_header,sizeof(png_header)); */

/* 	if (png_sig_cmp(png_header,0,8) != 0) */
/* 		return FALSE; */

/*     png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, */
/*             NULL, NULL, NULL); */

/*     if (png_ptr == NULL) { */
/*     	AAsset_close(assetFile); */
/*        return -2;//FALSE; */
/*     } */

/*     /\* Allocate/initialize the memory */
/*      * for image information.  REQUIRED. *\/ */
/*     info_ptr = png_create_info_struct(png_ptr); */
/*     if (info_ptr == NULL) { */
/*     	AAsset_close(assetFile); */
/*         png_destroy_read_struct(&png_ptr, NULL, NULL); */
/*         return -3;//FALSE; */
/*     } */

/*     // Prepares reading operation by setting-up a read callback. */
/*     png_set_read_fn(png_ptr, , callback_read); */

/*    if (setjmp(png_jmpbuf(png_ptr))) { */
/*         /\* Free all of the memory associated */
/*          * with the png_ptr and info_ptr *\/ */
/*        png_destroy_read_struct(&png_ptr, &info_ptr, NULL); */
/*    	AAsset_close(assetFile); */
/*         /\* If we get here, we had a */
/*          * problem reading the file *\/ */
/*        return -4;//FALSE; */
/*    } */

/*     /\* Set up the output control if */
/*      * you are using standard C streams *\/ */
/* //    png_init_io(png_ptr, fp); */

/*     /\* If we have already */
/*      * read some of the signature *\/ */
/*     png_set_sig_bytes(png_ptr, 8); */
/*     png_read_info(png_ptr, info_ptr); */

/*     png_get_IHDR(png_ptr, info_ptr, outWidth, outHeight, */
/*                &depth, &color_type, NULL, NULL, NULL); */

/*     // Expands PNG with less than 8bits per channel to 8bits. */
/*     if (depth < 8) { */
/*         png_set_packing (png_ptr); */
/*     // Shrinks PNG with 16bits per color channel down to 8bits. */
/*     } else if (depth == 16) { */
/*         png_set_strip_16(png_ptr); */
/*     } */

/*     switch (color_type) { */
/*     	case PNG_COLOR_TYPE_PALETTE: */
/*     	png_set_palette_to_rgb(png_ptr); */
/*        format = GL_RGB; */
/*     	break; */

/*     	case PNG_COLOR_TYPE_RGB: */
/*     	       format = GL_RGB; */
/*     	       break; */

/*     	case PNG_COLOR_TYPE_RGBA: */
/*     	       format = GL_RGBA; */
/*     	       break; */
/*         default: */
/* //            std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl; */
/*             png_destroy_read_struct(&png_ptr, &info_ptr, NULL); */
/*         	AAsset_close(assetFile); */
/*             return -5;//FALSE; */
/*     } */
/*     // Validates all tranformations. */
/*     png_read_update_info(png_ptr, info_ptr); */

/*     unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr); */
/*     *outData = (unsigned char*) malloc(row_bytes * *outHeight); */

/*     png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr); */

/*     int i; */
/*     for (i = 0; i < *outHeight; i++) { */
/*         // note that png is ordered top to */
/*         // bottom, but OpenGL expect it bottom to top */
/*         // so the order or swapped */
/*         memcpy(*outData+(row_bytes * (*outHeight-1-i)), row_pointers[i], row_bytes); */
/*     } */

/*     /\* Clean up after the read, */
/*      * and free any memory allocated *\/ */
/*     png_destroy_read_struct(&png_ptr, &info_ptr, NULL); */

/*     /\* Close the file *\/ */
/* 	AAsset_close(assetFile); */

/*     /\* That's it *\/ */
/*     return TRUE; */
/* } */


int offset = 0;

void callback_read(png_structp pPng,
        png_bytep buf, png_size_t size) {

	u_char* p = (u_char*)png_get_io_ptr(pPng);
	    memcpy(buf, p+offset, size);
	    offset += size;
    }

int loadPngImage(AAssetManager* mgr, char *filename, png_uint_32* outWidth, png_uint_32* outHeight, GLint *type, u_char **outData) {

	png_structp png_ptr;
    png_infop info_ptr;
    int depth;
    int color_type;

	// ファイルをオープンする
	AAsset* assetFile = AAssetManager_open(mgr, filename, AASSET_MODE_RANDOM);

    int size = AAsset_getLength(assetFile);
    u_char* buf = (u_char*)malloc(size);
    AAsset_read(assetFile, buf, size);
    AAsset_close(assetFile);
    
    if (png_sig_cmp(buf,0,8) != 0)
		return FALSE;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
            NULL, NULL, NULL);

    if (png_ptr == NULL) {

      if (buf){free(buf);buf=NULL;}
      
      return FALSE;
    }

    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
      if (buf){free(buf);buf=NULL;}
        return FALSE;
    }

    // Prepares reading operation by setting-up a read callback.
    png_set_read_fn(png_ptr, buf, callback_read);
    offset = 8;

   if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
       png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        /* If we get here, we had a
         * problem reading the file */
      if (buf){free(buf);buf=NULL;}
       return FALSE;
   }

    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, 8);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    *outWidth = info_ptr->width;
    *outHeight = info_ptr->height;
    color_type = info_ptr->color_type;
    depth = info_ptr->bit_depth;

//    // Expands PNG with less than 8bits per channel to 8bits.
//    if (depth < 8) {
//        png_set_packing (png_ptr);
//    // Shrinks PNG with 16bits per color channel down to 8bits.
//    } else if (depth == 16) {
//        png_set_strip_16(png_ptr);
//    }

    switch (color_type) {
    	case PNG_COLOR_TYPE_PALETTE:
    	png_set_palette_to_rgb(png_ptr);
       *type = GL_RGB;
    	break;

    	case PNG_COLOR_TYPE_RGB:
    		*type = GL_RGB;
    	       break;

    	case PNG_COLOR_TYPE_RGBA:
    		*type = GL_RGBA;
    	       break;
        default:
//            std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      if (buf){free(buf);buf=NULL;}
            return FALSE;
    }
    // Validates all tranformations.
//    png_read_update_info(png_ptr, info_ptr);

    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    *outData = (unsigned char*) malloc(row_bytes * *outHeight);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    int i;
    for (i = 0; i < *outHeight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(*outData+(row_bytes * (*outHeight-1-i)), row_pointers[i], row_bytes);
    }

    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    free(buf);

    /* That's it */
    return TRUE;
}
