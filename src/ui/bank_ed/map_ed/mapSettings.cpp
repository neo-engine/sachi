#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>

#include "../../../data/maprender.h"
#include "../../../log.h"
#include "../../root.h"
#include "mapSettings.h"

namespace UI::MED {
    mapSettings::mapSettings( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mainBox.set_margin( MARGIN );

        auto shbox1f1 = Gtk::Frame( );
        shbox1f1.set_label_align( Gtk::Align::CENTER );

        auto sboxv1 = Gtk::Box( Gtk::Orientation::VERTICAL );
        shbox1f1.set_child( sboxv1 );
        sboxv1.set_margin_start( MARGIN );
        sboxv1.set_margin_end( MARGIN );

        auto shbox1 = Gtk::CenterBox( );
        shbox1.set_hexpand( true );
        auto shbox1l = Gtk::Label( "Map Mode" );
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
        sboxv1.append( s1 );

        auto shbox2 = Gtk::CenterBox( );
        shbox2.set_hexpand( true );
        auto shbox2l = Gtk::Label( "Map Weather" );
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
        _mainBox.append( shbox1f1 );
    }

    void mapSettings::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        auto v{ _model.mapData( ).m_mapType };
        v <<= 1;
        if( !v ) { v = 1; }
        if( _mapMode ) { _mapMode->choose( v ); }
        if( _mapWeather ) { _mapWeather->choose( _model.mapData( ).m_weather ); }
    }

} // namespace UI::MED
