#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "mapPosition.h"

namespace UI {
    mapPosition::mapPosition( bool p_allowSliceEdit )
        : _bankA( Gtk::Adjustment::create( 0.0, 10.0, 99.0, 1.0, 5.0, 0.0 ) ),
          _mapXA( Gtk::Adjustment::create( 0.0, 0.0, 99.0, 1.0, 5.0, 0.0 ) ),
          _localXA( Gtk::Adjustment::create( 0.0, 0.0, 31.0, 1.0, 1.0, 0.0 ) ),
          _globalXA( Gtk::Adjustment::create( 0.0, 0.0, 99.0 * 31.0, 1.0, 1.0, 0.0 ) ),
          _mapYA( Gtk::Adjustment::create( 0.0, 0.0, 99.0, 1.0, 5.0, 0.0 ) ),
          _localYA( Gtk::Adjustment::create( 0.0, 0.0, 31.0, 1.0, 1.0, 0.0 ) ),
          _globalYA( Gtk::Adjustment::create( 0.0, 0.0, 99.0 * 31.0, 1.0, 1.0, 0.0 ) ),
          _posZA( Gtk::Adjustment::create( 0.0, 0.0, 15.0, 1.0, 1.0, 0.0 ) ), _bankE( _bankA ),
          _mapXE( _mapXA ), _localXE( _localXA ), _globalXE( _globalXA ), _mapYE( _mapYA ),
          _localYE( _localYA ), _globalYE( _globalYA ), _posZE( _posZA ) {

        Gtk::Grid g1;

        g1.set_margin_end( MARGIN );
        _mainBox.append( g1 );

        Gtk::Label lm{ " Map" }, lx{ "X" }, ly{ "Y" }, lz{ "Z" };
        lm.set_margin_end( MARGIN );
        lx.set_margin_end( MARGIN );
        lx.set_margin_start( MARGIN );

        if( p_allowSliceEdit ) { g1.attach( lm, 0, 0 ); }
        g1.attach( lx, 0, 2 );
        g1.attach( ly, 0, 1 );
        g1.attach( lz, 0, 3 );

        Gtk::Box bx{ Gtk::Orientation::HORIZONTAL }, by{ Gtk::Orientation::HORIZONTAL };

        if( !p_allowSliceEdit ) {
            _mapXE.set_editable( false );
            _mapYE.set_editable( false );
        }

        bx.append( _localXB );
        _localXB.append( _mapXE );
        _localXB.append( _localXE );
        _localXE.set_hexpand( true );
        _mapXE.set_hexpand( true );
        bx.append( _globalXE );
        by.append( _localYB );
        _localYB.append( _mapYE );
        _localYB.append( _localYE );
        _localYE.set_hexpand( true );
        _mapYE.set_hexpand( true );
        by.append( _globalYE );

        _localXB.get_style_context( )->add_class( "linked" );
        _localYB.get_style_context( )->add_class( "linked" );

        if( p_allowSliceEdit ) { g1.attach( _bankE, 1, 0 ); }
        g1.attach( bx, 1, 2 );
        g1.attach( by, 1, 1 );
        g1.attach( _posZE, 1, 3 );

        _localXB.set_hexpand( );
        _localYB.set_hexpand( );
        _globalXE.set_hexpand( );
        _globalYE.set_hexpand( );
        _bankE.set_hexpand( );
        _posZE.set_hexpand( );
        g1.set_hexpand( false );

        _modeSwitch
            = std::make_shared<switchButton>( std::vector<std::string>{ "Local", "Global" } );
        if( _modeSwitch ) {
            _mainBox.append( *_modeSwitch );
            _modeSwitch->connect( [ & ]( u8 p_choice ) {
                switch( p_choice ) {
                case 0:
                default: setDisplayMode( displayMode::LOCAL_POSITION ); break;
                case 1: setDisplayMode( displayMode::GLOBAL_POSITION ); break;
                }
            } );
        }

        setDisplayMode( displayMode::LOCAL_POSITION );
    }
} // namespace UI
