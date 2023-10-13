#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../defines.h"
#include "../../model.h"

namespace UI {
    /*
     * @brief: A widget to draw a single map bank
     */
    class mapBank {
      protected:
        model& _model;

        u16    _bankName = 0;
        u32    _sizeX = 0, _sizeY = 0;
        bool   _selected      = false;
        status _currentStatus = STATUS_UNTOUCHED;
        bool   _collapsed     = false;

        Gtk::Frame _outerFrame;
        Gtk::Box _nameBox{ Gtk::Orientation::HORIZONTAL }, _entryBox{ Gtk::Orientation::VERTICAL };
        Gtk::Box _nameBoxD{ Gtk::Orientation::HORIZONTAL };
        Gtk::Label                       _nameLabel;
        Gtk::Label                       _nameLabelD;
        std::shared_ptr<Gtk::Adjustment> _mapXAdj, _mapYAdj;
        Gtk::SpinButton                  _mapXEntry, _mapYEntry;
        Gtk::Button                      _loadMapButton{ "Load" }, _loadDiveMapButton{ "Dive" };

        // mapBank( model& p_model, const std::string& p_yicon = "view-more-symbolic",
        //          const std::string& p_xicon = "content-loading-symbolic" );

        mapBank( model& p_model, const std::string& p_yicon, const std::string& p_xicon );

        mapBank( model& p_model, const std::string& p_yicon );

      public:
        mapBank( model& p_model, u16 p_bankname, u32 p_sizeX = 0, u32 p_sizeY = 0,
                 status p_initialStatus = STATUS_UNTOUCHED );

        virtual inline ~mapBank( ) {
        }

        virtual inline void setSelectedMap( u32 p_x, u32 p_y ) {
            _mapXEntry.set_value( p_x );
            _mapYEntry.set_value( p_y );
        }

        virtual inline void redraw( ) {
            setSelected( _model.selectedBank( ) % DIVE_MAP == _bankName );
            collapse( _model.m_settings.m_focusMode || _collapsed );
            setSizeX( _model.sizeX( _bankName ) );
            setSizeY( _model.sizeY( _bankName ) );
            setStatus( _model.bankStatus( _bankName ) );
            if( _loadDiveMapButton.is_visible( ) && _model.existsBank( _bankName + DIVE_MAP ) ) {
                setDiveStatus( _model.bankStatus( _bankName + DIVE_MAP ) );
                _nameBoxD.show( );
            } else {
                _nameBoxD.hide( );
            }
        }

        virtual inline void collapse( bool p_collapsed = true ) {
            if( p_collapsed ) {
                _entryBox.hide( );
                _loadMapButton.hide( );
                _loadDiveMapButton.hide( );
            } else {
                _entryBox.show( );
                _loadMapButton.show( );
                if( _model.existsBank( _bankName ) && _model.bank( _bankName ).getDiveStatus( ) ) {
                    _loadDiveMapButton.show( );
                } else {
                    _loadDiveMapButton.hide( );
                }
            }
            _collapsed = p_collapsed;
        }

        inline bool getSelected( ) const {
            return _selected;
        }

        inline void setSelected( bool p_selected = true ) {
            if( p_selected ) {
                select( );
            } else {
                unselect( );
            }
        }

        void select( );
        void unselect( );

        inline u16 getBankName( ) const {
            return _bankName;
        }

        inline void setBankName( u16 p_bankName ) {
            _bankName = p_bankName;
            _nameLabel.set_markup( "<span size=\"x-large\" weight=\"bold\">"
                                   + std::to_string( p_bankName ) + "</span>" );
            _nameLabelD.set_markup( "<span size=\"x-large\" weight=\"bold\">"
                                    + std::to_string( p_bankName ) + "U</span>" );
        }

        inline u32 getSizeX( ) const {
            return _sizeX;
        }

        inline void setSizeX( u32 p_sizeX ) {
            _sizeX = p_sizeX;
            _mapXAdj->set_upper( p_sizeX );
        }

        inline u32 getSizeY( ) const {
            return _sizeY;
        }

        inline void setSizeY( u32 p_sizeY ) {
            _sizeY = p_sizeY;
            _mapYAdj->set_upper( p_sizeY );
        }

        inline status getStatus( ) const {
            return _currentStatus;
        }

        inline void setStatus( status p_status ) {
            _nameBox.get_style_context( )->remove_class( "mapbank-saved" );
            _nameBox.get_style_context( )->remove_class( "mapbank-created" );
            _nameBox.get_style_context( )->remove_class( "mapbank-modified" );

            _currentStatus = p_status;
            switch( _currentStatus ) {
            case STATUS_SAVED: _nameBox.get_style_context( )->add_class( "mapbank-saved" ); break;
            case STATUS_NEW: _nameBox.get_style_context( )->add_class( "mapbank-created" ); break;
            case STATUS_EDITED_UNSAVED:
                _nameBox.get_style_context( )->add_class( "mapbank-modified" );
                break;
            default: break;
            }
        }

        inline void setDiveStatus( status p_status ) {
            _nameBoxD.get_style_context( )->remove_class( "mapbank-saved" );
            _nameBoxD.get_style_context( )->remove_class( "mapbank-created" );
            _nameBoxD.get_style_context( )->remove_class( "mapbank-modified" );

            switch( p_status ) {
            case STATUS_SAVED: _nameBoxD.get_style_context( )->add_class( "mapbank-saved" ); break;
            case STATUS_NEW: _nameBoxD.get_style_context( )->add_class( "mapbank-created" ); break;
            case STATUS_EDITED_UNSAVED:
                _nameBoxD.get_style_context( )->add_class( "mapbank-modified" );
                break;
            default: break;
            }
        }

        virtual inline void connect( const std::function<void( u16, u8, u8 )>& p_callback ) {
            if( !p_callback ) { return; }

            _loadMapButton.signal_clicked( ).connect( [ this, p_callback ]( ) {
                p_callback( _bankName, _mapYEntry.get_value_as_int( ),
                            _mapXEntry.get_value_as_int( ) );
            } );
            _loadDiveMapButton.signal_clicked( ).connect( [ this, p_callback ]( ) {
                p_callback( _bankName + DIVE_MAP, _mapYEntry.get_value_as_int( ),
                            _mapXEntry.get_value_as_int( ) );
            } );
        }

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }
    };

    class addMapBank : public mapBank {
        std::shared_ptr<Gtk::Adjustment> _nameAdj;
        Gtk::SpinButton                  _nameEntry;

      public:
        addMapBank( model& p_model );

        inline ~addMapBank( ) override {
        }

        inline void redraw( ) override {
            collapse( _model.m_settings.m_focusMode || _collapsed );
        }

        inline void connect( const std::function<void( u16, u8, u8 )>& p_callback ) override {
            if( !p_callback ) { return; }

            _loadMapButton.signal_clicked( ).connect( [ this, p_callback ]( ) {
                p_callback( _nameEntry.get_value_as_int( ), _mapYEntry.get_value_as_int( ),
                            _mapXEntry.get_value_as_int( ) );
            } );
        }
    };

    class editTileSet : public mapBank {
        // mapy: ts0; mapx: ts1
      public:
        editTileSet( model& p_model );

        inline ~editTileSet( ) override {
        }

        inline void redraw( ) override {
            collapse( _model.m_settings.m_focusMode || _collapsed );
            setStatus( _model.tileStatus( ) );
        }

        inline void connect( const std::function<void( u8, u8 )>& p_callback ) {
            if( !p_callback ) { return; }

            _loadMapButton.signal_clicked( ).connect( [ this, p_callback ]( ) {
                p_callback( _mapYEntry.get_value_as_int( ), _mapXEntry.get_value_as_int( ) );
            } );
        }
    };

    class editTrainer : public mapBank {
        // mapy: trainer idx, mapx: unused
      public:
        editTrainer( model& p_model, std::string p_name = "TR" );

        inline ~editTrainer( ) override {
        }

        inline void redraw( ) override {
            collapse( _model.m_settings.m_focusMode || _collapsed );
            setStatus( _model.trainerStatus( ) );
        }

        inline void connect( const std::function<void( u16 )>& p_callback ) {
            if( !p_callback ) { return; }

            _loadMapButton.signal_clicked( ).connect(
                [ this, p_callback ]( ) { p_callback( _mapYEntry.get_value_as_int( ) ); } );
        }
    };

} // namespace UI
