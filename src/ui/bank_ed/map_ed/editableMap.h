#pragma once
#include <memory>
#include <tuple>
#include <vector>

#include <gtkmm/grid.h>
#include <gtkmm/overlay.h>
#include <gtkmm/scrolledwindow.h>

#include "../../../model.h"
#include "../../pure/dropDown.h"
#include "../../pure/mapSlice.h"
#include "../mapEditor.h"
#include "blockStamp.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: Widget to edit a single map slice
     */
    class editableMap {
        model&     _model;
        root&      _rootWindow;
        mapEditor& _mapEditor;

        mapEditor::mapDisplayMode _currentMapDisplayMode;

        std::vector<std::vector<lookupMapSlice>>
            _currentMap; // main map and parts of the adjacent maps

        Gtk::Overlay _centerMapOverlay;
        Gtk::Grid    _locationGrid;

        std::shared_ptr<blockStamp> _blockStamp;

        std::vector<std::vector<std::shared_ptr<locationDropDown>>> _locations;

        std::tuple<u16, u16, s8, s8> _dragStart;
        std::tuple<s16, s16>         _dragLast;

        Gtk::ScrolledWindow _mainWindow;
        Gtk::Grid           _mapGrid; // contains the actual maps

        void onMapDragStart( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, s8 p_mapX,
                             s8 p_mapY, bool p_allowEdit = true );
        void onMapDragUpdate( mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                              s8 p_mapY, bool p_allowEdit = true );
        void onMapDragEnd( mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX, s8 p_mapY,
                           bool p_allowEdit = true );

        /*
         * @brief: Handles clicks on maps:
         * Left: Change block to currently selected block (if p_allowChange)
         * Middle: Change block and recursively all adjacent blocks that are the same as
         * the original block (flood fill)
         * Right: Select block
         */
        void onMapClicked( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

        /*
         * @brief: Checks if the specified block coordinates are currently visible
         * on-screen.
         */
        inline bool isInMapBounds( s16 p_blockX, s16 p_blockY, s8 p_mapX, s8 p_mapY ) {
            if( p_blockX < 0 || p_blockY < 0 ) { return false; }

            u16 wd = DATA::SIZE;
            u16 hg = DATA::SIZE;
            if( p_mapX ) { wd = _model.m_settings.m_adjacentBlocks; }
            if( p_mapY ) { hg = _model.m_settings.m_adjacentBlocks; }

            if( p_blockX >= wd || p_blockY >= hg ) { return false; }
            return true;
        }

      public:
        editableMap( model& p_model, root& p_root, mapEditor& p_parent );

        inline operator Gtk::Widget&( ) {
            return _mainWindow;
        }

        inline void show( ) {
            _mainWindow.show( );
        }

        inline void hide( ) {
            _mainWindow.hide( );
        }

        inline bool isVisible( ) {
            return _mainWindow.is_visible( );
        }

        void setNewMapEditMode( mapEditor::mapDisplayMode p_newMode );

        /*
         * @brief: Sets the currently selected block / movement for drawing.
         */
        void updateSelectedBlock( DATA::mapBlockAtom p_block );

        void redraw( bool p_full = true );
    };
} // namespace UI::MED
