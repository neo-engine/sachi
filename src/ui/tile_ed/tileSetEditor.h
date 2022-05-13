#pragma once
#include <memory>

#include <gtkmm/notebook.h>

#include "../../model.h"
// #include "blockEditor.h"
#include "tileSetSettings.h"

namespace UI {
    class root;

    /*
     * @brief: Main widget of the bank editor.
     */
    class tileSetEditor {
        model& _model;
        root&  _rootWindow;

        Gtk::Notebook _tseNotebook; // main container for anything tse bank related

        // std::shared_ptr<blockEditor>     _blockEditor;
        std::shared_ptr<tileSetSettings> _tileSetSettings;

      public:
        tileSetEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _tseNotebook;
        }

        inline void hide( ) {
            _tseNotebook.hide( );
        }

        inline void show( ) {
            _tseNotebook.show( );
        }

        inline bool isVisible( ) {
            return _tseNotebook.is_visible( );
        }

        inline void redraw( ) {
            // if( _blockEditor ) { _blockEditor->redraw( ); }
            if( _tileSetSettings ) { _tileSetSettings->redraw( ); }
        }
    };
} // namespace UI
