#include "mapEditor.h"
#include "../../log.h"
#include "../root.h"

namespace UI {
    mapEditor::mapEditor( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {
        _currentMapDisplayMode = MODE_EDIT_TILES;

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

        // _edMap = std::make_shared<MED::editableMap>( p_model );
        // if( _edMap ) { _mapMainBox.append( *_edMap ); }

        // _actionBar = std::make_shared<MED::actionBar>( p_model );
        // if( _actionBar ) { _mapMainBox.append( *_actionBar ); }

        // right side: blockPicker, movementPicker
        _mapContentMainBox.append( _sideBox );
        _sideBox.set_expand( );

        // _blockPicker = std::make_shared<MED::blockSelector>( p_model );
        // if( _blockPicker ) { _sideBox.append( _blockPicker ); }

        // _mvmtPicker = std::make_shared<MED::movementSelector>( p_model );
        // if( _mvmtPicker ) { _sideBox.append( _mvmtPicker ); }
    }

    void mapEditor::redraw( ) {
        // if( _edMap ) { _edMap->redraw( ); }
        // if( _blockPicker ) { _blockPicker->redraw( ); }
        // if( _mvmtPicker ) { _mvmtPicker->redraw( ); }
        // if( _actionBar ) { _actionBar->redraw( ); }
    }

    void mapEditor::setNewMapEditMode( mapDisplayMode p_newMode ) {
        // restore default state

        message_error( "mapEditMode", "Set new map edit mode." );

        _sideBox.hide( );
        // if( _edMap ) { _edMap->hide( ); }
        // if( _blockPicker ) { _blockPicker->hide( ); }
        // if( _mvmtPicker ) { _mvmtPicker->hide( ); }

        _currentMapDisplayMode = p_newMode;

        switch( _currentMapDisplayMode ) {
        default:
        case MODE_EDIT_TILES:
            _sideBox.show( );
            //  if( _edMap ) {
            //     _edMap->show( );
            //     _edMap->setNewMapEditMode( _currentMapDisplayMode );
            // }
            // if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
            // if( _blockPicker ) { _blockPicker->show( ); }
            break;
        case MODE_EDIT_MOVEMENT:
            _sideBox.show( );
            //  if( _edMap ) {
            //      _edMap->show( );
            //      _edMap->setNewMapEditMode( _currentMapDisplayMode );
            // }
            //  if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
            //  if( _mvmtPicker ) { _mvmtPicker->show( ); }
            break;
        case MODE_EDIT_LOCATIONS:
            _sideBox.show( );
            // if( _edMap ) {
            //    _edMap->show( );
            //   _edMap->setNewMapEditMode( _currentMapDisplayMode );
            // }
            // if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
            // TODO
            break;
        case MODE_EDIT_EVENTS:
            _sideBox.show( );
            // if( _edMap ) {
            //     _edMap->show( );
            //     _edMap->setNewMapEditMode( _currentMapDisplayMode );
            // }
            // if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
            // TODO
            break;
        case MODE_EDIT_PKMN:
            //  if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
            // TODO
            break;
        case MODE_EDIT_DATA:
            // if( _actionBar ) { _actionBar->setNewMapEditMode( _currentMapDisplayMode ); }
            // TODO
            break;
        }
    }
} // namespace UI
