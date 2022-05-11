#include "bankEditor.h"
#include "../root.h"

namespace UI {
    bankEditor::bankEditor( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {
        // Map editor
        _mapNotebook.set_expand( );
        _mapEditor = std::make_shared<mapEditor>( p_model, p_root );
        if( _mapEditor ) { _mapNotebook.append_page( *_mapEditor, "Map _Editor", true ); }
        //        _bankOverview = std::make_shared<bankOverview>( p_model, p_root );
        //        if( _bankOverview ) { _mapNotebook.append_page( *_bankOverview, "Bank _Overview",
        //        true ); }
        _bankSettings = std::make_shared<bankSettings>( p_model, p_root );
        if( _bankSettings ) { _mapNotebook.append_page( *_bankSettings, "Bank Settin_gs", true ); }
    }
} // namespace UI
