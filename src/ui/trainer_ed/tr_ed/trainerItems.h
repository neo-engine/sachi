#pragma once
#include <memory>

#include <gtkmm/box.h>

#include "../../../model.h"
#include "../../pure/itemSelector.h"

namespace UI {
    class root;
}

namespace UI::TRE {
    /*
     * @brief: Widget to display/edit the items a trainer may use during battle.
     */
    class trainerItems {
        model& _model;
        root&  _rootWindow;

        Gtk::Box _mainBox{ Gtk::Orientation::HORIZONTAL };

        // Up to 5 items per trainer, displayed in a horizontal box
        std::shared_ptr<itemSelector> _items[ 5 ];

      public:
        trainerItems( model& p_model, root& p_root );

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
