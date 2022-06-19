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
            shbox1.set_end_widget( *_mapBankSettingsMapModeToggles );
            _mapBankSettingsMapModeToggles->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.setCurrentBankOWStatus( p_newChoice );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( shbox1 );

        auto shbox2 = Gtk::CenterBox( );
        shbox2.set_hexpand( true );
        auto shbox2l = Gtk::Label( "Bank is Overworld" );
        shbox2.set_start_widget( shbox2l );

        _mapIsOW = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
        if( _mapIsOW ) {
            shbox2.set_end_widget( *_mapIsOW );
            _mapIsOW->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.bank( ).setOWStatus( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( shbox2 );

        _mapSettingsBox.append( shbox1f1 );
    }

    void bankSettings::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        if( _mapBankSettingsMapModeToggles ) {
            _mapBankSettingsMapModeToggles->choose( _model.bank( ).getMapMode( ) );
        }
        if( _mapIsOW ) { _mapIsOW->choose( _model.bank( ).getOWStatus( ) ); }
        if( _model.bank( ).getOWStatus( ) ) {
            // draw the ow map, default location selector
        }
    }

} // namespace UI
