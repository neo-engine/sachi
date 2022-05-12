#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>

#include "../../../model.h"
#include "../mapEditor.h"

namespace UI {
    class root;
}

namespace UI::BOV {
    /*
     * @brief: map slice navigation / map slice display setiings bar
     */
    class actionBar {
        model& _model;
        root&  _rootWindow;

        Gtk::Frame      _mapBankOverviewActionBar;
        Gtk::SpinButton _mapBankOverviewSettings1;
        Gtk::SpinButton _mapBankOverviewSettings2;
        Gtk::SpinButton _mapBankOverviewSettings3;

        Gtk::Box _abSlBO1; // map grid settings box, contains _mapBankOverviewSettings1
        Gtk::Box _abSlBO2; // scale settings box, contains _mapBankOverviewSettings2
        Gtk::Box _abSlBO3; // daytime settings box, contains _mapBankOverviewSettings3

      public:
        actionBar( model& p_model, root& p_rootWindow );

        inline operator Gtk::Widget&( ) {
            return _mapBankOverviewActionBar;
        }

        void show( );
        void hide( );
        bool isVisible( );

        void redraw( );
    };
} // namespace UI::MED
