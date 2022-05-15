#include <gtkmm/scrolledwindow.h>

#include "../../log.h"
#include "../root.h"
#include "blck_ed/actionBar.h"
#include "blck_ed/blockSelector.h"
#include "blck_ed/tileSelector.h"
#include "blockEditor.h"

namespace UI {
    blockEditor::blockEditor( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        // set up boxes
        _mainBox.set_margin( MARGIN );

        // set up mode toggles
        _modeToggles = std::make_shared<switchButton>(
            std::vector<std::string>{ "_Blocks", "_Tiles", "_Palettes" } );

        if( _modeToggles ) {
            _mainBox.append( *_modeToggles );
            _modeToggles->connect(
                [ this ]( u8 p_newChoice ) { setNewEditMode( tseDisplayMode( p_newChoice ) ); } );
        }

        // set up main box

        Gtk::ScrolledWindow sb{ };
        sb.set_child( _contentMainBox );
        sb.set_expand( );

        _contentMainBox.set_margin_top( MARGIN );
        _mainBox.append( sb );

        _contentMainBox.append( _col1MainBox );
        _col1MainBox.set_expand( );
        _blockPicker = std::make_shared<TED::blockSelector>( p_model, p_root );
        if( _blockPicker ) { _col1MainBox.append( *_blockPicker ); }

        _contentMainBox.append( _col2MainBox );
        _col2MainBox.set_expand( );
        _tilePicker = std::make_shared<TED::tileSelector>( p_model, p_root );
        if( _tilePicker ) { _col2MainBox.append( *_tilePicker ); }

        _contentMainBox.append( _col3MainBox );
        _col3MainBox.set_expand( );

        _contentMainBox.append( _col4MainBox );
        _col4MainBox.set_expand( );

        _contentMainBox.append( _col5MainBox );
        _col5MainBox.set_expand( );

        _actionBar = std::make_shared<TED::actionBar>( p_model, p_root );
        if( _actionBar ) { _mainBox.append( *_actionBar ); }
        setNewEditMode( TSEMODE_EDIT_BLOCKS );
    }

    void blockEditor::redraw( ) {
        if( _model.selectedBank( ) != -1 ) { return; }

        if( _actionBar ) { _actionBar->redraw( ); }
        if( _blockPicker ) { _blockPicker->redraw( ); }
        if( _tilePicker ) { _tilePicker->redraw( ); }
    }

    void blockEditor::setNewEditMode( tseDisplayMode p_newMode ) {
        _currentDisplayMode = p_newMode;

        switch( _currentDisplayMode ) {
        default:
        case TSEMODE_EDIT_BLOCKS:
            if( !_col1MainBox.is_visible( ) ) { _col1MainBox.show( ); }
            if( !_col2MainBox.is_visible( ) ) { _col2MainBox.show( ); }
            if( _col3MainBox.is_visible( ) ) { _col3MainBox.hide( ); }
            if( _col4MainBox.is_visible( ) ) { _col4MainBox.hide( ); }
            if( _col5MainBox.is_visible( ) ) { _col5MainBox.hide( ); }
            _col2MainBox.set_margin_start( MARGIN );
            _col3MainBox.set_margin_start( 0 );
            _col5MainBox.set_margin_start( 0 );
            break;

        case TSEMODE_EDIT_TILES:
            if( _col1MainBox.is_visible( ) ) { _col1MainBox.hide( ); }
            if( !_col2MainBox.is_visible( ) ) { _col2MainBox.show( ); }
            if( !_col3MainBox.is_visible( ) ) { _col3MainBox.show( ); }
            if( _col4MainBox.is_visible( ) ) { _col4MainBox.hide( ); }
            if( _col5MainBox.is_visible( ) ) { _col5MainBox.hide( ); }
            _col2MainBox.set_margin_start( 0 );
            _col3MainBox.set_margin_start( MARGIN );
            _col5MainBox.set_margin_start( 0 );
            break;

        case TSEMODE_EDIT_PALETTES:
            if( _col1MainBox.is_visible( ) ) { _col1MainBox.hide( ); }
            if( _col2MainBox.is_visible( ) ) { _col2MainBox.hide( ); }
            if( _col3MainBox.is_visible( ) ) { _col3MainBox.hide( ); }
            if( !_col4MainBox.is_visible( ) ) { _col4MainBox.show( ); }
            if( !_col5MainBox.is_visible( ) ) { _col5MainBox.show( ); }
            _col2MainBox.set_margin_start( 0 );
            _col3MainBox.set_margin_start( 0 );
            _col5MainBox.set_margin_start( MARGIN );
            break;
        }
        if( _actionBar ) { _actionBar->setNewEditMode( p_newMode ); }
    }
} // namespace UI
