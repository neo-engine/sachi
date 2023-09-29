#include "trainerPoke.h"

namespace UI {
    trainerPoke::trainerPoke( model& p_model )
        : _model{ p_model }, _level{ Gtk::Adjustment::create( 0.0, 0.0, 100.0, 1.0, 5.0, 0.0 ) },
          _levelE{ _level } {

        _mainBox = Gtk::Box{ Gtk::Orientation::VERTICAL };
    }

    void trainerPoke::set( const DATA::trainerPokemon& p_poke ) {
        _lock = true;
        _data = p_poke;
        _lock = false;
    }

    void trainerPoke::connect( const std::function<void( )>& p_callback ) {
        _level->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            _data.m_level = _levelE.get_value( );
            p_callback( );
        } );
    }
} // namespace UI
