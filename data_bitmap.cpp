#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <png.h>

#include "data_bitmap.h"

namespace DATA {

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Bitmap
    //
    ///////////////////////////////////////////////////////////////////////////////

    bitmap::bitmap( size_t p_width, size_t p_height ) {
        m_width  = p_width;
        m_height = p_height;
        m_pixels = std::vector<std::vector<pixel>>( p_width,
                                                    std::vector<pixel>( p_height, { 0, 0, 0 } ) );
    }

    bitmap::bitmap( const char* p_path ) {
        static png_structp pngPtr;
        static png_infop   infoPtr;

        FILE* fd = std::fopen( p_path, "r" );
        if( !fd ) {
            std::fprintf( stderr, "[ERROR] Reading PNG at %s failed. (BAD FILE PATH)\n", p_path );
            return;
        }

        unsigned char sig[ 8 ];
        std::fread( sig, 1, 8, fd );
        if( !png_check_sig( sig, 8 ) ) {
            std::fprintf( stderr, "[ERROR] Reading PNG at %s failed. (BAD SIGNATURE)\n", p_path );
            return;
        }

        pngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
        if( !pngPtr ) {
            std::fprintf( stderr, "[ERROR] Reading PNG at %s failed. (OUT OF MEMORY)\n", p_path );
            return;
        }

        infoPtr = png_create_info_struct( pngPtr );
        if( !infoPtr ) {
            png_destroy_read_struct( &pngPtr, NULL, NULL );
            std::fprintf( stderr, "[ERROR] Reading PNG at %s failed. (OUT OF MEMORY)\n", p_path );
            return;
        }

        if( setjmp( png_jmpbuf( pngPtr ) ) ) {
            png_destroy_read_struct( &pngPtr, &infoPtr, NULL );
            std::fprintf( stderr, "[ERROR] Reading PNG at %s failed. (SETJMP TRIGGERED)\n",
                          p_path );
            return;
        }

        png_init_io( pngPtr, fd );
        png_set_sig_bytes( pngPtr, 8 );
        png_read_info( pngPtr, infoPtr );

        m_width          = png_get_image_width( pngPtr, infoPtr );
        m_height         = png_get_image_height( pngPtr, infoPtr );
        png_byte clrType = png_get_color_type( pngPtr, infoPtr );
        png_byte bitDpt  = png_get_bit_depth( pngPtr, infoPtr );

        if( bitDpt == 16 ) png_set_strip_16( pngPtr );

        if( clrType == PNG_COLOR_TYPE_PALETTE ) png_set_strip_16( pngPtr );

        if( clrType == PNG_COLOR_TYPE_GRAY && bitDpt < 8 ) png_set_expand_gray_1_2_4_to_8( pngPtr );

        if( png_get_valid( pngPtr, infoPtr, PNG_INFO_tRNS ) ) png_set_tRNS_to_alpha( pngPtr );

        if( clrType == PNG_COLOR_TYPE_RGB || clrType == PNG_COLOR_TYPE_GRAY
            || clrType == PNG_COLOR_TYPE_PALETTE )
            png_set_filler( pngPtr, 0xFF, PNG_FILLER_AFTER );

        if( clrType == PNG_COLOR_TYPE_GRAY || clrType == PNG_COLOR_TYPE_GRAY_ALPHA )
            png_set_gray_to_rgb( pngPtr );

        png_read_update_info( pngPtr, infoPtr );

        png_bytep* row_pointers = (png_bytep*) malloc( sizeof( png_bytep ) * m_height );
        for( size_t y = 0; y < m_height; y++ )
            row_pointers[ y ] = (png_byte*) malloc( png_get_rowbytes( pngPtr, infoPtr ) );

        png_read_image( pngPtr, row_pointers );

        fclose( fd );

        m_pixels = std::vector<std::vector<pixel>>( m_width,
                                                    std::vector<pixel>( m_height, { 0, 0, 0 } ) );

        for( size_t y = 0; y < m_height; y++ ) {
            png_bytep row = row_pointers[ y ];
            for( size_t x = 0; x < m_width; x++ ) {
                png_bytep px = &( row[ x * 4 ] );
                if( px[ 3 ] )
                    m_pixels[ x ][ y ] = { px[ 0 ], px[ 1 ], px[ 2 ], 0 };
                else
                    m_pixels[ x ][ y ] = { 0, 0, 0, 1 };
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Bitmap - Operator overloadings
    ///////////////////////////////////////////////////////////////////////////////

    pixel& bitmap::operator( )( size_t p_x, size_t p_y ) {
        if( p_x >= m_width || p_y >= m_height ) {
            std::fprintf( stderr,
                          "[ERROR] Index out of range while trying to get pixel at ( %lu|%lu )!\n"
                          "Returning default pixel value instead.",
                          p_x, p_y );
            static pixel defPixel = { 0, 0, 0 };
            return defPixel;
        }
        return m_pixels[ p_x ][ p_y ];
    }
    pixel bitmap::operator( )( size_t p_x, size_t p_y ) const {
        if( p_x >= m_width || p_y >= m_height ) {
            std::fprintf( stderr,
                          "[ERROR] Index out of range while trying to get pixel at ( %lu|%lu )!\n"
                          "Returning default pixel value instead.",
                          p_x, p_y );
            static pixel defPixel = { 0, 0, 0 };
            return defPixel;
        }
        return m_pixels[ p_x ][ p_y ];
    }

    std::shared_ptr<Gdk::Pixbuf> bitmap::pixbuf( ) const {
        u8* linbuffer = new u8[ m_height * m_width * 3 + 10 ];
        u16 cnt       = 0;
        for( u16 y = 0; y < m_height; ++y ) {
            for( u16 x = 0; x < m_width; ++x ) {
                auto px            = m_pixels[ x ][ y ];
                linbuffer[ cnt++ ] = px.m_red;
                linbuffer[ cnt++ ] = px.m_green;
                linbuffer[ cnt++ ] = px.m_blue;
            }
        }
        auto res = Gdk::Pixbuf::create_from_data( linbuffer, Gdk::Colorspace::RGB, false, 8,
                                                  m_width, m_height, m_width * 3 );
        return res;
    }

    void bitmap::updatePixbuf( std::shared_ptr<Gdk::Pixbuf>& p_out ) const {
        if( m_width != u32( p_out->get_width( ) ) || m_height != u32( p_out->get_height( ) ) ) {
            fprintf( stderr, "[ERROR] PixBuf has wrong dimensions.\n" );
            return;
        }
        auto data = p_out->get_pixels( );
        u32  cnt  = 0;
        for( auto row : m_pixels ) {
            for( auto p : row ) {
                data[ cnt++ ] = p.m_red;
                data[ cnt++ ] = p.m_green;
                data[ cnt++ ] = p.m_blue;
                data[ cnt++ ] = p.m_transparent;
            }
        }
    }

    int bitmap::writeToFile( const char* p_path ) const {
        FILE*       fd;
        png_structp pngPtr  = NULL;
        png_infop   infoPtr = NULL;
        size_t      x, y;
        png_byte**  rowPointers = NULL;

        int status = -1;

        int pixelSize = 3, depth = 8;

        fd = std::fopen( p_path, "wb" );
        if( !fd ) {
            std::fprintf( stderr, "[ERROR] Could not open file %s for writing.\n", p_path );
            goto FOPEN_FAILED;
        }

        pngPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
        if( pngPtr == NULL ) {
            std::fprintf( stderr, "[ERROR] Could not create png write struct.\n" );
            status = -2;
            goto PNG_CREATE_WRITE_STRUCT_FAILED;
        }

        infoPtr = png_create_info_struct( pngPtr );
        if( pngPtr == NULL ) {
            std::fprintf( stderr, "[ERROR] Could not create png info struct.\n" );
            status = -3;
            goto PNG_CREATE_INFO_STRUCT_FAILED;
        }

        if( setjmp( png_jmpbuf( pngPtr ) ) ) {
            std::fprintf( stderr, "[ERROR] Png failure\n" );

            status = -4;
            goto PNG_FAILURE;
        }

        // Set image attributes

        png_set_IHDR( pngPtr, infoPtr, m_width, m_height, depth, PNG_COLOR_TYPE_RGB,
                      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

        // Initialize rows of png

        rowPointers = (png_byte**) png_malloc( pngPtr, m_height * sizeof( png_bytep ) );
        for( y = 0; y < m_height; ++y ) {
            png_byte* row    = (png_byte*) png_malloc( pngPtr, m_width * pixelSize );
            rowPointers[ y ] = row;
            for( x = 0; x < m_width; ++x ) {
                auto px = operator( )( x, y );
                *row++  = px.m_red;
                *row++  = px.m_green;
                *row++  = px.m_blue;
            }
        }

        // Write data to fd

        png_init_io( pngPtr, fd );
        png_set_rows( pngPtr, infoPtr, rowPointers );
        png_write_png( pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL );

        status = 0;

        for( y = 0; y < m_height; ++y ) png_free( pngPtr, rowPointers[ y ] );
        png_free( pngPtr, rowPointers );

    PNG_FAILURE:
    PNG_CREATE_INFO_STRUCT_FAILED:
        png_destroy_write_struct( &pngPtr, &infoPtr );
    PNG_CREATE_WRITE_STRUCT_FAILED:
        fclose( fd );
    FOPEN_FAILED:
        return status;
    }
} // namespace DATA
