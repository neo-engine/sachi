#pragma once
#include <memory>

#include <gtkmm/notebook.h>

#include "../../model.h"
// #include "trainerEditor.h"
// #include "trainerBankSettings.h"

namespace UI {
    class root;

    /*
     * @brief: Main widget of the trainer editor.
     */
    class trainerBankEditor {
        model& _model;
        root&  _rootWindow;

        Gtk::Notebook _treNotebook; // main container for anything tre related

        // std::shared_ptr<trainerEditor>     _trainerEditor;
        // std::shared_ptr<trainerBankSettings> _trainerSettings;

      public:
        trainerBankEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _treNotebook;
        }

        inline void hide( ) {
            _treNotebook.hide( );
        }

        inline void show( ) {
            _treNotebook.show( );
        }

        inline bool isVisible( ) {
            return _treNotebook.is_visible( );
        }

        inline void redraw( ) {
            // if( _trainerEditor ) { _trainerEditor->redraw( ); }
            // if( _trainerBankSettings ) { _trainerBankSettings->redraw( ); }
        }
    };
} // namespace UI
