#pragma once
#include <memory>

#include <gtkmm/notebook.h>

#include "../../defines.h"
#include "../../model.h"
#include "trainerEditor.h"
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

        std::shared_ptr<trainerEditor> _trainerEditor;
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
            if( _trainerEditor ) { _trainerEditor->redraw( ); }
            // if( _trainerBankSettings ) { _trainerBankSettings->redraw( ); }
        }

        inline void copyAction( ) {
            if( _model.selectedBank( ) != -1 ) { return; }
            if( _trainerEditor ) { _trainerEditor->copyAction( ); }
        }

        inline void pasteAction( ) {
            if( _model.selectedBank( ) != -1 ) { return; }
            if( _trainerEditor ) { _trainerEditor->pasteAction( ); }
        }

        inline void deleteAction( ) {
            if( _model.selectedBank( ) != -1 ) { return; }
            if( _trainerEditor ) { _trainerEditor->deleteAction( ); }
        }

        inline void selectnewAction( ) {
            if( _model.selectedBank( ) != -1 ) { return; }
            if( _trainerEditor ) { _trainerEditor->selectnewAction( ); }
        }

        inline void performAction( u8 p_actionId ) {
            if( _model.selectedBank( ) != -1 ) { return; }
            if( _trainerEditor ) { _trainerEditor->performAction( p_actionId ); }
        }
    };
} // namespace UI
