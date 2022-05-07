#pragma once
#include <map>
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>

#include "../../model.h"
#include "mapBank.h"

namespace UI {
    /*
     * @brief: The side panel widget used for loading the bank editor or other subtools.
     */
    class sideBar {
        model& _model;

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

        std::map<u16, std::shared_ptr<mapBank>> _mapBanks; // bank -> bankinfo
        int                                     _selectedBank = -1;
        s16                                     _selectedMapX = -1, _selectedMapY = -1;

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
