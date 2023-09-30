#include <gtkmm/image.h>
#include "pokeSelector.h"

namespace UI {
    pokeSelector::pokeSelector( model& p_model, bool p_enhanced )
        : _model{ p_model }, _enhanced{ p_enhanced },
          _pkmnIdxA{ Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 5.0, 0.0 ) },
          _pkmnFormeA{ Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 5.0, 0.0 ) },
          _pkmnIdx{ _pkmnIdxA }, _pkmnForme{ _pkmnFormeA } {

        _pkmnImage = std::make_shared<fsImage<imageType::IT_SPRITE_PKMN>>( );
        if( _pkmnImage ) { _outerBox.append( *_pkmnImage ); }

        Gtk::Box selBox{ Gtk::Orientation::VERTICAL };
        _outerBox.append( selBox );
        _outerBox.get_style_context( )->add_class( "linked" );
        _outerBox.set_valign( Gtk::Align::CENTER );
        selBox.set_valign( Gtk::Align::CENTER );

        Gtk::Box numBox{ Gtk::Orientation::HORIZONTAL };
        numBox.append( _pkmnIdx );
        _pkmnIdx.set_hexpand( );
        numBox.append( _pkmnForme );
        _pkmnForme.set_hexpand( );
        numBox.get_style_context( )->add_class( "linked" );
        selBox.append( numBox );

        Gtk::Box textBox{ Gtk::Orientation::HORIZONTAL };
        _pkmnChooser = std::make_shared<stringCacheDropDown>( );
        if( _pkmnChooser ) {
            ( (Gtk::Widget&) *_pkmnChooser ).set_hexpand( );
            textBox.append( *_pkmnChooser );
        }
        _gender = std::make_shared<switchButton>( std::vector<std::string>{ "(m)", "(f)", "(n)" },
                                                  0, true );
        if( _gender && _enhanced ) {
            ( (Gtk::Widget&) *_gender ).set_margin( 0 );
            textBox.append( *_gender );
        }
        selBox.append( textBox );

        selBox.get_style_context( )->add_class( "linked" );
    }

    void pokeSelector::setData( pkmnDscr p_data ) {
        _lock = true;
        if( _pkmnChooser ) { _pkmnChooser->choose( p_data.first ); }
        if( !_disableIdx ) { _pkmnIdx.set_value( p_data.first ); }
        if( !_disableFIdx ) { _pkmnForme.set_value( p_data.second & 31 ); }
        // TODO: update adjustment
        if( _pkmnFormeA ) { _pkmnFormeA->set_upper( 31 ); }
        if( _gender ) { _gender->choose( p_data.second >> 6 ); }

        if( _pkmnImage ) {
            if( !( p_data.second & 31 ) ) {
                _pkmnImage->load(
                    std::string( "@" ) + std::to_string( p_data.first ) + "@"
                    + _model.m_fsdata.pkmnSpritePath( ( p_data.second >> 6 ) & 1, _shiny ) );
            } else {
                auto path = _model.m_fsdata.pkmnFormePath( p_data.first, p_data.second & 31,
                                                           ( p_data.second >> 6 ) & 1, _shiny );
                _pkmnImage->load( path );
            }
            _image = p_data;
        }
        _lock = false;
    }
} // namespace UI
