#pragma once
#include <optional>
#include <vector>
#include <gtkmm/image.h>

#include "../defines.h"

namespace DATA {
    /**
     * @brief Struct to store the RGB values of a pixel.
     */
    struct pixel {
        u8 m_red;               ///< R value
        u8 m_green;             ///< G value
        u8 m_blue;              ///< B value
        u8 m_transparent = 255; ///< A value

        constexpr pixel( u8 p_red, u8 p_green, u8 p_blue, u8 p_transparent = 255 )
            : m_red( p_red ), m_green( p_green ), m_blue( p_blue ), m_transparent( p_transparent ) {
        }

        constexpr auto operator<=>( const pixel& p_other ) const = default;

        constexpr pixel operator^=( const pixel& p_other ) {
            u16 tp        = 510 - ( this->m_transparent + p_other.m_transparent );
            u16 ttp       = 255 - ( this->m_transparent );
            u16 otp       = 255 - ( p_other.m_transparent );
            this->m_red   = this->m_red * ttp / tp + p_other.m_red * otp / tp;
            this->m_green = this->m_green * ttp / tp + p_other.m_green * otp / tp;
            this->m_blue  = this->m_blue * ttp / tp + p_other.m_blue * otp / tp;
            return *this;
        }

        constexpr pixel operator&=( const pixel& p_other ) {
            u16 tp        = 255;
            u16 otp       = 255 - ( p_other.m_transparent );
            u16 ttp       = p_other.m_transparent;
            this->m_red   = this->m_red * ttp / tp + p_other.m_red * otp / tp;
            this->m_green = this->m_green * ttp / tp + p_other.m_green * otp / tp;
            this->m_blue  = this->m_blue * ttp / tp + p_other.m_blue * otp / tp;
            return *this;
        }

        constexpr pixel operator+=( const pixel& p_other ) {
            this->m_red += p_other.m_red;
            this->m_green += p_other.m_green;
            this->m_blue += p_other.m_blue;
            return *this;
        }

        constexpr pixel operator-=( const pixel& p_other ) {
            this->m_red -= p_other.m_red;
            this->m_green -= p_other.m_green;
            this->m_blue -= p_other.m_blue;
            return *this;
        }

        constexpr pixel operator*=( u8 p_scale ) {
            this->m_red *= p_scale;
            this->m_green *= p_scale;
            this->m_blue *= p_scale;
            return *this;
        }

        constexpr pixel operator/=( u8 p_scale ) {
            this->m_red /= p_scale;
            this->m_green /= p_scale;
            this->m_blue /= p_scale;
            return *this;
        }

        friend constexpr pixel operator^( pixel p_lhs, const pixel& p_rhs ) {
            p_lhs ^= p_rhs;
            return p_lhs;
        }

        friend constexpr pixel operator&( pixel p_lhs, const pixel& p_rhs ) {
            p_lhs &= p_rhs;
            return p_lhs;
        }

        friend constexpr pixel operator+( pixel p_lhs, const pixel& p_rhs ) {
            p_lhs += p_rhs;
            return p_lhs;
        }

        friend constexpr pixel operator-( pixel p_lhs, const pixel& p_rhs ) {
            p_lhs -= p_rhs;
            return p_lhs;
        }

        friend constexpr pixel operator*( pixel p_lhs, u8 p_rhs ) {
            p_lhs *= p_rhs;
            return p_lhs;
        }

        friend constexpr pixel operator/( pixel p_lhs, u8 p_rhs ) {
            p_lhs /= p_rhs;
            return p_lhs;
        }
    };

#define SCALE( x, mx ) \
    ( ( ( x ) < 0 ) ? 0 : ( (int) ( 256.0 * ( (double) ( x ) / (double) ( mx ) ) ) ) )

    /**
     * @brief Struct to hold a bitmap.
     */
    struct bitmap {
      private:
        std::vector<std::vector<pixel>> m_pixels; ///< The raw data

        void addFromSprite( unsigned* p_imgData, unsigned short* p_palData, size_t p_width,
                            size_t p_height, size_t p_x = 0, size_t p_y = 0 );

      public:
        size_t m_width;  ///< The image width
        size_t m_height; ///< The image height
        bitmap( size_t p_width, size_t p_height );
        bitmap( const char* p_path );

        void crop( u16 p_cx, u16 p_cy, u16 p_cw, u16 p_ch );

        pixel& operator( )( size_t p_x, size_t p_y );
        pixel  operator( )( size_t p_x, size_t p_y ) const;

        /**
         * @param p_path String containing the path to the file to write the file to
         * @return Non-zero if an error occured.
         */
        int writeToFile( const char* p_path ) const;

        std::shared_ptr<Gdk::Pixbuf> pixbuf( ) const;

        void updatePixbuf( std::shared_ptr<Gdk::Pixbuf>& p_out ) const;

        static bitmap fromPkmnSprite( const char* p_path );

        static bitmap fromBGImage( const char* p_path );

        static bitmap fromPlatformSprite( const char* p_path );

        static bitmap fromSprite( const char* p_path, size_t p_width, size_t p_height );
    };
} // namespace DATA
