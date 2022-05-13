#include "tileSetEditor.h"
#include "../root.h"

namespace UI {
    tileSetEditor::tileSetEditor( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {
        // Map editor
        _tseNotebook.set_expand( );

        // _blockEditor = std::make_shared<blockEditor>( p_model, p_root );
        // if( _blockEditor ) { _tseNotebook.append_page( *_blockEditor, "Tile Set _Editor", true );
        // }

        _tileSetSettings = std::make_shared<tileSetSettings>( p_model, p_root );
        if( _tileSetSettings ) {
            _tseNotebook.append_page( *_tileSetSettings, "Tile Set Settin_gs", true );
        }
    }
} // namespace UI
