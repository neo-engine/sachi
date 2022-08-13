#pragma once
#include <memory>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "../defines.h"
#include "../model.h"
#include "bank_ed/bankEditor.h"
#include "headerBar.h"
#include "side_p/sideBar.h"
#include "tile_ed/tileSetEditor.h"
#include "welcome.h"

namespace UI {
    class root : public Gtk::Window {
        std::shared_ptr<Gio::SimpleActionGroup> _loadActions;
        std::shared_ptr<Gio::SimpleAction>      _loadFsrootAction;
        std::shared_ptr<Gio::SimpleAction>      _loadReloadmapAction;
        std::shared_ptr<Gio::SimpleAction>      _loadReloadmapbankAction;
        std::shared_ptr<Gio::SimpleAction>      _loadImportmapAction;
        std::shared_ptr<Gio::SimpleAction>      _loadImportlargemapAction;
        std::shared_ptr<Gio::SimpleAction>      _loadImportblocks1Action;
        std::shared_ptr<Gio::SimpleAction>      _loadImportblocks2Action;
        std::shared_ptr<Gio::SimpleAction>      _loadImporttiles1Action;
        std::shared_ptr<Gio::SimpleAction>      _loadImporttiles2Action;
        std::shared_ptr<Gio::SimpleActionGroup> _saveActions;
        std::shared_ptr<Gio::SimpleAction>      _saveFsrootAction;
        std::shared_ptr<Gio::SimpleAction>      _saveMapAction;
        std::shared_ptr<Gio::SimpleAction>      _saveMapbankAction;
        std::shared_ptr<Gio::SimpleAction>      _saveExportmapAction;
        std::shared_ptr<Gio::SimpleAction>      _saveExportblocks1Action;
        std::shared_ptr<Gio::SimpleAction>      _saveExportblocks2Action;
        std::shared_ptr<Gio::SimpleAction>      _saveExporttiles1Action;
        std::shared_ptr<Gio::SimpleAction>      _saveExporttiles2Action;
        std::shared_ptr<Gio::SimpleActionGroup> _specialActions;
        std::shared_ptr<Gio::SimpleAction>      _specialRecomputedns1Action;
        std::shared_ptr<Gio::SimpleAction>      _specialRecomputedns2Action;
        std::shared_ptr<Gio::SimpleAction>      _specialCopylocationsAction;
        std::shared_ptr<Gio::SimpleAction>      _specialCopylocations2Action;
        std::shared_ptr<Gio::SimpleAction>      _specialCacheAction;
        std::shared_ptr<Gio::SimpleAction>      _specialPasteAction;
        std::shared_ptr<Gio::SimpleAction>      _specialDeleteAction;

        context _context;
        model   _model;

        bool _disableRedraw = true;

        std::shared_ptr<headerBar> _headerBar;
        std::shared_ptr<welcome>   _welcome;

        Gtk::Box                       _mainBox; // main box containing all other widgets
        std::shared_ptr<sideBar>       _sideBar;
        Gtk::Label                     _loadMapLabel;
        std::shared_ptr<bankEditor>    _bankEditor;
        std::shared_ptr<tileSetEditor> _tileSetEditor;

        Gtk::SpinButton _sb1, _sb2;

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

        /*
         * @brief: redraws the side panel.
         */
        void redrawPanel( );

        void initActions( );
        void initEvents( );

        /*
         * @brief: Sets the current main working context, enables/disables actions and
         * hides/shows the corresponding widgets.
         */
        void switchContext( context p_context );

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

        /*
         * @brief: Does nothing if p_bank is loaded, otherwise creates and loads a new
         * bank.
         */
        void createMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX );

        /*
         * @brief: Loads the specified map bank in the editor. Lazily reads the data from
         * the FS, one map bank at a time.
         */
        void loadMapBank( u16 p_bank, bool p_redraw = false );

        /*
         * @brief: Loads the specified bank and map.
         */
        void loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX );

        /*
         * @brief: Loads the map in the current bank that lies at the relative position
         * (p_dx, p_dy). Does nothing if the new coordinates are (partially) negative or larger
         * than 99; extends the map bank if the new coordinates lie outside of the current
         * map bank bounds, otherwise.
         */
        void moveToMap( s8 p_dy, s8 p_dx );

        /*
         * @brief: Cleanup operations when the specified map gets unloaded (update map bank
         * overview)
         */
        void onUnloadMap( u16 p_bank, u8 p_mapY, u8 p_mapX );

        /*
         * @brief: Loads the tile set editing UI and the combined tile set of p_ts1 and
         * p_ts2 in particular.
         */
        void editTileSets( u8 p_ts1, u8 p_ts2 );

        /*
         * @brief: Handler for the "Open FSROOT" button.
         */
        void onFsRootOpenClick( );

        /*
         * @brief: Handler for the "Select" button in the Open Folder Dialog of the Open
         * FSROOT button.
         */
        void onFolderDialogResponse( int p_responseId, Gtk::FileChooserDialog* p_dialog );

        /*
         * @brief: Handler for the "Save Changes" button.
         */
        void onFsRootSaveClick( );
    };
} // namespace UI
