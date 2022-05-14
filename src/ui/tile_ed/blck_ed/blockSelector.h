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
    class blockSelector {
        model& _model;
        root&  _rootWindow;

        Gtk::Frame       _blockSetFrame;
        Gtk::Box         _mapEditorBlockSetBox{ Gtk::Orientation::VERTICAL };
        computedMapSlice _ts1widget, _ts2widget;

        std::shared_ptr<editableBlock> _editBlock;

      public:
        blockSelector( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _blockSetFrame;
        }

        void redraw( );

        inline void show( ) {
            _blockSetFrame.show( );
        }

        inline void hide( ) {
            _blockSetFrame.hide( );
        }

        inline bool isVisible( ) {
            return _blockSetFrame.is_visible( );
        }

        /*
         * @brief: Looks up the computed image data of the block sets.
         */
        inline std::shared_ptr<Gdk::Pixbuf> blockSetLookup( u16 p_blockIdx ) {
            if( p_blockIdx >= DATA::MAX_BLOCKS_PER_TILE_SET ) {
                p_blockIdx -= DATA::MAX_BLOCKS_PER_TILE_SET;
                return _ts2widget.getImageData( p_blockIdx );
            } else {
                return _ts1widget.getImageData( p_blockIdx );
            }
        }

        inline const DATA::computedBlock& blockDataLookup( u16 p_blockIdx ) {
            if( p_blockIdx >= DATA::MAX_BLOCKS_PER_TILE_SET ) {
                p_blockIdx -= DATA::MAX_BLOCKS_PER_TILE_SET;
                return _ts2widget.getBlockData( p_blockIdx );
            } else {
                return _ts1widget.getBlockData( p_blockIdx );
            }
        }

        void onTSClicked( mapSlice::clickType, u16 p_blockX, u16 p_blockY, u8 p_ts );

        void onBlockClicked( editableTiles::clickType p_button, u8 p_layer, u16 p_tX, u16 p_tY );
    };
} // namespace UI::TED
