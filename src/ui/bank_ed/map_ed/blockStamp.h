#pragma once

#include <memory>
#include <gtkmm/dialog.h>
#include "../../../data/maprender.h"
#include "../../../model.h"
#include "../../pure/editableBlock.h"
#include "../../pure/mapSlice.h"
#include "../mapEditor.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: A Dialog window displaying block stamp data
     */
    class blockStamp {
        model&     _model;
        root&      _rootWindow;
        mapEditor& _mapEditor;

        mapEditor::mapDisplayMode _currentMapDisplayMode;

        std::vector<DATA::mapBlockAtom> _blockStampData;
        std::shared_ptr<Gtk::Dialog>    _blockStampDialog;

        lookupMapSlice _blockStampMap;
        u16            _blockStampWidth;
        bool           _blockStampDialogInvalid = true;

      public:
        blockStamp( model& p_model, root& p_root, mapEditor& p_parent );

        std::shared_ptr<Gtk::Dialog> getDialog( ) {
            return _blockStampDialog;
        }

        void update( const std::vector<DATA::mapBlockAtom>& p_blockData, u16 p_width );

        void reset( );

        void create( );

        inline void setNewMapEditMode( mapEditor::mapDisplayMode p_newMode ) {
            _currentMapDisplayMode = p_newMode;
            redraw( );
        }

        void redraw( );

        inline bool isValid( ) const {
            return !_blockStampDialogInvalid;
        }

        inline u16 sizeX( ) const {
            if( !isValid( ) ) { return 0; }
            return _blockStampWidth;
        }

        inline u16 sizeY( ) const {
            if( !isValid( ) ) { return 0; }
            return _blockStampData.size( ) / _blockStampWidth;
        }

        const DATA::mapBlockAtom& at( u16 p_x, u16 p_y ) const {
            auto pos{ _blockStampWidth * p_y + p_x };
            return _blockStampData[ pos ];
        }

        DATA::mapBlockAtom& at( u16 p_x, u16 p_y ) {
            auto pos{ _blockStampWidth * p_y + p_x };
            return _blockStampData[ pos ];
        }
    };
} // namespace UI::MED
