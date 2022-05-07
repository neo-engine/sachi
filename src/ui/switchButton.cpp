#include "switchButton.h"

namespace UI {
    switchButton::switchButton( const std::vector<std::string>& p_choices,
                                std::function<void( u8 )>       p_choiceChangedCallback,
                                u8                              p_defaultChoice ) {
        _currentSelection = p_defaultChoice;
        _buttonBox.set_margin( MARGIN );

        for( const auto& i : p_choices ) {
            _modeToggles.push_back( std::make_shared<Gtk::ToggleButton>( i, true ) );
        }

        _buttonBox.get_style_context( )->add_class( "linked" );
        _buttonBox.set_halign( Gtk::Align::CENTER );
        for( u8 i = 0; i < _modeToggles.size( ); ++i ) {
            _buttonBox.append( *_modeToggles[ i ] );
            _modeToggles[ i ]->signal_clicked( ).connect( [ &, this, i ]( ) {
                _currentSelection = i;
                p_choiceChangedCallback( i );
            } );
            if( i ) { _modeToggles[ i ]->set_group( *_modeToggles[ 0 ] ); }
        }
        _modeToggles[ _currentSelection ]->set_active( );
    }
} // namespace UI
