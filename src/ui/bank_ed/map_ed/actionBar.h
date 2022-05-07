#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>

namespace UI::MED {
    /*
     * @brief: map slice navigation / map slice display setiings bar
     */
    class actionBar {
        model& _model;

        Gtk::Frame                   _mapEditorActionBar;
        std::shared_ptr<Gtk::Button> _mapNavButton[ 3 ][ 3 ];
        Gtk::SpinButton              _mapEditorSettings1;
        Gtk::SpinButton              _mapEditorSettings2;
        Gtk::SpinButton              _mapEditorSettings3;
        Gtk::SpinButton              _mapEditorSettings4;
        Gtk::SpinButton              _mapEditorSettings5;
        Gtk::SpinButton              _mapEditorSettings6;

        Gtk::Box _abEb1; // block set width settings box, contains _mapEditorSettings4

      public:
        actionBar( model& p_model );

        inline operator Gtk::Widget&( ) {
            return _mapEditorActionBar;
        }
    };
} // namespace UI::MED
