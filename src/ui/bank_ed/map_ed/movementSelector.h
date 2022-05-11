#pragma once
#include <gtkmm/frame.h>

#include "../../data/maprender.h"
#include "../editableBlock.h"
#include "mapSlice.h"

namespace UI::MED {
    /*
     * @brief: Widget to select a movement
     */
    class movementSelector {
        Gtk::Frame       _movementFrame;
        computedMapSlice _movementWidget;

      public:
        movementSelector( );

        inline operator Gtk::Widget&( ) {
            return _movementFrame;
        }

        inline void hide( ) {
            _movementFrame.hide( );
        }

        inline void show( ) {
            _movementFrame.show( );
        }

        inline bool isVisible( ) {
            return _movementFrame.is_visible( );
        }
    };
} // namespace UI::MED
