#include "switchButton.h"

namespace UI {
    switchButton::switchButton( const std::vector<std::string>& p_choices, u8 p_defaultChoice,
                                bool p_compress ) {
        _currentSelection = p_defaultChoice;
        if( !p_compress ) { _buttonBox.set_margin( MARGIN ); }
        for( const auto& i : p_choices ) {
            _toggles.push_back( std::make_shared<Gtk::ToggleButton>( i, true ) );
            if( p_compress ) {
                _toggles.back( )->get_style_context( )->remove_class( "text-button" );
                _toggles.back( )->get_style_context( )->add_class( "compress" );
            }
        }

        _buttonBox.get_style_context( )->add_class( "linked" );
        _buttonBox.set_halign( Gtk::Align::CENTER );
        for( u8 i{ 0 }; i < _toggles.size( ); ++i ) {
            _buttonBox.append( *_toggles[ i ] );
            if( i ) { _toggles[ i ]->set_group( *_toggles[ 0 ] ); }
        }
        _toggles[ _currentSelection ]->set_active( );
    }

    void switchButton::connect( const std::function<void( u8 )>& p_choiceChangedCallback ) {
        for( u8 i{ 0 }; i < _toggles.size( ); ++i ) {
            _toggles[ i ]->signal_clicked( ).connect( [ this, i, p_choiceChangedCallback ]( ) {
                _currentSelection = i;
                if( p_choiceChangedCallback ) { p_choiceChangedCallback( i ); }
            } );
        }
    }
} // namespace UI
