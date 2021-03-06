#pragma once
#include <gtkmm/frame.h>

#include "../../../model.h"
#include "../../pure/mapSlice.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: Widget to select a movement
     */
    class movementSelector {
        model& _model;
        root&  _rootWindow;

        Gtk::Frame       _movementFrame;
        computedMapSlice _movementWidget;

      public:
        movementSelector( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _movementFrame;
        }

        void redraw( );

        inline void hide( ) {
            _movementFrame.hide( );
        }

        inline void show( ) {
            _movementFrame.show( );
        }

        void updateSelection( );

        inline bool isVisible( ) {
            return _movementFrame.is_visible( );
        }
    };
} // namespace UI::MED
