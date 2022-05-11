#include "mapEditor.h"
#include "../../log.h"
#include "../root.h"
#include "map_ed/actionBar.h"
#include "map_ed/blockSelector.h"
#include "map_ed/editableMap.h"

namespace UI {
    mapEditor::mapEditor( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        // set up boxes
        _mapEditorMainBox.set_margin( MARGIN );

        // set up mode toggles
        _mapEditorModeToggles = std::make_shared<switchButton>( std::vector<std::string>{
            "_Blocks", "_Movements", "Loca_tions", "E_vents", "_Wild PKMN", "Meta _Data" } );

        if( _mapEditorModeToggles ) {
            _mapEditorMainBox.append( *_mapEditorModeToggles );
            _mapEditorModeToggles->connect( [ this ]( u8 p_newChoice ) {
                setNewMapEditMode( mapDisplayMode( p_newChoice ) );
            } );
        }

        // set up main box

        _mapContentMainBox.set_margin_top( MARGIN );
        _mapEditorMainBox.append( _mapContentMainBox );

        // left side: map, wpoke, meta, actionbar
        _mapContentMainBox.append( _mapMainBox );
        _mapMainBox.set_expand( );

        _edMap = std::make_shared<MED::editableMap>( p_model, p_root, *this );
        if( _edMap ) { _mapMainBox.append( *_edMap ); }

        _actionBar = std::make_shared<MED::actionBar>( p_model, p_root );
        if( _actionBar ) { _mapMainBox.append( *_actionBar ); }

        // right side: blockPicker, movementPicker
        _mapContentMainBox.append( _sideBox );
        // _sideBox.set_expand( );

        _blockPicker = std::make_shared<MED::blockSelector>( p_model, p_root );
        if( _blockPicker ) { _sideBox.append( *_blockPicker ); }

        // _mvmtPicker = std::make_shared<MED::movementSelector>( p_model );
        // if( _mvmtPicker ) { _sideBox.append( _mvmtPicker ); }

        setNewMapEditMode( MODE_EDIT_TILES );
    }

    void mapEditor::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        if( _blockPicker ) { _blockPicker->redraw( ); }
        // if( _mvmtPicker ) { _mvmtPicker->redraw( ); }
        if( _actionBar ) { _actionBar->redraw( ); }
        if( _edMap ) { _edMap->redraw( ); }
    }

    void mapEditor::setNewMapEditMode( mapDisplayMode p_newMode ) {
        _currentMapDisplayMode = p_newMode;

        // restore default state

        message_error( "mapEditMode", "Set new map edit mode." );

        if( _currentMapDisplayMode == MODE_EDIT_PKMN || _currentMapDisplayMode == MODE_EDIT_DATA ) {
            if( _sideBox.is_visible( ) ) { _sideBox.hide( ); }
            if( _edMap && _edMap->isVisible( ) ) { _edMap->hide( ); }
        } else {
            if( !_sideBox.is_visible( ) ) { _sideBox.show( ); }
            if( _edMap ) {
                if( !_edMap->isVisible( ) ) { _edMap->show( ); }
                _edMap->setNewMapEditMode( _currentMapDisplayMode );
            }
        }
        // if( _mvmtPicker ) { _mvmtPicker->hide( ); }

        if( _currentMapDisplayMode != MODE_EDIT_TILES ) {
            if( _blockPicker && _blockPicker->isVisible( ) ) { _blockPicker->hide( ); }
        } else {
            if( _blockPicker && !_blockPicker->isVisible( ) ) { _blockPicker->show( ); }
        }

        if( _currentMapDisplayMode != MODE_EDIT_MOVEMENT ) {
            //            if( _mvmtPicker && _mvmtPicker->isVisible( ) ) { _mvmtPicker->hide( ); }
        } else {
            //            if( _mvmtPicker && !_mvmtPicker->isVisible( ) ) { _mvmtPicker->show( ); }
        }

        if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
    }

    std::shared_ptr<Gdk::Pixbuf> mapEditor::blockSetLookup( u16 p_blockIdx ) {
        if( _blockPicker ) { return _blockPicker->blockSetLookup( p_blockIdx ); }
        return nullptr;
    }
} // namespace UI
