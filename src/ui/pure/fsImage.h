#pragma once
#include <gtkmm/image.h>
#include <gtkmm/widget.h>

#include "../../data/bitmap.h"
#include "../../defines.h"

namespace UI {
    enum class imageType {
        IT_SPRITE_ICON_16x16, // vanilla sprite/icon
        IT_SPRITE_ICON_32x32, // vanilla sprite/icon
        IT_SPRITE_ICON_64x64, // vanilla sprite/icon
        IT_SPRITE_PKMN,       // multi-part pkmn sprite
        // IT_SPRITE_ANIMATED,   // animated rsd
        IT_SPRITE_PLATFORM, // multi-part platform
        IT_BG_IMAGE,        // 256x192 bg image
    };

    /*
     * @brief: A widget to display a single gfx object from FSROOT
     */
    template <imageType t_type>
    class fsImage : public Gtk::Widget {
      public:
      protected:
        Gtk::Image                   _image;
        u16                          _scale = 1;
        std::shared_ptr<Gdk::Pixbuf> _data;

        u16 _cropx = 0, _cropy = 0;

      public:
        inline fsImage( ) {
            _image.set_parent( *this );
        }

        virtual inline ~fsImage( ) {
            _image.unparent( );
        }

        inline void load( const std::string& p_path, u16 p_cx = 0, u16 p_cy = 0, u16 p_cw = 0,
                          u16 p_ch = 0 ) {
            if( t_type == imageType::IT_BG_IMAGE ) {
                auto btm = DATA::bitmap::fromBGImage( p_path.c_str( ) );
                if( p_ch && p_cw ) {
                    btm.crop( p_cx, p_cy, p_cw, p_ch );
                    _cropx = 256 - p_cw;
                    _cropy = 192 - p_ch;
                }
                _data = btm.pixbuf( );
                _image.set( _data );
                return;
            }
            if( t_type == imageType::IT_SPRITE_PLATFORM ) {
                auto btm = DATA::bitmap::fromPlatformSprite( p_path.c_str( ) );
                if( p_ch && p_cw ) {
                    btm.crop( p_cx, p_cy, p_cw, p_ch );
                    _cropx = 128 - p_cw;
                    _cropy = 64 - p_ch;
                }
                _data = btm.pixbuf( );
                _image.set( _data );
                return;
            }
        }

        inline u16 getWidth( ) const {
            switch( t_type ) {
            case imageType::IT_SPRITE_ICON_16x16: return 16 - _cropx;
            case imageType::IT_SPRITE_ICON_32x32: return 32 - _cropx;
            case imageType::IT_SPRITE_ICON_64x64: return 64 - _cropx;
            case imageType::IT_SPRITE_PKMN: return 96 - _cropx;
            case imageType::IT_SPRITE_PLATFORM: return 128 - _cropx;
            case imageType::IT_BG_IMAGE: return 256 - _cropx;
            }
            return 0;
        }

        inline u16 getHeight( ) const {
            switch( t_type ) {
            case imageType::IT_SPRITE_ICON_16x16: return 16 - _cropy;
            case imageType::IT_SPRITE_ICON_32x32: return 32 - _cropy;
            case imageType::IT_SPRITE_ICON_64x64: return 64 - _cropy;
            case imageType::IT_SPRITE_PKMN: return 96 - _cropy;
            case imageType::IT_SPRITE_PLATFORM: return 64 - _cropy;
            case imageType::IT_BG_IMAGE: return 192 - _cropy;
            }
            return 0;
        }

        inline void setScale( u16 p_scale = 1 ) {
            if( p_scale ) { _scale = p_scale; }
        }

      protected:
        inline Gtk::SizeRequestMode get_request_mode_vfunc( ) const override {
            return Gtk::SizeRequestMode::CONSTANT_SIZE;
        }

        inline void measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum,
                                   int& p_natural, int& p_minimumBaseline,
                                   int& p_naturalBaseline ) const override {
            p_minimumBaseline = -1;
            p_naturalBaseline = -1;

            if( p_orientation == Gtk::Orientation::HORIZONTAL ) {
                p_minimum = getWidth( ) * _scale;
                p_natural = getWidth( ) * _scale;
            } else {
                p_minimum = getHeight( ) * _scale;
                p_natural = getHeight( ) * _scale;
            }
        }

        inline void size_allocate_vfunc( int, int, int p_baseline ) override {
            // make dummy calls to measure to suppress warnings (yes we do know how big
            // every block should be.)
            int ignore;
            _image.measure( Gtk::Orientation::HORIZONTAL, -1, ignore, ignore, ignore, ignore );

            Gtk::Allocation allo;
            allo.set_x( 0 );
            allo.set_y( 0 );
            auto width  = _scale * getWidth( );
            auto height = _scale * getHeight( );
            allo.set_width( width );
            allo.set_height( height );
            _image.size_allocate( allo, p_baseline );
        }
    };
} // namespace UI
