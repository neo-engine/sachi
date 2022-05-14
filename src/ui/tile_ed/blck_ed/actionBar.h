#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>

#include "../../../model.h"
#include "../blockEditor.h"

namespace UI {
    class root;
}

namespace UI::TED {
    class actionBar {
        model&                      _model;
        root&                       _rootWindow;
        blockEditor::tseDisplayMode _currentMode;

        Gtk::Frame      _actionBar;
        Gtk::SpinButton _setting1;
        Gtk::SpinButton _setting2;
        Gtk::SpinButton _setting3;
        Gtk::SpinButton _setting4;
        Gtk::SpinButton _setting5;

        Gtk::Box _abSlBO1; // map grid settings box, contains _setting1
        Gtk::Box _abSlBO2; // scale settings box, contains _setting2
        Gtk::Box _abSlBO3; // daytime settings box, contains _setting3
        Gtk::Box _abSlBO4; // block set wd settings box, contains _setting4
        Gtk::Box _abSlBO5; // tile set wd settings box, contains _setting5

      public:
        actionBar( model& p_model, root& p_rootWindow );

        inline operator Gtk::Widget&( ) {
            return _actionBar;
        }

        void show( );
        void hide( );
        bool isVisible( );

        void redraw( );

        void setNewEditMode( blockEditor::tseDisplayMode p_newMode );
    };
} // namespace UI::TED
