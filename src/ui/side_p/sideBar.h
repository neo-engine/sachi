#pragma once
#include <map>
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>

#include "../../defines.h"
#include "../../model.h"
#include "mapBank.h"

namespace UI {
    class root;

    /*
     * @brief: The side panel widget used for loading the bank editor or other subtools.
     */
    class sideBar {
        model&  _model;
        root&   _rootWindow;
        context _context = CONTEXT_NONE;

        Gtk::Box                     _lMainBox;
        std::shared_ptr<Gtk::Button> _collapseMapBanksButton; // collapse/show sidebar

        bool _mapBankBarCollapsed = false;

        //////////////////////////////////////////////////////////////////////////////////
        //
        // side bar -> trainer editor
        //
        //////////////////////////////////////////////////////////////////////////////////

        Gtk::Label _sbTrainerBarLabel;
        Gtk::Box   _sbTrainerBox;
        s16        _sbTrainerSel1 = 0;

        std::shared_ptr<editTrainer> _editTrainer;

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

      public:
        sideBar( model& p_model, root& p_root );

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

        inline void switchContext( context p_context ) {
            _context = p_context;
            redraw( );
        }

        /*
         * @brief: Redraws all existing widgets in the sidebar, updating their status to
         * match the corresponding values in the model.
         */
        void redraw( );

        /*
         * @brief: Re-reads the model and reconstructs any mapBank widgets
         */
        void reinit( );

        void addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, status p_status );
    };
} // namespace UI
