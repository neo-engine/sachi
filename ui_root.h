#pragma once
#include <map>
#include <memory>
#include <set>
#include <tuple>

#include <gtkmm/actionbar.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/iconview.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/notebook.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/window.h>

#include "data_maprender.h"
#include "ui_mapBank.h"
#include "ui_mapSlice.h"

namespace UI {
    class root : public Gtk::Window {
        struct mapBankInfo {
            std::shared_ptr<mapBank> m_widget = nullptr;
            bool                     m_loaded = false;
            bool m_scattered = true; // maps are scattered over subfolders according to y
            std::shared_ptr<DATA::mapBank> m_bank  = nullptr;
            u8                             m_sizeX = 0;
            u8                             m_sizeY = 0;
        };

        struct blockSetInfo {
            DATA::blockSet<1> m_blockSet;
            DATA::tileSet<1>  m_tileSet;
            DATA::palette     m_pals[ 8 * 5 ];
            u8                m_stringListItem = 0;
        };

        struct mapBankData {
            u8   m_sizeX     = 0;
            u8   m_sizeY     = 0;
            bool m_scattered = true;
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

        recentFsRootModelColumn             _recentViewColumns;
        Gtk::IconView                       _recentFsRootIconView;
        Glib::RefPtr<Gtk::ListStore>        _recentFsRootListModel;
        std::shared_ptr<Gtk::RecentManager> _recentlyUsedFsRoots;

        Gtk::ScrolledWindow _ivScrolledWindow;
        Gtk::Label          _loadMapLabel;
        Gtk::Box            _mainBox, _mapOverviewBox;
        Gtk::Box            _mapEditorBlockSetBox{ Gtk::Orientation::VERTICAL },
            _mapEditorMapBox{ Gtk::Orientation::VERTICAL };
        Gtk::DropDown                _mapEditorBS1CB, _mapEditorBS2CB;
        Gtk::Notebook                _mapNotebook;
        std::shared_ptr<Gtk::Button> _saveButton, _openButton;
        std::shared_ptr<Gtk::Button> _collapseMapBanksButton;
        Gtk::Box                     _mapBankBox;
        std::shared_ptr<addMapBank>  _addMapBank;
        Gtk::Grid                    _mapGrid;
        Gtk::ActionBar               _mapEditorActionBar;

        Gtk::SpinButton _mapEditorSettings1;
        Gtk::SpinButton _mapEditorSettings2;
        Gtk::SpinButton _mapEditorSettings3;
        Gtk::SpinButton _mapEditorSettings4;
        Gtk::SpinButton _mapEditorSettings5;
        Gtk::SpinButton _mapEditorSettings6;

        std::shared_ptr<Gtk::Button> _mapNavButton[ 3 ][ 3 ];

        mapSlice _ts1widget, _ts2widget;

        std::shared_ptr<Gtk::StringList> _mapBankStrList;

        std::vector<std::vector<mapSlice>> _currentMap; // main map and parts of the adjacent maps
        std::vector<DATA::computedBlock>   _currentBlockset1;
        std::vector<DATA::computedBlock>   _currentBlockset2;

        // bank -> bankinfo
        std::map<u16, mapBankInfo> _mapBanks;

        int  _selectedBank = -1;
        s16  _selectedMapX = -1, _selectedMapY = -1;
        bool _fsRootLoaded = false;

        u8   _currentDayTime  = 0;
        u8   _blockScale      = 1;
        u8   _blockSpacing    = 0;
        u8   _neighborSpacing = 10;
        bool _showAdjacent    = true;
        u16  _blockSetWidth   = 8;
        u8   _adjacentBlocks  = 8;

        std::tuple<u16, u16, s8, s8> _dragStart;
        std::tuple<u16, u16, s8, s8> _dragEnd;

        bool _mapBankBarCollapsed = false;
        bool _disableRedraw       = true;

        bool _focusMode = false;

        std::map<u8, blockSetInfo> _blockSets;
        std::set<u8>               _blockSetNames;

        DATA::mapBlockAtom  _currentlySelectedBlock = DATA::mapBlockAtom( );
        DATA::computedBlock _currentlySelectedComputedBlock;

      public:
        root( );
        ~root( ) override;

        void loadNewFsRoot( const std::string& p_path );

      private:
        void onFsRootOpenClick( );
        void onFsRootSaveClick( );

        void onFolderDialogResponse( int p_responseId, Gtk::FileChooserDialog* p_dialog );

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
         * @brief: Handles clicks on maps:
         * Left: Change block to currently selected block (if p_allowChange)
         * Middle: Change block and recursively all adjacent blocks that are the same as
         * the original block (flood fill)
         * Right: Select block
         */
        void onMapClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

        void onTSClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY, u8 p_ts );

        void onMapDragStart( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                             s8 p_mapX, s8 p_mapY, bool p_allowEdit = true );
        void onMapDragUpdate( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                              s8 p_mapY, bool p_allowEdit = true );
        void onMapDragEnd( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                           s8 p_mapY, bool p_allowEdit = true );

        void onBlockSetClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                                u8 p_tsIdx );

        void loadMapBank( u16 p_bank );

        void collapseMapBankBar( bool p_collapse = true );

        void moveToMap( s8 p_dy, s8 p_dx );

        void updateSelectedBlock( DATA::mapBlockAtom p_block );

        /*
         * @brief: Loads the specified bank and map.
         */
        void loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX );

        /*
         * @brief: Displays the specified map of the currently selected map.
         */
        void redrawMap( u8 p_mapY, u8 p_mapX );

        void addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, bool p_scattered = false,
                            mapBank::status p_status = mapBank::STATUS_UNTOUCHED );

        void createMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX );

        void markBankChanged( u16 p_bank, mapBank::status p_newStatus
                                          = mapBank::status::STATUS_EDITED_UNSAVED );

        void currentMapUpdateTS1( u8 p_newTS );
        void currentMapUpdateTS2( u8 p_newTS );

        void buildBlockSet( DATA::blockSet<2>* p_out );
        void buildTileSet( DATA::tileSet<2>* p_out );
        void buildPalette( DATA::palette p_out[ 5 * 16 ] );

        void addFsRootToRecent( const std::string& p_path );
        void removeFsRootFromRecent( const std::string& p_path );
        auto getRecentFsRoots( );

        void populateRecentFsRootIconView( );

        static auto createButton(
            const std::string& p_iconName, const std::string& p_labelText,
            std::function<void( )> p_callBack = []( ) {} );

        static mapBankData exploreMapBank( const fs::path& p_path );

        static bool checkOrCreatePath( const std::string& p_path );
    };
} // namespace UI
