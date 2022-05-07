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
    };
} // namespace UI::MED
