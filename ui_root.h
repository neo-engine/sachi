#pragma once
#include <map>
#include <memory>
#include <set>
#include <tuple>

#include <giomm/menu.h>
#include <giomm/menuitem.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/iconview.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/notebook.h>
#include <gtkmm/popovermenu.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/window.h>

#include "data_maprender.h"
#include "ui_editableBlock.h"
#include "ui_mapBank.h"
#include "ui_mapBankOverview.h"
#include "ui_mapSlice.h"

namespace UI {
    class root : public Gtk::Window {
        struct mapBankInfo {
            std::shared_ptr<mapBank>                                          m_widget = nullptr;
            std::unique_ptr<DATA::mapBank>                                    m_bank   = nullptr;
            std::unique_ptr<std::vector<std::vector<DATA::computedMapSlice>>> m_computedBank;

            bool m_loaded = false;

            DATA::mapBankInfo m_info = { 0, 0, 1 };

            inline u8 getSizeX( ) const {
                return m_info.m_sizeX;
            }
            inline u8 getSizeY( ) const {
                return m_info.m_sizeY;
            }
            inline bool isScattered( ) const {
                return m_info.m_mapMode == DATA::MAPMODE_SCATTERED;
            }
            inline bool isCombined( ) const {
                return m_info.m_mapMode == DATA::MAPMODE_COMBINED;
            }
            inline u8 getMapMode( ) const {
                return m_info.m_mapMode;
            }

            inline void setSizeX( u8 p_newSize ) {
                m_info.m_sizeX = p_newSize;
            }
            inline void setSizeY( u8 p_newSize ) {
                m_info.m_sizeY = p_newSize;
            }
            inline void setScattered( bool p_scattered = true ) {
                m_info.m_mapMode = p_scattered;
            }
            inline void setCombined( ) {
                m_info.m_mapMode = DATA::MAPMODE_COMBINED;
            }
            inline void setMapMode( u8 p_mode ) {
                m_info.m_mapMode = p_mode;
            }
        };

        struct blockSetInfo {
            DATA::blockSet<1> m_blockSet;
            DATA::tileSet<1>  m_tileSet;
            DATA::palette     m_pals[ 8 * DAYTIMES ];
            u8                m_stringListItem = 0;
        };

        class recentFsRootModelColumn : public Gtk::TreeModel::ColumnRecord {
          public:
            recentFsRootModelColumn( ) {
                //                add( m_modified );
                add( m_path );
                add( m_pixbuf );
            }

            //            Gtk::TreeModelColumn<Glib::DateTime>               m_modified;
            Gtk::TreeModelColumn<std::string>                  m_path;
            Gtk::TreeModelColumn<std::shared_ptr<Gdk::Pixbuf>> m_pixbuf;
        };

        enum mapDisplayMode : u8 {
            MODE_EDIT_TILES,
            MODE_EDIT_MOVEMENT,
            MODE_EDIT_LOCATIONS,
            MODE_EDIT_EVENTS,
            MODE_EDIT_DATA,
        };

        enum bankOverviewMode : u8 {
            MODE_DISPLAY_MAPS,
            MODE_DISPLAY_LOCATIONS,
        };

        enum context : u8 {
            NONE,        // nothing loaded
            FSROOT_NONE, // fsroot loaded, but nothing else
            MAP_EDITOR,  // map bank loaded and map visible
            TILE_EDITOR, // tile set editor loaded
        };

        //////////////////////////////////////////////////////////////////////////////////
        //
        // Block stamp dialog
        //
        //////////////////////////////////////////////////////////////////////////////////

        std::vector<DATA::mapBlockAtom> _blockStampData;
        std::shared_ptr<Gtk::Dialog>    _blockStampDialog;

        lookupMapSlice               _blockStampMap;
        u16                          _blockStampWidth;
        bool                         _blockStampDialogInvalid = true;
        std::tuple<u16, u16, s8, s8> _dragStart;
        std::tuple<s16, s16>         _dragLast;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // main window
        //
        //////////////////////////////////////////////////////////////////////////////////

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

        Gtk::Box                         _mainBox; // main box containing all other widgets
        std::shared_ptr<Gtk::Button>     _saveButton, _openButton;
        std::shared_ptr<Gtk::MenuButton> _openMenuButton, _saveMenuButton;
        Gtk::PopoverMenu                 _openMenuPopover, _saveMenuPopover;
        std::shared_ptr<Gio::Menu>       _openMenu, _saveMenu;

        Gtk::Label _titleLabel, _subtitleLabel;

        bool _fsRootLoaded  = false;
        bool _focusMode     = false;
        bool _disableRedraw = true;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // welcome screen
        //
        //////////////////////////////////////////////////////////////////////////////////

        recentFsRootModelColumn             _recentViewColumns;
        Gtk::IconView                       _recentFsRootIconView;
        Glib::RefPtr<Gtk::ListStore>        _recentFsRootListModel;
        std::shared_ptr<Gtk::RecentManager> _recentlyUsedFsRoots;
        Gtk::ScrolledWindow                 _ivScrolledWindow;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // sidebar
        //
        //////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<Gtk::Button> _collapseMapBanksButton; // collapse/show sidebar

        bool _mapBankBarCollapsed = false;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // side bar -> tile set editor
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Label _sbTileSetBarLabel;
        Gtk::Box   _sbTileSetBox;
        s16        _sbTileSetSel1 = 0, _sbTileSetSel2 = 1;

        std::shared_ptr<editTileSet> _editTileSet;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // side bar -> map bank / map chooser
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Label                  _mapBankBarLabel;
        Gtk::Box                    _mapBankBox;
        std::shared_ptr<addMapBank> _addMapBank;

        std::map<u16, mapBankInfo> _mapBanks; // bank -> bankinfo
        int                        _selectedBank = -1;
        s16                        _selectedMapX = -1, _selectedMapY = -1;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // tile set editor
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Notebook _tseNotebook; // main container for anything tile set editor related
        std::shared_ptr<editableBlock> _editBlock;
        std::shared_ptr<tileInfo>      _currentTile; // currently selected tile

        computedMapSlice _tsets1widget, _tsets2widget;

        std::vector<std::shared_ptr<Gtk::ToggleButton>> _tseTileModeToggles;

        u16 _tseSelectedBlockIdx = 0;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // map editor
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Label     _loadMapLabel; // Message before map bank loaded
        Gtk::Notebook  _mapNotebook;  // main container for anything map bank related
        mapDisplayMode _currentMapDisplayMode;
        std::vector<std::shared_ptr<Gtk::ToggleButton>> _mapEditorModeToggles;
        Gtk::Box  _mapEditorMapBox{ Gtk::Orientation::VERTICAL };
        Gtk::Grid _mapGrid; // contains the actual maps

        Gtk::Frame                   _mapEditorActionBar;
        std::shared_ptr<Gtk::Button> _mapNavButton[ 3 ][ 3 ];
        Gtk::SpinButton              _mapEditorSettings1;
        Gtk::SpinButton              _mapEditorSettings2;
        Gtk::SpinButton              _mapEditorSettings3;
        Gtk::SpinButton              _mapEditorSettings4;
        Gtk::SpinButton              _mapEditorSettings5;
        Gtk::SpinButton              _mapEditorSettings6;

        std::vector<std::vector<lookupMapSlice>>
             _currentMap; // main map and parts of the adjacent maps
        u8   _currentDayTime  = 0;
        u8   _blockScale      = 1;
        u8   _blockSpacing    = 0;
        u8   _neighborSpacing = 10;
        bool _showAdjacent    = true;
        u16  _blockSetWidth   = 8;
        u8   _adjacentBlocks  = 8;

        u8 _bankOverviewSpacing = 2;
        u8 _bankOverviewScale   = 3;

        u8 _tileSetMode = DATA::TILEMODE_DEFAULT;

        DATA::mapBlockAtom _currentlySelectedBlock = DATA::mapBlockAtom( );

        //////////////////////////////////////////////////////////////////////////////////
        //
        // edit blocks
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Frame       _blockSetFrame;
        Gtk::Box         _mapEditorBlockSetBox{ Gtk::Orientation::VERTICAL };
        Gtk::Box         _abEb1; // block set width settings box, contains _mapEditorSettings4
        Gtk::DropDown    _mapEditorBS1CB, _mapEditorBS2CB; // select BS1/BS2
        computedMapSlice _ts1widget, _ts2widget;

        std::shared_ptr<Gtk::StringList>                _mapBankStrList; // block set names
        std::vector<std::pair<DATA::computedBlock, u8>> _currentBlockset1;
        std::vector<std::pair<DATA::computedBlock, u8>> _currentBlockset2;
        std::map<u8, blockSetInfo>                      _blockSets;
        std::set<u8>                                    _blockSetNames;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // edit movements
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Frame       _movementFrame;
        computedMapSlice _movementWidget;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // Map bank overview
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Box _mapOverviewBox{ Gtk::Orientation::VERTICAL };
        std::vector<std::shared_ptr<Gtk::ToggleButton>> _bankOverviewModeToggles;

        mapBankOverview _mapBankOverview;
        Gtk::SpinButton _mapBankOverviewSettings1;
        Gtk::SpinButton _mapBankOverviewSettings2;
        Gtk::SpinButton _mapBankOverviewSettings3;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // Map bank Settings
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Box _mapSettingsBox{ Gtk::Orientation::VERTICAL };
        std::vector<std::shared_ptr<Gtk::ToggleButton>> _mapBankSettingsMapModeToggles;

      public:
        root( );
        ~root( ) override;

        /*
         * @brief: Sets p_path as the current working directory, checks which map banks
         * are available and checks/creates other required FSROOT subfolders.
         */
        void loadNewFsRoot( const std::string& p_path );

      private:
        //////////////////////////////////////////////////////////////////////////////////
        //
        // Main window
        //
        //////////////////////////////////////////////////////////////////////////////////

        void initActions( );
        void initHeaderBar( );
        void initSideBar( );

        void initWelcome( );
        void initTileSetEditor( );
        void initMapEditor( );

        void initEvents( );

        /*
         * @brief: Sets the current main working context, enables/disables actions and
         * hides/shows the corresponding widgets.
         */
        void switchContext( context p_context );

        /*
         * @brief: Sets the title displayed on the header bar.
         */
        void setTitle( const std::string& p_windowTitle = "", const std::string& p_mainTitle = "",
                       const std::string& p_subTitle = "" );

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

        /*
         * @brief: Collapses/shows the sidebar.
         */
        void collapseMapBankBar( bool p_collapse = true );

        /*
         * @brief: Adds the specified path to the list of recently used FSROOT paths.
         */
        void addFsRootToRecent( const std::string& p_path );

        /*
         * @brief: Removes the specified path from the recently used FSROOT paths.
         */
        void removeFsRootFromRecent( const std::string& p_path );

        /*
         * @brief: Returns a list of all recently used FSROOT paths.
         */
        auto getRecentFsRoots( );

        /*
         * @brief: Adds the recently used FSROOT paths for quick access to the IconView on
         * the start screen.
         */
        void populateRecentFsRootIconView( );

        //////////////////////////////////////////////////////////////////////////////////
        //
        // Tile set editor
        //
        //////////////////////////////////////////////////////////////////////////////////

        /*
         * @brief: Creates a new block/tile/palette set with the specified index.
         */
        void createBlockSet( u8 p_tsIdx );

        /*
         * @brief: Loads the tile set editing UI and the combined tile set of p_ts1 and
         * p_ts2 in particular.
         */
        void editTileSets( u8 p_ts1, u8 p_ts2 );

        /*
         * @brief: Sets the status of the edit tile set widget to the specified status,
         * resulting in the TS being highlighted in the sidebar.
         */
        void markTileSetsChanged( mapBank::status p_newStatus
                                  = mapBank::status::STATUS_EDITED_UNSAVED );

        /*
         * @brief: Sets the tile set mode (single file per ts/bs/pal or one combined file
         * for everything)
         */
        inline void setTileSetMode( u8 p_mode ) {
            _tileSetMode = p_mode;
        }

        //////////////////////////////////////////////////////////////////////////////////
        //
        // Map editor
        //
        //////////////////////////////////////////////////////////////////////////////////

        /*
         * @brief: Sets the mode of the map editor (edit blocks, movements, events, etc)
         */
        void setNewMapEditMode( mapDisplayMode p_newMode );

        /*
         * @brief: Sets the daytime of all maps and redraws them.
         */
        void setCurrentDaytime( u8 p_newDaytime );

        /*
         * @brief: Looks up the computed image data of the block sets.
         */
        inline std::shared_ptr<Gdk::Pixbuf> blockSetLookup( u16 p_blockIdx ) {
            if( p_blockIdx > DATA::MAX_BLOCKS_PER_TILE_SET ) {
                p_blockIdx -= DATA::MAX_BLOCKS_PER_TILE_SET;
                return _ts2widget.getImageData( p_blockIdx );
            } else {
                return _ts1widget.getImageData( p_blockIdx );
            }
        }

        //  IO

        /*
         * @brief: Reads all tile / block / palette sets from disk; first looks for a
         * combined tile set bank ("MAP_PATH/tileset.tsb"), then checks the directories
         * "TILESET_PATH", "BLOCKSET_PATH" and "PALETTE_PATH".
         */
        bool readTileSets( );

        /*
         * @brief: Writes all tile / block / palette sets to disk.
         */
        bool writeTileSets( );

        /*
         * @brief: Writes the specified map slice to the FS.
         * @param p_path: If specified, the map is stored at the specified path, otherwise
         * it will be stored at the default location (FSROOT/MAPS/p_bank[/p_mapY]/p_mapY_p_mapX.map)
         * in the current working directory.
         */
        bool writeMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path = "",
                            bool p_writeMapData = true );

        /*
         * @brief: Writes all map slices of the specified map bank to the FS.
         */
        bool writeMapBank( u16 p_bank );

        /*
         * @brief: Reads the specified map slice from the FS.
         * @param p_path: If specified, the map is read from the specified path, otherwise
         * it will be read from the default location (FSROOT/MAPS/p_bank[/p_mapY]/p_mapY_p_mapX.map)
         * in the current working directory.
         */
        bool readMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path = "",
                           bool p_readMapData = true );

        /*
         * @brief: Reads all map slices of the specified map bank from the FS.
         * @param p_forceReread: If true, ignores and overwrites any previously read data.
         */
        bool readMapBank( u16 p_bank, bool p_forceReread = false );

        /*
         * @brief: Loads the specified map bank in the editor. Lazily reads the data from
         * the FS, one map bank at a time.
         */
        void loadMapBank( u16 p_bank );

        /*
         * @brief: Loads the specified bank and map.
         */
        void loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX );

        /*
         * @brief: Cleanup operations when the specified map gets unloaded (update map bank
         * overview)
         */
        void onUnloadMap( u16 p_bank, u8 p_mapY, u8 p_mapX );

        /*
         * @brief: Displays the specified map of the currently selected map.
         */
        void redrawMap( u8 p_mapY, u8 p_mapX );

        /*
         * @brief: Loads the map in the current bank that lies at the relative position
         * (p_dx, p_dy). Does nothing if the new coordinates are (partially) negative or larger
         * than 99; extends the map bank if the new coordinates lie outside of the current
         * map bank bounds, otherwise.
         */
        void moveToMap( s8 p_dy, s8 p_dx );

        /*
         * @brief: Adds a new map bank to the FSROOT and the editor, unloading any data
         * that may exist in the editor.
         */
        void addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX,
                            u8              p_mapMode = DATA::MAPMODE_COMBINED,
                            mapBank::status p_status  = mapBank::STATUS_UNTOUCHED );

        /*
         * @brief: Does nothing if p_bank is loaded, otherwise creates and loads a new
         * bank.
         */
        void createMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX );

        /*
         * @brief: Sets the status of the specified map bank to the specified status,
         * resulting in the specified map bank being highlighted in the sidebar.
         */
        void markBankChanged( u16 p_bank, mapBank::status p_newStatus
                                          = mapBank::status::STATUS_EDITED_UNSAVED );

        /*
         * @brief: Redraws the first block/tile set widget of the current map.
         */
        void currentMapUpdateTS1( u8 p_newTS );

        /*
         * @brief: Redraws the second block/tile set widget of the current map.
         */
        void currentMapUpdateTS2( u8 p_newTS );

        // Map utility

        /*
         * @brief: Checks if the specified block coordinates are currently visible
         * on-screen.
         */
        inline bool isInMapBounds( s16 p_blockX, s16 p_blockY, s8 p_mapX, s8 p_mapY ) {
            if( p_blockX < 0 || p_blockY < 0 ) { return false; }

            u16 wd = DATA::SIZE;
            u16 hg = DATA::SIZE;
            if( p_mapX ) { wd = _adjacentBlocks; }
            if( p_mapY ) { hg = _adjacentBlocks; }

            if( p_blockX >= wd || p_blockY >= hg ) { return false; }
            return true;
        }

        /*
         * @brief: Merges the two currently visible block sets into a single block set used
         * for rendering maps.
         */
        void buildBlockSet( DATA::blockSet<2>* p_out, s8 p_ts1 = -1, s8 p_ts2 = -1 );

        /*
         * @brief: Merges the two currently visible tile sets into a single tile set used
         * for rendering maps.
         */
        void buildTileSet( DATA::tileSet<2>* p_out, s8 p_ts1 = -1, s8 p_ts2 = -1 );

        /*
         * @brief: Merges the two currently visible palette sets into a single palette used
         * for rendering maps.
         */
        void buildPalette( DATA::palette p_out[ 5 * 16 ], s8 p_ts1 = -1, s8 p_ts2 = -1 );

        // Map editing

        /*
         * @brief: Sets the currently selected block / movement for drawing.
         */
        void updateSelectedBlock( DATA::mapBlockAtom p_block );

        /*
         * @brief: Handles clicks on maps:
         * Left: Change block to currently selected block (if p_allowChange)
         * Middle: Change block and recursively all adjacent blocks that are the same as
         * the original block (flood fill)
         * Right: Select block
         */
        void onMapClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

        /*
         * @brief: Handles clicks of blocks of the tile/block set widget.
         */
        void onTSClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, u8 p_ts );

        void onTSETSClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                             u8 p_ts );

        void onMapDragStart( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                             s8 p_mapX, s8 p_mapY, bool p_allowEdit = true );
        void onMapDragUpdate( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                              s8 p_mapY, bool p_allowEdit = true );
        void onMapDragEnd( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

        //////////////////////////////////////////////////////////////////////////////////
        //
        // Map bank overview
        //
        //////////////////////////////////////////////////////////////////////////////////

        /*
         * @brief: Sets the mode of the bank overview
         */
        void setNewBankOverviewMode( bankOverviewMode p_newMode );

        // Static methods

        /*
         * @brief: Creates a new Gtk::Button with an icon and a string and the given
         * handler for the clicked signal.
         */
        static auto createButton(
            const std::string& p_iconName, const std::string& p_labelText,
            std::function<void( )> p_callBack = []( ) {} );

        /*
         * @brief: Walks through the specified directory and its subdirectory to explore
         * how large a map bank is and whether its maps are distributed over subfolders.
         */
        static DATA::mapBankInfo exploreMapBank( const fs::path& p_path );

        /*
         * @brief: Checks if the specified path exists or creates it if it doesn't.
         */
        static bool checkOrCreatePath( const std::string& p_path );
    };
} // namespace UI
