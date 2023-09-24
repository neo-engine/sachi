#include "../root.h"
#include "trainerBankEditor.h"

namespace UI {
    trainerBankEditor::trainerBankEditor( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {
        _treNotebook.set_expand( );

        // _trainerEditor = std::make_shared<trainerEditor>( p_model, p_root );
        // if( _trainerEditor ) {
        //    _treNotebook.append_page( *_trainerEditor, "Trainer _Editor", true );
        //}

        // _trainerBankSettings = std::make_shared<trainerBankSettings>( p_model, p_root );
        // if( _trainerBankSettings ) {
        //    _treNotebook.append_page( *_trainerBankSettings, "General Trainer Settin_gs", true );
        //}
    }
} // namespace UI
