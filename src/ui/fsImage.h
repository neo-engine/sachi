#pragma once

#include <functional>

#include <gtkmm/checkbutton.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/frame.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/widget.h>

#include "../data/maprender.h"
#include "../defines.h"

namespace UI {
    /*
     * @brief: A widget to display a single gfx object from FSROOT
     */
    class fsImage : public Gtk::Widget {
      public:
        enum imageType {
            IT_SPRITE_ICON,     // vanilla sprite/icon
            IT_SPRITE_PKMN,     // multi-part pkmn sprite
            IT_SPRITE_ANIMATED, // animated rsd
            IT_BG_IMAGE,        // 256x192 bg image
        };

      protected:
        std::shared_ptr<Gtk::Image> _image;
        u16                         _scale = 2;

      public:
        fsImage( );

        virtual inline ~fsImage( ) {
            _image->unparent( );
        }

        void load( imageType p_imType, const std::string& p_path );

        void setScale( u16 p_scale = 1 );

      protected:
        Gtk::SizeRequestMode get_request_mode_vfunc( ) const override;
        void measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum, int& p_natural,
                            int& p_minimumBaseline, int& p_naturalBaseline ) const override;
        void size_allocate_vfunc( int p_width, int p_height, int p_baseline ) override;
    };
} // namespace UI
