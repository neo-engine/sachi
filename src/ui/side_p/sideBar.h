#pragma once
#include <map>
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>

/*
#include <set>
#include <tuple>

#include <giomm/menu.h>
#include <giomm/menuitem.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/iconview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/notebook.h>
#include <gtkmm/popovermenu.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/window.h>
*/

#include "../../data/maprender.h"
#include "mapBank.h"

namespace UI {
    /*
     * @brief: The side panel widget used for loading the bank editor or other subtools.
     */
    class sideBar {
        friend class root;

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

        Gtk::Box                     _lMainBox;
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

      public:
        sideBar( );
        virtual inline ~sideBar( ) {
        }

        /*
         * @brief: Collapses/shows the sidebar.
         */
        void collapse( bool p_collapse = true );

        virtual inline void connect( const std::function<void( u8, u8 )>&      p_editTS,
                                     const std::function<void( u16, u8, u8 )>& p_addMB ) {
            if( p_editTS && _editTileSet ) { _editTileSet->connect( p_editTS ); }
            if( p_addMB && _addMapBank ) { _addMapBank->connect( p_addMB ); }
        }

        inline operator Gtk::Widget&( ) {
            return _lMainBox;
        }

        inline auto selectedBank( ) const {
            return _selectedBank;
        }

        inline void selectBank( int p_newSelection ) {
            _selectedBank = p_newSelection;
        }

        inline auto selectedMapX( ) const {
            return _selectedMapX;
        }

        inline auto selectedMapY( ) const {
            return _selectedMapY;
        }

        inline void selectMap( s16 p_mapX, s16 p_mapY ) {
            _selectedMapX = p_mapX;
            _selectedMapY = p_mapY;
        }

        inline auto selectedSizeX( ) const {
            return _mapBanks.at( selectedBank( ) ).getSizeX( );
        }

        inline auto selectedSizeY( ) const {
            return _mapBanks.at( selectedBank( ) ).getSizeY( );
        }

        inline void selectedSetSizeX( u8 p_sx ) {
            _mapBanks[ selectedBank( ) ].setSizeX( p_sx );
            _mapBanks[ selectedBank( ) ].m_widget->setSizeX( p_sx );
        }

        inline void selectedSetSizeY( u8 p_sy ) {
            _mapBanks[ selectedBank( ) ].setSizeY( p_sy );
            _mapBanks[ selectedBank( ) ].m_widget->setSizeY( p_sy );
        }

        inline bool existsBank( u16 p_bank ) {
            return _mapBanks.count( p_bank );
        }

        inline auto& bank( u16 p_bank ) {
            return _mapBanks[ p_bank ];
        }

        inline auto& bank( ) {
            return bank( selectedBank( ) );
        }

        inline auto& slice( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
            return bank( p_bank ).m_bank->m_slices[ p_mapY ][ p_mapX ];
        }

        inline auto& slice( ) {
            return slice( selectedBank( ), selectedMapY( ), selectedMapX( ) );
        }

        /*
         * @brief: Sets the status of the edit tile set widget to the specified status,
         * resulting in the TS being highlighted in the sidebar.
         */
        void markTileSetsChanged( mapBank::status p_newStatus
                                  = mapBank::status::STATUS_EDITED_UNSAVED );

        /*
         * @brief: Sets the status of the specified map bank to the specified status,
         * resulting in the specified map bank being highlighted in the sidebar.
         */
        void markBankChanged( u16 p_bank, mapBank::status p_newStatus
                                          = mapBank::status::STATUS_EDITED_UNSAVED );

        inline void markSelectedBankChanged( mapBank::status p_newStatus
                                             = mapBank::status::STATUS_EDITED_UNSAVED ) {
            markBankChanged( selectedBank( ), p_newStatus );
        }
    };
} // namespace UI
