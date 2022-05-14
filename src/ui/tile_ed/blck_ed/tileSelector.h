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
     * @brief: Widget to select a block from a blockset
     */
    class tileSelector {
        model& _model;
        root&  _rootWindow;

        Gtk::Frame      _tileSetFrame;
        Gtk::Box        _mainBox{ Gtk::Orientation::VERTICAL };
        tileSetMapSlice _ts1widget, _ts2widget;

        std::shared_ptr<tileInfo> _tileInfo;

      public:
        tileSelector( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _tileSetFrame;
        }

        void redraw( );

        inline void show( ) {
            _tileSetFrame.show( );
        }

        inline void hide( ) {
            _tileSetFrame.hide( );
        }

        inline bool isVisible( ) {
            return _tileSetFrame.is_visible( );
        }

        inline const DATA::tile& tileDataLookup( u16 p_tileIdx ) {
            if( p_tileIdx >= DATA::MAX_TILES_PER_TILE_SET ) {
                p_tileIdx -= DATA::MAX_TILES_PER_TILE_SET;
                return _ts2widget.getTileData( p_tileIdx );
            } else {
                return _ts1widget.getTileData( p_tileIdx );
            }
        }

        void onTSClicked( mapSlice::clickType, u16 p_tileX, u16 p_tileY, u8 p_ts );
    };
} // namespace UI::TED
