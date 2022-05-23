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

      public:
        inline fsImage( ) {
            _image.set_parent( *this );
        }

        virtual inline ~fsImage( ) {
            _image.unparent( );
        }

        inline void load( const std::string& p_path ) {
            if( t_type == imageType::IT_BG_IMAGE ) {
                auto btm = DATA::bitmap::fromBGImage( p_path.c_str( ) );
                _data    = btm->pixbuf( );
                delete btm;
                _image.set( _data );
            }
        }

        inline u16 getWidth( ) const {
            switch( t_type ) {
            case imageType::IT_SPRITE_ICON_16x16: return 16;
            case imageType::IT_SPRITE_ICON_32x32: return 32;
            case imageType::IT_SPRITE_ICON_64x64: return 64;
            case imageType::IT_SPRITE_PKMN: return 96;
            case imageType::IT_SPRITE_PLATFORM: return 128;
            case imageType::IT_BG_IMAGE: return 256;
            }
            return 0;
        }

        inline u16 getHeight( ) const {
            switch( t_type ) {
            case imageType::IT_SPRITE_ICON_16x16: return 16;
            case imageType::IT_SPRITE_ICON_32x32: return 32;
            case imageType::IT_SPRITE_ICON_64x64: return 64;
            case imageType::IT_SPRITE_PKMN: return 96;
            case imageType::IT_SPRITE_PLATFORM: return 32;
            case imageType::IT_BG_IMAGE: return 192;
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
