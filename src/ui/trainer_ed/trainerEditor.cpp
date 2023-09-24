#include <gtkmm/scrolledwindow.h>

#include "../../log.h"
#include "../root.h"
// #include "tr_ed/trainerInfo.h"
// #include "tr_ed/trainerItems.h"
// #include "tr_ed/trainerTeamEditor.h"
#include "trainerEditor.h"

namespace UI {
    trainerEditor::trainerEditor( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        // set up boxes
        _mainBox.set_margin( MARGIN );

        Gtk::ScrolledWindow sb{ };
        sb.set_child( _contentMainBox );
        sb.set_expand( );

        _contentMainBox.set_margin_top( MARGIN );
        _mainBox.append( sb );

        /*
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
        _tileCanvas = std::make_shared<TED::tileCanvas>( p_model, p_root );
        if( _tileCanvas ) { _col3MainBox.append( *_tileCanvas ); }

        _contentMainBox.append( _col4MainBox );
        _col4MainBox.set_expand( );
        _paletteEditor = std::make_shared<TED::paletteEditor>( p_model, p_root );
        if( _paletteEditor ) { _col4MainBox.append( *_paletteEditor ); }

        _actionBar = std::make_shared<TED::actionBar>( p_model, p_root );
        if( _actionBar ) { _mainBox.append( *_actionBar ); }
        setNewEditMode( TSEMODE_EDIT_BLOCKS );
        */
    }

    void trainerEditor::redraw( ) {
        if( _model.selectedBank( ) != -1 ) { return; }

        // if( _trainerInfo ) { _trainerInfo->redraw( ); }
        // if( _trainerItems ) { _trainerItems->redraw( ); }
        // if( _trainerTeams ) { _trainerTeams->redraw( ); }
    }
} // namespace UI
