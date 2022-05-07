#pragma once
#include <functional>
#include <memory>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>

namespace UI::BOV {
    /*
     * @brief: Main widget of the bank editor.
     */
    class actionBar {
        static constexpr u8 NUM_SETTINGS    = 3;
        static constexpr u8 SETTING_SPACING = 0;
        static constexpr u8 SETTING_SCALE   = 1;
        static constexpr u8 SETTING_DAYTIME = 2;

        Gtk::Frame _outerFrame;

        Gtk::SpinButton _settings[ NUM_SETTINGS ];

        u8 _settingValue[ NUM_SETTINGS ] = { 2, 3, 0 };

      public:
        actionBar( );

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }

        void setCurrentDaytime( u8 p_daytime, bool p_propagate = false );

        inline void connect( u8 p_setting, std::function<void( u8 )> p_callback ) {
            _settings[ p_setting ].signal_value_changed( ).connect( [ & ]( ) {
                auto value = _settings[ p_setting ].get_value_as_int( );
                p_callback( value );
            } );
        }
    };
} // namespace UI::BOV
