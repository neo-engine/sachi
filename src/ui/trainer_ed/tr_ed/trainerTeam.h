#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/frame.h>

#include "../../../model.h"

namespace UI {
    class root;
}

namespace UI::TRE {
    /*
     * @brief: Widget to display/edit the pkmn of a trainer
     */
    class trainerTeam {
        model& _model;
        root&  _rootWindow;

        Gtk::Box _mainBox{ Gtk::Orientation::HORIZONTAL };

        // switchbox with 6 elements that displays sprites of pkmn/held items; clicking
        // entry selects and make editable a single pkmn

      public:
        trainerTeam( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        void redraw( );

        inline void show( ) {
            _mainBox.show( );
        }

        inline void hide( ) {
            _mainBox.hide( );
        }

        inline bool isVisible( ) {
            return _mainBox.is_visible( );
        }
    };
} // namespace UI::TRE
