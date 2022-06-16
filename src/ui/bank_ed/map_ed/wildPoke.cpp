#include <string>
#include <vector>

#include <gtkmm/centerbox.h>
#include "../../root.h"
#include "wildPoke.h"

namespace UI::MED {
    wildPoke::wildPoke( model& p_model, root& p_root, u8 p_slot )
        : _model{ p_model }, _rootWindow{ p_root }, _slot{ p_slot },
          _encRateA{ Gtk::Adjustment::create( 0.0, 0.0, 100.0, 1.0, 10.0, 0.0 ) },
          _reqBadgesA{ Gtk::Adjustment::create( 0.0, 0.0, 255.0, 1.0, 5.0, 0.0 ) },
          _encRate{ _encRateA }, _reqBadges{ _reqBadgesA } {

        auto mainBox = Gtk::Box{ Gtk::Orientation::HORIZONTAL };

        mainBox.set_valign( Gtk::Align::CENTER );
        mainBox.set_hexpand( );

        _outerFrame.set_child( mainBox );
        _outerFrame.set_margin_start( MARGIN );
        _outerFrame.set_margin_end( MARGIN );
        _outerFrame.set_margin_bottom( MARGIN );

        auto navBox = Gtk::Box{ Gtk::Orientation::VERTICAL };
        mainBox.append( navBox );
        navBox.get_style_context( )->add_class( "linked" );
        navBox.set_valign( Gtk::Align::CENTER );

        _upB.set_icon_name( "pan-up-symbolic" );
        _downB.set_icon_name( "pan-down-symbolic" );
        _removeB.set_icon_name( "window-close-symbolic" );

        navBox.append( _upB );
        // navBox.append( _duplicateB );
        navBox.append( _removeB );
        navBox.append( _downB );

        _pkmnChooser = std::make_shared<pokeSelector>( _model );
        if( _pkmnChooser ) { mainBox.append( *_pkmnChooser ); }

        mainBox.append( _s1 );
        _s1.set_margin( MARGIN );

        _encBox.set_hexpand( true );

        mainBox.append( _encBox );

        Gtk::CenterBox ercb{ };
        Gtk::Label     erl{ "Enc Rate" };
        ercb.set_start_widget( erl );
        ercb.set_end_widget( _encRate );

        Gtk::CenterBox rbcb{ };
        Gtk::Label     rbl{ "Req Badges" };
        rbcb.set_start_widget( rbl );
        rbcb.set_end_widget( _reqBadges );

        _encBox.set_valign( Gtk::Align::CENTER );

        _encBox.append( ercb );
        _encBox.append( rbcb );

        std::vector<std::string> wtn{ };
        for( u8 t{ DATA::wildPkmnType::_TP_FIRST }; t <= DATA::wildPkmnType::_TP_LAST; ++t ) {
            wtn.push_back( DATA::wildPkmnTypeName( DATA::wildPkmnType{ t } ) );
        }
        _encType = std::make_shared<dropDown>( wtn );
        if( _encType ) { _encBox.append( *_encType ); }

        _encTimes = std::make_shared<multiButton>(
            std::vector<std::string>{ "Night", "Dawn", "Day", "Dusk" },
            []( u32 p_os, u8 p_nc ) { return p_os ^= ( 1 << p_nc ); }, 0,
            Gtk::Orientation::VERTICAL );
        if( _encTimes ) {
            mainBox.append( *_encTimes );
            ( (Gtk::Widget&) ( *_encTimes ) ).set_hexpand( false );
            ( (Gtk::Widget&) ( *_encTimes ) ).set_margin( 0 );
            ( (Gtk::Widget&) ( *_encTimes ) ).set_margin_start( MARGIN );
        }

        if( _pkmnChooser ) {
            _pkmnChooser->connect( [ this ]( pokeSelector::pkmnDscr ) {
                if( _lock ) { return; }
                _model.encounterData( _slot ) = getData( );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }

        if( _encType ) {
            _encType->connect( [ this ]( u64 ) {
                if( _lock ) { return; }
                _model.encounterData( _slot ) = getData( );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }

        if( _encTimes ) {
            _encTimes->connect( [ this ]( u32 ) {
                if( _lock ) { return; }
                _model.encounterData( _slot ) = getData( );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }

        _encRateA->signal_value_changed( ).connect( [ this ]( ) {
            if( _lock ) { return; }
            _model.encounterData( _slot ) = getData( );
            _model.markSelectedBankChanged( );
            _rootWindow.redraw( );
        } );
        _reqBadgesA->signal_value_changed( ).connect( [ this ]( ) {
            if( _lock ) { return; }
            _model.encounterData( _slot ) = getData( );
            _model.markSelectedBankChanged( );
            _rootWindow.redraw( );
        } );
    }

    void wildPoke::setData( const DATA::mapData::wildPkmnData& p_data ) {
        _lock = true;
        if( _pkmnChooser ) { _pkmnChooser->setData( { p_data.m_speciesId, p_data.m_forme } ); }
        _reqBadges.set_value( p_data.m_slot );
        if( _encTimes ) { _encTimes->choose( p_data.m_daytime ); }
        _encRate.set_value( p_data.m_encounterRate );
        if( _encType ) { _encType->choose( p_data.m_encounterType ); }

        for( u8 t{ DATA::wildPkmnType::_TP_FIRST }; t <= DATA::wildPkmnType::_TP_LAST; ++t ) {
            _outerFrame.get_style_context( )->remove_class(
                classForEncType( DATA::wildPkmnType{ t } ) );
        }

        if( _pkmnChooser && !_pkmnChooser->getData( ).first ) {
            // no pkmn selected, compress widget

            _removeB.hide( );
            if( _encTimes ) { ( (Gtk::Widget&) ( *_encTimes ) ).hide( ); }
            _encBox.hide( );
            _s1.hide( );
        } else {
            _removeB.show( );
            if( _encTimes ) { ( (Gtk::Widget&) ( *_encTimes ) ).show( ); }
            _encBox.show( );
            _s1.show( );

            _outerFrame.get_style_context( )->add_class(
                classForEncType( p_data.m_encounterType ) );
        }

        _lock = false;
    }

    void wildPoke::redraw( ) {
        if( _pkmnChooser ) { _pkmnChooser->refreshModel( ); }
        setData( _model.encounterData( _slot ) );
    }
} // namespace UI::MED
