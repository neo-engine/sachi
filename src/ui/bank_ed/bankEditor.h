#pragma once
#include <memory>

#include <gtkmm/label.h>
#include <gtkmm/notebook.h>

#include "../../model.h"
//#include "bankOverview.h"
//#include "bankSettings.h"
#include "mapEditor.h"

namespace UI {
    class root;

    /*
     * @brief: Main widget of the bank editor.
     */
    class bankEditor {
        model& _model;
        root&  _rootWindow;

        Gtk::Notebook _mapNotebook; // main container for anything map bank related

        std::shared_ptr<mapEditor> _mapEditor;
        // std::shared_ptr<bankOverview> _bankOverview;
        // std::shared_ptr<bankSettings> _bankSettings;

      public:
        bankEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mapNotebook;
        }

        inline void hide( ) {
            _mapNotebook.hide( );
        }

        inline void show( ) {
            _mapNotebook.show( );
        }

        inline bool isVisible( ) {
            return _mapNotebook.is_visible( );
        }

        inline void redraw( ) {
            if( _mapEditor ) { _mapEditor->redraw( ); }
            //    if( _bankOverview ) { _bankOverview->redraw( ); }
            //    if( _bankSettings ) { _bankSettings->redraw( ); }
        }
    };
} // namespace UI
