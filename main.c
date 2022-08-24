#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "spng.h"


/* values range from 0 to 255 */
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} bitmap_pixel_t;

typedef struct {
	size_t width;
	size_t height;
	bitmap_pixel_t pixels[];
} bitmap_t;


inline bitmap_t *bitmap_new( size_t width, size_t height ) {
	bitmap_t *bm = malloc( sizeof(bitmap_t) + sizeof(bitmap_pixel_t) * width * height );
	bm->width = width;
	bm->height = height;
	return bm;
}

inline void bitmap_free( bitmap_t *bitmap ) {
	free( bitmap );
}

void bitmap_to_file( bitmap_t const *bitmap, char const *path ) {
	FILE *fp;
	if ( fopen_s( &fp, path, "w" ) )
		exit( EXIT_FAILURE );

	spng_ctx *ctx = spng_ctx_new( SPNG_CTX_ENCODER );
	spng_set_png_file( ctx, fp );

	/* set png header */
	struct spng_ihdr ihdr = {
		.width = bitmap->width,
		.height = bitmap->height,
		.bit_depth = 8,
		.color_type = SPNG_COLOR_TYPE_TRUECOLOR_ALPHA
	};

	spng_set_ihdr( ctx, &ihdr );

	/* encode bitmap data */
	int error = spng_encode_image( ctx, &bitmap->pixels, sizeof(bitmap_pixel_t) * bitmap->width * bitmap->height, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE );
	if ( error ) {
		printf( "spng_encode_image() error: %s\n", spng_strerror(error) );
		exit( EXIT_FAILURE );
	}

	/* cleanup */
	spng_ctx_free( ctx );

	fclose( fp );
}


int main() {
	bitmap_t *bm = bitmap_new( 75, 50 );

	for ( size_t y = 0; y < bm->height; ++y ) {
		for ( size_t x = 0; x < bm->width; ++x ) {
			bitmap_pixel_t *px = &bm->pixels[ bm->width * y + x ];
			px->r = 255;
			px->g = 0;
			px->b = 0;
			px->a = 255;
		}
	}

	bitmap_to_file( bm, "test.png" );
	bitmap_free( bm );

	return EXIT_SUCCESS;
}
