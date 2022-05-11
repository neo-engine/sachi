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

namespace UI::MED {
    /*
     * @brief: map slice navigation / map slice display setiings bar
     */
    class actionBar {
        model&                    _model;
        root&                     _rootWindow;
        mapEditor::mapDisplayMode _currentMode;

        Gtk::Frame                   _mapEditorActionBar;
        std::shared_ptr<Gtk::Button> _mapNavButton[ 3 ][ 3 ];
        Gtk::SpinButton              _mapEditorSettings1;
        Gtk::SpinButton              _mapEditorSettings2;
        Gtk::SpinButton              _mapEditorSettings3;
        Gtk::SpinButton              _mapEditorSettings4;
        Gtk::SpinButton              _mapEditorSettings5;
        Gtk::SpinButton              _mapEditorSettings6;

        Gtk::Box _abSb1; // map grid settings box, contains _mapEditorSettings1
        Gtk::Box _abSb2; // scale settings box, contains _mapEditorSettings2
        Gtk::Box _abSb3; // daytime settings box, contains _mapEditorSettings3
        Gtk::Box _abEb1; // block set width settings box, contains _mapEditorSettings4
        Gtk::Box _abEb2; // num adjacent blocks settings box, contains _mapEditorSettings5
        Gtk::Box _abEb3; // neighbor spacing settings box, contains _mapEditorSettings6

      public:
        actionBar( model& p_model, root& p_rootWindow );

        inline operator Gtk::Widget&( ) {
            return _mapEditorActionBar;
        }

        void show( );
        void hide( );
        bool isVisible( );

        void setNewMapEditMode( mapEditor::mapDisplayMode p_newMode );

        void redraw( );
    };
} // namespace UI::MED
