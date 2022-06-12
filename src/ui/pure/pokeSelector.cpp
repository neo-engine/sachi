#include <gtkmm/image.h>
#include "pokeSelector.h"

namespace UI {
    pokeSelector::pokeSelector( model& p_model )
        : _model{ p_model }, _pkmnIdxA{ Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 5.0, 0.0 ) },
          _pkmnFormeA{ Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 5.0, 0.0 ) },
          _pkmnIdx{ _pkmnIdxA }, _pkmnForme{ _pkmnFormeA } {

        _pkmnImage = std::make_shared<fsImage<imageType::IT_SPRITE_PKMN>>( );
        if( _pkmnImage ) { _outerBox.append( *_pkmnImage ); }

        Gtk::Box selBox{ Gtk::Orientation::VERTICAL };
        _outerBox.append( selBox );
        _outerBox.get_style_context( )->add_class( "linked" );

        Gtk::Box numBox{ Gtk::Orientation::HORIZONTAL };
        numBox.append( _pkmnIdx );
        numBox.append( _pkmnForme );
        numBox.get_style_context( )->add_class( "linked" );

        selBox.append( numBox );
        _pkmnChooser = std::make_shared<pkmnDropDown>( );
        if( _pkmnChooser ) { selBox.append( *_pkmnChooser ); }
        selBox.get_style_context( )->add_class( "linked" );
    }
} // namespace UI
