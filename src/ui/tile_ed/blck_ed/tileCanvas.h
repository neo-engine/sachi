#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/frame.h>

#include "../../../data/maprender.h"
#include "../../../model.h"
#include "../../pure/editableBlock.h"
#include "../../pure/mapSlice.h"

namespace UI {
    class root;
}

namespace UI::TED {
    /*
     * @brief: Widget to edit a single tile
     */
    class tileCanvas {
        model& _model;
        root&  _rootWindow;

        Gtk::Frame _frame;
        Gtk::Box   _mainBox{ Gtk::Orientation::HORIZONTAL };
        tileSlice  _tile;
        colorSlice _pal;

      public:
        tileCanvas( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _frame;
        }

        void redraw( );

        inline void show( ) {
            _frame.show( );
        }

        inline void hide( ) {
            _frame.hide( );
        }

        inline bool isVisible( ) {
            return _frame.is_visible( );
        }

        void onTileClicked( mapSlice::clickType, u16 p_tileX, u16 p_tileY );

        void onPalClicked( mapSlice::clickType, u16 p_tileX, u16 p_tileY );
    };
} // namespace UI::TED
