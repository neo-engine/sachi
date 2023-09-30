#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>

#include "trainerPoke.h"

namespace UI {
    trainerPoke::trainerPoke( model& p_model )
        : _model{ p_model }, _level{ Gtk::Adjustment::create( 0.0, 0.0, 100.0, 1.0, 5.0, 0.0 ) },
          _levelE{ _level } {

        _mainBox = Gtk::Box{ Gtk::Orientation::VERTICAL };

        auto box1 = Gtk::Box{ Gtk::Orientation::HORIZONTAL };

        _pkmn = std::make_shared<pokeSelector>( _model, true );
        if( _pkmn ) { box1.append( *_pkmn ); }

        _item = std::make_shared<itemSelector>( _model );
        if( _item ) { box1.append( *_item ); }
        _mainBox.append( box1 );

        auto       box2  = Gtk::Box{ Gtk::Orientation::HORIZONTAL };
        auto       box2l = Gtk::Box{ Gtk::Orientation::VERTICAL };
        Gtk::Frame lframe{ };
        box2.append( lframe );
        box2l.set_margin( MARGIN );
        lframe.set_hexpand( );
        lframe.set_child( box2l );

        // Level
        Gtk::CenterBox lcbox{ };
        Gtk::Label     ll{ "Level" };
        lcbox.set_start_widget( ll );
        lcbox.set_end_widget( _levelE );
        box2l.append( lcbox );

        // Ability
        Gtk::CenterBox acbox{ };
        acbox.set_margin_top( MARGIN );
        Gtk::Label al{ "Ability" };
        al.set_margin_end( MARGIN );
        acbox.set_start_widget( al );

        _ability = std::make_shared<numberedStringCacheDropDown>( );
        if( _ability ) {
            acbox.set_end_widget( *_ability );
            ( (Gtk::Widget&) *_ability ).set_hexpand( false );
        }
        box2l.append( acbox );

        // Shininess
        Gtk::CenterBox scbox{ };
        scbox.set_margin_top( MARGIN );
        Gtk::Label sl{ "Shininess" };
        sl.set_margin_end( MARGIN );
        scbox.set_start_widget( sl );

        _shiny = std::make_shared<switchButton>( std::vector<std::string>{ "N/A", "S", "C", "F" } );
        if( _shiny ) {
            scbox.set_end_widget( *_shiny );
            ( (Gtk::Widget&) *_shiny ).set_margin( 0 );
        }
        box2l.append( scbox );

        auto       box2r = Gtk::Box{ Gtk::Orientation::VERTICAL };
        Gtk::Frame rframe{ };
        box2.append( rframe );
        rframe.set_margin_start( MARGIN );
        rframe.set_hexpand( );
        box2r.set_margin( MARGIN );
        rframe.set_child( box2r );

        // Moves (in a 2x2 grid)
        Gtk::Grid movegrid{ };
        movegrid.set_column_homogeneous( );
        movegrid.set_expand( );
        movegrid.get_style_context( )->add_class( "linked" );

        for( u8 y = 0; y < 2; ++y ) {
            for( u8 x = 0; x < 2; ++x ) {
                auto mv = std::make_shared<numberedStringCacheDropDown>( );
                if( mv ) {
                    movegrid.attach( *mv, x, y );
                    _moves.push_back( mv );
                    ( (Gtk::Widget&) *mv ).set_hexpand( false );
                }
            }
        }
        box2r.append( movegrid );

        _mainBox.append( box2 );
    }

    void trainerPoke::set( const DATA::trainerPokemon& p_poke ) {
        _lock = true;
        _data = p_poke;
        if( _pkmn ) {
            _pkmn->refreshModel( );
            _pkmn->setShiny( _data.m_shiny );
            _pkmn->setData( { _data.m_speciesId, _data.m_forme } );
        }
        if( _item ) {
            _item->refreshModel( );
            _item->setData( _data.m_heldItem );
        }
        _levelE.set_value( _data.m_level );
        if( _ability ) {
            _ability->refreshModel( _model.abilityNames( ) );
            _ability->choose( _data.m_ability );
        }

        for( u8 y = 0; y < 2; ++y ) {
            for( u8 x = 0; x < 2; ++x ) {
                auto i = 2 * y + x;
                if( i < _moves.size( ) && _moves[ i ] ) {
                    _moves[ i ]->refreshModel( _model.moveNames( ) );
                    _moves[ i ]->choose( _data.m_moves[ i ] );
                }
            }
        }

        if( _shiny ) { _shiny->choose( _data.m_shiny ); }
        _lock = false;
    }

    void trainerPoke::connect( const std::function<void( )>& p_callback ) {
        _level->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            _data.m_level = _levelE.get_value( );
            p_callback( );
        } );

        if( _pkmn ) {
            _pkmn->connect( [ this, p_callback ]( std::pair<u16, u8> p_pkmn ) {
                if( _lock ) { return; }
                _data.m_speciesId = p_pkmn.first;
                _data.m_forme     = p_pkmn.second;
                p_callback( );
            } );
        }
        if( _item ) {
            _item->connect( [ this, p_callback ]( u16 p_item ) {
                if( _lock ) { return; }
                _data.m_heldItem = p_item;
                p_callback( );
            } );
        }
        if( _ability ) {
            _ability->connect( [ this, p_callback ]( u64 p_newChoice ) {
                if( _lock ) { return; }
                _data.m_ability = p_newChoice;
                p_callback( );
            } );
        }

        for( u8 y = 0; y < 2; ++y ) {
            for( u8 x = 0; x < 2; ++x ) {
                auto i = 2 * y + x;
                if( i < _moves.size( ) && _moves[ i ] ) {
                    _moves[ i ]->connect( [ this, i, p_callback ]( u64 p_newChoice ) {
                        if( _lock ) { return; }
                        _data.m_moves[ i ] = p_newChoice;
                        p_callback( );
                    } );
                }
            }
        }

        if( _shiny ) {
            _shiny->connect( [ this, p_callback ]( u8 p_newSelection ) {
                if( _lock ) { return; }
                _data.m_shiny = p_newSelection;
                if( _pkmn ) { _pkmn->setShiny( _data.m_shiny ); }
                p_callback( );
            } );
        }
        _level->signal_value_changed( ).connect( [ this, p_callback ] {
            if( _lock ) { return; }
            _data.m_level = _levelE.get_value( );
            p_callback( );
        } );
    }
} // namespace UI
