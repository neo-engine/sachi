#pragma once
#include <map>
#include <memory>
#include <set>

#include <giomm/liststore.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/gridview.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/window.h>

#include "data_maprender.h"
#include "ui_blockSet.h"
#include "ui_mapBank.h"

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
            blockSet *m_bs1widget = nullptr, *m_bs2widget = nullptr;

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

        Gtk::Label _loadMapLabel;
        Gtk::Box   _mainBox, _mapOverviewBox;
        Gtk::Box   _mapEditorBlockSetBox{ Gtk::Orientation::VERTICAL },
            _mapEditorMapBox{ Gtk::Orientation::VERTICAL };
        Gtk::DropDown                _mapEditorBS1CB, _mapEditorBS2CB;
        Gtk::GridView                _mapView;
        Gtk::Notebook                _mapNotebook;
        std::shared_ptr<Gtk::Button> _saveButton, _openButton;
        Gtk::Box                     _mapBankBox;
        std::shared_ptr<addMapBank>  _addMapBank;

        std::shared_ptr<Gtk::StringList>            _mapBankStrList;
        std::shared_ptr<Gio::ListStore<Gtk::Image>> _currentMap;
        std::shared_ptr<Gtk::SignalListItemFactory> _mapFactory;

        // bank -> bankinfo
        std::map<u16, mapBankInfo> _mapBanks;

        int  _selectedBank = -1;
        bool _fsRootLoaded = false;

        u8 _currentDayTime = 0;
        u8 _blockScale     = 1;

        std::map<u8, blockSetInfo> _blockSets;
        std::set<u8>               _blockSetNames;

      public:
        root( );
        ~root( ) override;

      private:
        void onFsRootOpenClick( );
        void onFsRootSaveClick( );

        void onFolderDialogResponse( int p_responseId, Gtk::FileChooserDialog* p_dialog );

        void loadNewFsRoot( const std::string& p_path );

        void loadMapBank( u16 p_bank );

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

        static auto createButton(
            const std::string& p_iconName, const std::string& p_labelText,
            std::function<void( )> p_callBack = []( ) {} );

        static mapBankData exploreMapBank( const fs::path& p_path );

        static bool checkOrCreatePath( const std::string& p_path );
    };
} // namespace UI
