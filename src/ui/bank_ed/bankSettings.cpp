#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "../../log.h"
#include "../root.h"
#include "bankSettings.h"

namespace UI {
    bankSettings::bankSettings( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mapSettingsBox.set_margin( MARGIN );

        auto shbox1f1 = Gtk::Frame( "General Settings" );
        shbox1f1.set_label_align( Gtk::Align::CENTER );

        auto sboxv1 = Gtk::Box( Gtk::Orientation::VERTICAL );
        shbox1f1.set_child( sboxv1 );
        sboxv1.set_margin( MARGIN );

        auto shbox1 = Gtk::CenterBox( );
        shbox1.set_hexpand( true );
        auto shbox1l = Gtk::Label( "Map Mode" );
        shbox1.set_start_widget( shbox1l );

        _mapBankSettingsMapModeToggles = std::make_shared<switchButton>(
            std::vector<std::string>{ "_Simple", "Sc_attered", "_Combined" } );

        if( _mapBankSettingsMapModeToggles ) {
            ( (Gtk::Widget&) ( *_mapBankSettingsMapModeToggles ) ).set_margin_end( 0 );
            ( (Gtk::Widget&) ( *_mapBankSettingsMapModeToggles ) ).set_margin_top( 0 );
            shbox1.set_end_widget( *_mapBankSettingsMapModeToggles );
            _mapBankSettingsMapModeToggles->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.setCurrentBankOWStatus( p_newChoice );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( shbox1 );

        _shbox2.set_hexpand( true );
        auto shbox2l = Gtk::Label( "Bank is Overworld" );
        _shbox2.set_start_widget( shbox2l );

        _mapIsOW = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
        if( _mapIsOW ) {
            ( (Gtk::Widget&) ( *_mapIsOW ) ).set_margin_end( 0 );
            ( (Gtk::Widget&) ( *_mapIsOW ) ).set_margin_top( 0 );
            _shbox2.set_end_widget( *_mapIsOW );
            _mapIsOW->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.bank( ).setOWStatus( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( _shbox2 );

        _shbox3.set_hexpand( true );
        auto shbox3l = Gtk::Label( "Default Overworld Location" );
        _shbox3.set_start_widget( shbox3l );
        _bankDefaultLocation = std::make_shared<locationDropDown>( );
        if( _bankDefaultLocation ) {
            _shbox3.set_end_widget( *_bankDefaultLocation );
            _bankDefaultLocation->connect( [ this ]( u64 p_newChoice ) {
                _model.bank( ).setDefaultLocation( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( _shbox3 );

        _shbox4.set_hexpand( true );
        auto shbox4l = Gtk::Label( "Bank has Underwater" );
        _shbox4.set_start_widget( shbox4l );

        _mapHasDive = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
        if( _mapHasDive ) {
            ( (Gtk::Widget&) ( *_mapHasDive ) ).set_margin_end( 0 );
            ( (Gtk::Widget&) ( *_mapHasDive ) ).set_margin_top( 0 );
            _shbox4.set_end_widget( *_mapHasDive );
            _mapHasDive->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.bank( ).setDiveStatus( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        _shbox4.set_margin_top( MARGIN );
        sboxv1.append( _shbox4 );

        _mapSettingsBox.append( shbox1f1 );
    }

    void bankSettings::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        if( _mapBankSettingsMapModeToggles ) {
            _mapBankSettingsMapModeToggles->choose( _model.bank( ).getMapMode( ) );
        }

        if( _model.selectedBankIsDive( ) ) {
            _shbox2.hide( );
            _shbox3.hide( );
            _shbox4.hide( );
        } else {
            _shbox2.show( );
            _shbox4.show( );

            if( _mapIsOW ) { _mapIsOW->choose( _model.bank( ).getOWStatus( ) ); }
            if( _mapHasDive ) { _mapHasDive->choose( _model.bank( ).getDiveStatus( ) ); }
            if( _bankDefaultLocation ) {
                _bankDefaultLocation->refreshModel( _model );
                _bankDefaultLocation->choose( _model.bank( ).getDefaultLocation( ) );
            }
            if( _model.bank( ).getOWStatus( ) ) {
                // draw the ow map, default location selector
                _shbox3.show( );
            } else {
                _shbox3.hide( );
            }
        }
    }

} // namespace UI
