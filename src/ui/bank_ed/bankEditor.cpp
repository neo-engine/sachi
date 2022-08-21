#include "bankEditor.h"
#include "../root.h"

namespace UI {
    bankEditor::bankEditor( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {
        // Map editor
        _mapNotebook.set_expand( );
        _mapEditor = std::make_shared<mapEditor>( p_model, p_root );
        if( _mapEditor ) { _mapNotebook.append_page( *_mapEditor, "Map _Editor", true ); }
        _bankOverview = std::make_shared<bankOverview>( p_model, p_root );
        if( _bankOverview ) { _mapNotebook.append_page( *_bankOverview, "Bank _Overview", true ); }
        _bankSettings = std::make_shared<bankSettings>( p_model, p_root );
        if( _bankSettings ) { _mapNotebook.append_page( *_bankSettings, "Bank Settin_gs", true ); }

        _mapNotebook.signal_switch_page( ).connect(
            [ this ]( Gtk::Widget*, guint ) { redraw( ); } );
    }

    void bankEditor::copyAction( ) {
        if( _model.selectedBank( ) == -1 ) { return; }
        switch( _mapNotebook.get_current_page( ) ) {
        case 0:
            if( _mapEditor ) { _mapEditor->copyAction( ); }
            break;
        case 1:
            if( _bankOverview ) { _bankOverview->copyAction( ); }
            break;
        default: break;
        }
    }

    void bankEditor::pasteAction( ) {
        if( _model.selectedBank( ) == -1 ) { return; }
        switch( _mapNotebook.get_current_page( ) ) {
        case 0:
            if( _mapEditor ) { _mapEditor->pasteAction( ); }
            break;
        case 1:
            if( _bankOverview ) { _bankOverview->pasteAction( ); }
            break;
        default: break;
        }
    }

    void bankEditor::deleteAction( ) {
        if( _model.selectedBank( ) == -1 ) { return; }
        switch( _mapNotebook.get_current_page( ) ) {
        case 0:
            if( _mapEditor ) { _mapEditor->deleteAction( ); }
            break;
        case 1:
            if( _bankOverview ) { _bankOverview->deleteAction( ); }
            break;
        default: break;
        }
    }

    void bankEditor::selectnewAction( ) {
        if( _model.selectedBank( ) == -1 ) { return; }
        switch( _mapNotebook.get_current_page( ) ) {
        case 0:
            if( _mapEditor ) { _mapEditor->selectnewAction( ); }
            break;
        case 1:
            if( _bankOverview ) { _bankOverview->selectnewAction( ); }
            break;
        default: break;
        }
    }

} // namespace UI
