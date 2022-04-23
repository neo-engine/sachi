#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../defines.h"

namespace UI {
    /*
     * @brief: A widget to draw a single map bank
     */
    class mapBank {
      public:
        enum status { STATUS_UNTOUCHED, STATUS_NEW, STATUS_EDITED_UNSAVED, STATUS_SAVED };

      protected:
        u16    _bankName = 0;
        u8     _sizeX = 0, _sizeY = 0;
        bool   _selected      = false;
        status _currentStatus = STATUS_UNTOUCHED;
        bool   _collapsed     = false;

        Gtk::Frame _outerFrame;
        Gtk::Box _nameBox{ Gtk::Orientation::HORIZONTAL }, _entryBox{ Gtk::Orientation::VERTICAL };
        Gtk::Label                       _nameLabel;
        std::shared_ptr<Gtk::Adjustment> _mapXAdj, _mapYAdj;
        Gtk::SpinButton                  _mapXEntry, _mapYEntry;
        Gtk::Button                      _loadMapButton{ "Load" };

        mapBank( const std::string& p_yicon = "view-more-symbolic",
                 const std::string& p_xicon = "content-loading-symbolic" );

      public:
        mapBank( u16 p_bankname, u8 p_sizeX = 0, u8 p_sizeY = 0,
                 status p_initialStatus = STATUS_UNTOUCHED );

        virtual inline ~mapBank( ) {
        }

        virtual inline void collapse( bool p_collapsed = true ) {
            if( p_collapsed ) {
                _entryBox.hide( );
                _loadMapButton.hide( );
            } else {
                _entryBox.show( );
                _loadMapButton.show( );
            }
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
        }

        inline u8 getSizeX( ) const {
            return _sizeX;
        }
        inline void setSizeX( u8 p_sizeX ) {
            _sizeX = p_sizeX;
            _mapXAdj->set_upper( p_sizeX );
        }
        inline u8 getSizeY( ) const {
            return _sizeY;
        }
        inline void setSizeY( u8 p_sizeY ) {
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

        virtual inline void connect( const std::function<void( u16, u8, u8 )>& p_callback ) {
            if( !p_callback ) { return; }

            _loadMapButton.signal_clicked( ).connect( [ this, p_callback ]( ) {
                p_callback( _bankName, _mapYEntry.get_value_as_int( ),
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
        addMapBank( );

        inline ~addMapBank( ) override {
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
        editTileSet( );

        inline ~editTileSet( ) override {
        }

        inline void connect( const std::function<void( u8, u8 )>& p_callback ) {
            if( !p_callback ) { return; }

            _loadMapButton.signal_clicked( ).connect( [ this, p_callback ]( ) {
                p_callback( _mapYEntry.get_value_as_int( ), _mapXEntry.get_value_as_int( ) );
            } );
        }
    };
} // namespace UI
