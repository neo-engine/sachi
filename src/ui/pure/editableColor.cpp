#include "editableColor.h"

namespace UI {
    editableColor::editableColor( std::string p_label ) : _label{ p_label } {
        Gtk::Box mbox{ Gtk::Orientation::VERTICAL };
        mbox.set_margin( MARGIN );
        _mainFrame.set_child( mbox );
        _mainFrame.set_label( _label );
        _mainFrame.set_label_align( Gtk::Align::CENTER );

        mbox.append( _colorButton );
        mbox.append( _entry );
        mbox.get_style_context( )->add_class( "linked" );

        _entry.set_alignment( Gtk::Align::CENTER );
        _entry.set_max_length( 4 );
        _entry.set_placeholder_text( "0000" );
        _entry.set_editable( false );

        _colorButton.set_use_alpha( false );
    }

    void editableColor::connect( const std::function<void( u16 )>& p_choiceChangedCallback ) {
        _colorButton.signal_color_set( ).connect( [ this, p_choiceChangedCallback ]( ) {
            if( _lock ) { return; }
            _lock = true;

            auto c{ _colorButton.get_rgba( ) };

            auto r{ std::min( 31, 1 + c.get_red_u( ) * 31 / 65535 ) },
                g{ std::min( 31, 1 + c.get_green_u( ) * 31 / 65535 ) },
                b{ std::min( 31, 1 + c.get_blue_u( ) * 31 / 65535 ) };

            set( r, g, b );
            p_choiceChangedCallback( get( ) );

            _lock = false;
        } );

        /*
        _entry.signal_changed( ).connect( [ this, p_choiceChangedCallback ]( ) {
            if( _lock ) { return; }
            _lock = true;

            try {
                auto c{ std::stoi( _entry.get_text( ), nullptr, 16 ) };
                fprintf( stderr, "%i\n", c );
                set( c );
            } catch( ... ) { return; }
            p_choiceChangedCallback( get( ) );

            _lock = false;
        } );
        */
    }

    void editableColor::set( u16 p_color ) {
        _color = p_color;

        int       r{ red( p_color ) }, g{ green( p_color ) }, b{ blue( p_color ) };
        Gdk::RGBA c{ };

        c.set_red( r / 255.0 );
        c.set_green( g / 255.0 );
        c.set_blue( b / 255.0 );
        c.set_alpha( 1.0 );

        _colorButton.set_rgba( c );

        char buffer[ 10 ];
        snprintf( buffer, 5, "%04X", p_color );
        _entry.set_text( buffer );
    }
} // namespace UI
