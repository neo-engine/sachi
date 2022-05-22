#include "multiButton.h"

namespace UI {
    multiButton::multiButton( const std::vector<std::string>&      p_choices,
                              const std::function<u32( u32, u8 )>& p_transition,
                              u32                                  p_defaultChoice ) {
        _transition = p_transition;
        _buttonBox.set_margin( MARGIN );

        for( const auto& i : p_choices ) {
            _toggles.push_back( std::make_shared<Gtk::ToggleButton>( i, true ) );
        }

        _buttonBox.get_style_context( )->add_class( "linked" );
        _buttonBox.set_halign( Gtk::Align::CENTER );
        for( u8 i{ 0 }; i < _toggles.size( ); ++i ) { _buttonBox.append( *_toggles[ i ] ); }

        choose( p_defaultChoice );
    }

    void multiButton::connect( const std::function<void( u32 )>& p_choiceChangedCallback ) {
        bool lock = false;
        for( u8 i{ 0 }; i < _toggles.size( ); ++i ) {
            _toggles[ i ]->signal_clicked( ).connect(
                [ this, i, &lock, p_choiceChangedCallback ]( ) {
                    if( lock ) { return; }
                    lock = true;
                    choose( _transition( _currentSelection, i ) );
                    if( p_choiceChangedCallback ) { p_choiceChangedCallback( _currentSelection ); }
                    lock = false;
                } );
        }
    }
} // namespace UI
