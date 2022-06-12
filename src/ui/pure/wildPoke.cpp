#include <string>
#include <vector>

#include <gtkmm/centerbox.h>
#include <gtkmm/separator.h>
#include "wildPoke.h"

namespace UI {
    wildPoke::wildPoke( model& p_model )
        : _model{ p_model }, _encRateA{ Gtk::Adjustment::create( 1.0, 100.0, 0.0, 1.0, 10.0,
                                                                 0.0 ) },
          _reqBadgesA{ Gtk::Adjustment::create( 0.0, 255.0, 0.0, 1.0, 5.0, 0.0 ) },
          _encRate{ _encRateA }, _reqBadges{ _reqBadgesA } {

        auto mainBox = Gtk::Box{ Gtk::Orientation::HORIZONTAL };

        _outerFrame.set_child( mainBox );

        auto navBox = Gtk::Box{ Gtk::Orientation::VERTICAL };
        mainBox.append( navBox );
        navBox.get_style_context( )->add_class( "linked" );

        navBox.append( _upB );
        navBox.append( _duplicateB );
        navBox.append( _removeB );
        navBox.append( _downB );

        _pkmnChooser = std::make_shared<pokeSelector>( _model );
        if( _pkmnChooser ) { mainBox.append( *_pkmnChooser ); }

        Gtk::Separator s1{ };
        mainBox.append( s1 );

        Gtk::Box encBox{ Gtk::Orientation::VERTICAL };
        encBox.set_hexpand( );

        mainBox.append( encBox );

        Gtk::CenterBox ercb{ };
        Gtk::Label     erl{ "Encounter Rate" };
        ercb.set_start_widget( erl );
        ercb.set_end_widget( _encRate );

        Gtk::CenterBox rbcb{ };
        Gtk::Label     rbl{ "Required Badges" };
        ercb.set_start_widget( rbl );
        ercb.set_end_widget( _reqBadges );

        encBox.append( ercb );
        encBox.append( rbcb );

        _encTimes = std::make_shared<multiButton>(
            std::vector<std::string>{ "Dawn", "Day", "Dusk", "Night" },
            []( u32 p_os, u8 p_nc ) { return p_os ^= ( 1 << p_nc ); }, 0,
            Gtk::Orientation::VERTICAL );
        if( _encTimes ) { mainBox.append( *_encTimes ); }
    }
} // namespace UI
