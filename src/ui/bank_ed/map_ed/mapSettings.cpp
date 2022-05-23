#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>

#include "../../../data/maprender.h"
#include "../../../log.h"
#include "../../root.h"
#include "mapSettings.h"

namespace UI::MED {
    mapSettings::mapSettings( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        Gtk::ScrolledWindow sw{ };
        _mainBox.append( sw );
        Gtk::Box mbox{ Gtk::Orientation::VERTICAL };
        mbox.set_vexpand( );
        sw.set_child( mbox );
        sw.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );

        auto shbox1f1 = Gtk::Frame{ };
        shbox1f1.set_label_align( Gtk::Align::CENTER );

        auto sboxv1 = Gtk::Box{ Gtk::Orientation::VERTICAL };
        shbox1f1.set_child( sboxv1 );
        sboxv1.set_margin_start( MARGIN );

        auto shbox1 = Gtk::CenterBox{ };
        shbox1.set_hexpand( true );
        auto shbox1l = Gtk::Label{ "Map Mode" };
        shbox1.set_start_widget( shbox1l );
        shbox1.set_margin( MARGIN );

        _mapMode = std::make_shared<multiButton>(
            std::vector<std::string>{ "_Outside", "_Cave", "_Inside", "_Dark", "_Flash Usable" },
            []( u32 p_oldState, u8 p_newChoice ) -> u32 {
                auto res{ p_oldState ^ ( 1 << p_newChoice ) };
                if( p_newChoice < 3 ) {
                    if( res < p_oldState ) { // something got toggled off
                        if( !( res & 7 ) ) { res = 1; }
                    } else {
                        res = ( res & ~1 ) | ( 1 << p_newChoice );
                    }
                }
                if( res & 1 ) { res = 1; }
                if( res & 16 ) {
                    if( p_newChoice == 3 ) {
                        res &= 7;
                    } else {
                        res |= 8;
                    }
                }
                return res;
            } );

        if( _mapMode ) {
            shbox1.set_end_widget( *_mapMode );
            ( (Gtk::Widget&) *_mapMode ).set_margin( 0 );
            _mapMode->connect( [ this ]( u32 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.mapData( ).m_mapType = p_newChoice >> 1;
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }

        sboxv1.append( shbox1 );
        Gtk::Separator s1{ };
        s1.set_margin_end( MARGIN );
        sboxv1.append( s1 );

        auto shbox2 = Gtk::CenterBox{ };
        shbox2.set_hexpand( true );
        auto shbox2l = Gtk::Label{ "Map Weather" };
        shbox2.set_start_widget( shbox2l );
        shbox2.set_margin( MARGIN );

        _mapWeather = std::make_shared<dropDown>( std::vector<std::string>{
            "Regular",
            "Sunny",
            "Regular",
            "Rain",
            "Snow",
            "Thunderstorm",
            "Mist",
            "Blizzard",
            "Sandstorm",
            "Fog",
            "Dense Mist",
            "Clouds",
            "Heavy Sun",
            "Heavy Rain",
            "Underwater",
            "(Dark flash usable)",
            "(Dark permanent)",
            "(Dark flash used)",
            "Forest Clouds",
            "Ash",
            "(Dark special 1)",
            "(Dark special 2)",
        } );

        if( _mapWeather ) {
            shbox2.set_end_widget( *_mapWeather );
            _mapWeather->connect( [ this ]( u64 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.mapData( ).m_weather = p_newChoice;
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }

        sboxv1.append( shbox2 );
        mbox.append( shbox1f1 );

        auto shbox1f2 = Gtk::Frame{ };
        // shbox1f2.set_vexpand( );
        shbox1f2.set_margin_top( MARGIN );

        auto sboxh1 = Gtk::Box{ Gtk::Orientation::HORIZONTAL };
        shbox1f2.set_child( sboxh1 );
        sboxh1.set_margin( MARGIN );

        _battleBG = std::make_shared<battleBG>( "Battle BG" );
        if( _battleBG ) {
            ( (Gtk::Widget&) ( *_battleBG ) ).set_hexpand( );
            sboxh1.append( *_battleBG );
            _battleBG->connect( [ this ]( u8 p_bg, u8 p_p1, u8 p_p2 ) {
                _model.mapData( ).m_battleBG    = p_bg;
                _model.mapData( ).m_battlePlat1 = p_p1;
                _model.mapData( ).m_battlePlat2 = p_p2;
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        Gtk::Separator s2{ };
        s2.set_margin( MARGIN );
        sboxh1.append( s2 );

        _surfBG = std::make_shared<battleBG>( "Water Battle BG" );
        if( _surfBG ) {
            ( (Gtk::Widget&) ( *_surfBG ) ).set_hexpand( );
            sboxh1.append( *_surfBG );
            _surfBG->connect( [ this ]( u8 p_bg, u8 p_p1, u8 p_p2 ) {
                _model.mapData( ).m_surfBattleBG    = p_bg;
                _model.mapData( ).m_surfBattlePlat1 = p_p1;
                _model.mapData( ).m_surfBattlePlat2 = p_p2;
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }

        mbox.append( shbox1f2 );
    }

    void mapSettings::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        auto v{ _model.mapData( ).m_mapType };
        v <<= 1;
        if( !v ) { v = 1; }
        if( _mapMode ) { _mapMode->choose( v ); }
        if( _mapWeather ) { _mapWeather->choose( _model.mapData( ).m_weather ); }

        if( _battleBG ) {
            _battleBG->set( _model.m_fsdata.battleBGPath( ), _model.m_fsdata.battlePlatPath( ),
                            _model.mapData( ).m_battleBG, _model.mapData( ).m_battlePlat1,
                            _model.mapData( ).m_battlePlat2 );
        }
        if( _surfBG ) {
            _surfBG->set( _model.m_fsdata.battleBGPath( ), _model.m_fsdata.battlePlatPath( ),
                          _model.mapData( ).m_surfBattleBG, _model.mapData( ).m_surfBattlePlat1,
                          _model.mapData( ).m_surfBattlePlat2 );
        }
    }

} // namespace UI::MED
