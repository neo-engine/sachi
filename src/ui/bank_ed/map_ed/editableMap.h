#pragma once
#include <memory>
#include <tuple>
#include <vector>

#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>

#include "../../../data/maprender.h"
#include "../../../model.h"
#include "../editableBlock.h"
#include "../mapEditor.h"
#include "mapBankOverview.h"
#include "mapSlice.h"

#include "blockStamp.h"

namespace UI::MED {
    /*
     * @brief: Widget to edit a single map slice
     */
    class editableMap {
        model& _model;

        mapEditor::mapDisplayMode _currentMapDisplayMode;

        std::shared_ptr<blockStamp> _blockStamp;

        std::tuple<u16, u16, s8, s8> _dragStart;
        std::tuple<s16, s16>         _dragLast;

        Gtk::ScrolledWindow _mainWindow;
        Gtk::Grid           _mapGrid; // contains the actual maps

        void onMapDragStart( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                             s8 p_mapX, s8 p_mapY, bool p_allowEdit = true );
        void onMapDragUpdate( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                              s8 p_mapY, bool p_allowEdit = true );
        void onMapDragEnd( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

        /*
         * @brief: Handles clicks on maps:
         * Left: Change block to currently selected block (if p_allowChange)
         * Middle: Change block and recursively all adjacent blocks that are the same as
         * the original block (flood fill)
         * Right: Select block
         */
        void onMapClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

      public:
        editableMap( model& p_model );

        inline operator Gtk::Widget&( ) {
            return _mainWindow;
        }

        void setNewMapEditMode( mapEditor::mapDisplayMode p_newMode );

        /*
         * @brief: Sets the currently selected block / movement for drawing.
         */
        void updateSelectedBlock( DATA::mapBlockAtom p_block );
    };
} // namespace UI::MED
