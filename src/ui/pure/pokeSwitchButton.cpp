#include "pokeSwitchButton.h"

namespace UI {
    pokeSwitchButton::pokeSwitchButton( model& p_model, const std::vector<pkmnDscr>& p_choices,
                                        u8 p_defaultChoice )
        : _model{ p_model } {
        _currentSelection = p_defaultChoice;

        for( const auto& i : p_choices ) {
            _toggles.push_back( std::make_shared<Gtk::ToggleButton>( ) );

            auto image = std::make_shared<fsImage<imageType::IT_SPRITE_PKMN>>( );
            _pkmnImages.push_back( image );
            if( image ) {
                image->setScale( 0.4 );
                _toggles.back( )->set_child( *image );
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

    void pokeSwitchButton::updateChoices( const std::vector<pkmnDscr>& p_choices ) {
        for( u64 i = 0; i < p_choices.size( ); ++i ) {
            if( _pkmnImages[ i ] ) {
                if( !p_choices[ i ].second ) {
                    _pkmnImages[ i ]->load( std::string( "@" )
                                            + std::to_string( p_choices[ i ].first ) + "@"
                                            + _model.m_fsdata.pkmnSpritePath( ) );
                } else {
                    _pkmnImages[ i ]->load( _model.m_fsdata.pkmnFormePath(
                        p_choices[ i ].first, p_choices[ i ].second ) );
                }
            }
        }
    }

    void pokeSwitchButton::connect( const std::function<void( u8 )>& p_choiceChangedCallback ) {
        for( u8 i{ 0 }; i < _toggles.size( ); ++i ) {
            _toggles[ i ]->signal_clicked( ).connect( [ this, i, p_choiceChangedCallback ]( ) {
                _currentSelection = i;
                if( p_choiceChangedCallback ) { p_choiceChangedCallback( i ); }
            } );
        }
    }
} // namespace UI
