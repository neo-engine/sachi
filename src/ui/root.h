#pragma once
#include <giomm/simpleactiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "../defines.h"
// #include "bank_ed/bankEditor.h"
#include "headerBar.h"
// #include "side_p/sideBar.h"
#include "../model.h"
#include "welcome.h"

namespace UI {
    class root : public Gtk::Window {

        /*
        std::shared_ptr<Gio::SimpleActionGroup> _loadActions;
        std::shared_ptr<Gio::SimpleAction>      _loadFsrootAction;
        std::shared_ptr<Gio::SimpleAction>      _loadReloadmapAction;
        std::shared_ptr<Gio::SimpleAction>      _loadReloadmapbankAction;
        std::shared_ptr<Gio::SimpleAction>      _loadImportmapAction;
        std::shared_ptr<Gio::SimpleActionGroup> _saveActions;
        std::shared_ptr<Gio::SimpleAction>      _saveFsrootAction;
        std::shared_ptr<Gio::SimpleAction>      _saveMapAction;
        std::shared_ptr<Gio::SimpleAction>      _saveMapbankAction;
        std::shared_ptr<Gio::SimpleAction>      _saveExportmapAction;
*/

        context _context;
        model   _model;

        bool _fsRootLoaded  = false;
        bool _focusMode     = false;
        bool _disableRedraw = true;

        std::shared_ptr<headerBar> _headerBar;
        std::shared_ptr<welcome>   _welcome;

        Gtk::Box _mainBox; // main box containing all other widgets
        // std::shared_ptr<sideBar> _sideBar;
        Gtk::Label _loadMapLabel;
        //        std::shared_ptr<bankEditor> _bankEditor;

      public:
        root( );
        inline ~root( ) override {
        }

        /*
         * @brief: Sets p_path as the current working directory, checks which map banks
         * are available and checks/creates other required FSROOT subfolders.
         */
        void loadNewFsRoot( const std::string& p_path );

        void redraw( );

      protected:
        // void initActions( );
        // void initEvents( );

        /*
         * @brief: Sets the current main working context, enables/disables actions and
         * hides/shows the corresponding widgets.
         */
        void switchContext( context p_context );

        /*
         * @brief: Collapses/shows the sidebar.
         */
        //      inline void collapseMapBankBar( bool p_collapse = true ) {
        //          if( _sideBar ) { _sideBar->collapse( p_collapse ); }
        //      }

        /*
         * @brief: Adds the specified path to the list of recently used FSROOT paths.
         */
        inline void addFsRootToRecent( const std::string& p_path ) {
            if( _welcome ) { _welcome->addFsRootToRecent( p_path ); }
        }

        /*
         * @brief: Removes the specified path from the recently used FSROOT paths.
         */
        inline void removeFsRootFromRecent( const std::string& p_path ) {
            if( _welcome ) { _welcome->removeFsRootFromRecent( p_path ); }
        }
    };
} // namespace UI
