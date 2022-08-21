#pragma once
#include <memory>

#include <gtkmm/label.h>
#include <gtkmm/notebook.h>

#include "../../model.h"
#include "bankOverview.h"
#include "bankSettings.h"
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

        std::shared_ptr<mapEditor>    _mapEditor;
        std::shared_ptr<bankOverview> _bankOverview;
        std::shared_ptr<bankSettings> _bankSettings;

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
            switch( _mapNotebook.get_current_page( ) ) {
            case 0:
                if( _mapEditor ) { _mapEditor->redraw( ); }
                break;
            case 1:
                if( _bankOverview ) { _bankOverview->redraw( ); }
                break;
            case 2:
                if( _bankSettings ) { _bankSettings->redraw( ); }
                break;
            }
        }

        inline void replaceOverviewMap( const DATA::computedMapSlice& p_map, u8 p_mapY,
                                        u8 p_mapX ) {
            if( _bankOverview ) { _bankOverview->replaceOverviewMap( p_map, p_mapY, p_mapX ); }
        }

        void copyAction( );
        void pasteAction( );
        void deleteAction( );
        void selectnewAction( );
    };
} // namespace UI
