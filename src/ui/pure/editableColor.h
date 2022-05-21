#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "../../defines.h"

namespace UI {
    /*
     * @brief: Widget for displaying a color and easy controls to change said color.
     */
    class editableColor {
        Gtk::Frame       _mainFrame;
        Gtk::ColorButton _colorButton;
        Gtk::Entry       _entry;
        std::string      _label;

        u16  _color = 0;
        bool _lock  = false;

      public:
        editableColor( std::string p_label );

        void connect( const std::function<void( u16 )>& p_colorChangedCallback );

        inline operator Gtk::Widget&( ) {
            return _mainFrame;
        }

        inline u16 get( ) const {
            return _color;
        }
        void set( u16 p_color );
        void set( u8 p_r, u8 p_g, u8 p_b ) {
            set( ( p_r & 31 ) | ( ( p_g & 31 ) << 5 ) | ( ( p_b & 31 ) << 10 ) );
        }
    };
} // namespace UI
