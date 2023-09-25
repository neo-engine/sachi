#include <gtkmm/centerbox.h>
#include <gtkmm/image.h>
#include <gtkmm/overlay.h>
#include <gtkmm/scrolledwindow.h>
#include "battleBG.h"

namespace UI {
    battleBG::battleBG( const std::string& p_name )
        : _bg{ Gtk::Adjustment::create( 0.0, 0.0, 99.0, 1.0, 5.0, 0.0 ) },
          _p1{ Gtk::Adjustment::create( 0.0, 0.0, 99.0, 1.0, 5.0, 0.0 ) },
          _p2{ Gtk::Adjustment::create( 0.0, 0.0, 99.0, 1.0, 5.0, 0.0 ) },
          _sprite{ Gtk::Adjustment::create( 0.0, 0.0, 255.0, 1.0, 5.0, 0.0 ) }, _bgE{ _bg },
          _p1E{ _p1 }, _p2E{ _p2 }, _spriteE{ _sprite } {

        _mainBox  = Gtk::Box{ Gtk::Orientation::VERTICAL };
        auto box1 = Gtk::Box{ Gtk::Orientation::HORIZONTAL };
        auto box2 = Gtk::CenterBox{ };
        auto box3 = Gtk::CenterBox{ };
        auto box4 = Gtk::CenterBox{ };

        Gtk::Box sw{ };

        Gtk::Overlay ov{ };
        ov.set_child( _bgI );
        ov.add_overlay( _p1I );
        ov.add_overlay( _p2I );
        ov.add_overlay( _spriteI );

        _p2I.set_margin_top( 72 );
        _p2I.set_margin_start( 128 );

        _p1I.setScale( 2 );
        _p1I.set_margin_top( 98 );
        _p1I.set_margin_start( -22 );

        _spriteI.hide( );
        _spriteI.set_margin_top( 72 - 32 );
        _spriteI.set_margin_start( 128 + 32 );

        ov.set_clip_overlay( _bgI );

        sw.append( ov );
        sw.set_halign( Gtk::Align::CENTER );
        sw.set_valign( Gtk::Align::CENTER );

        _mainBox.append( box1 );
        _mainBox.append( sw );
        _mainBox.append( box2 );
        _mainBox.append( box3 );
        _mainBox.append( box4 );
        _mainBox.append( _spriteEBox );

        Gtk::Label title{ p_name };
        box1.append( title );
        box1.set_halign( Gtk::Align::CENTER );

        auto bgl = Gtk::Label{ "Background" };
        box2.set_start_widget( bgl );
        box2.set_end_widget( _bgE );
        _bgE.set_width_chars( 2 );
        _bgE.set_max_width_chars( 2 );
        _bgE.set_numeric( true );

        auto p1l = Gtk::Label{ "Player Platform" };
        box3.set_start_widget( p1l );
        box3.set_end_widget( _p1E );
        _p1E.set_width_chars( 2 );
        _p1E.set_max_width_chars( 2 );
        _p1E.set_numeric( true );

        auto p2l = Gtk::Label{ "Opponent Platform" };
        box4.set_start_widget( p2l );
        box4.set_end_widget( _p2E );
        _p2E.set_width_chars( 2 );
        _p2E.set_max_width_chars( 2 );
        _p2E.set_numeric( true );

        auto p3l = Gtk::Label{ "Opponent Sprite" };
        _spriteEBox.set_start_widget( p3l );
        _spriteEBox.set_end_widget( _spriteE );
        _spriteE.set_width_chars( 3 );
        _spriteE.set_max_width_chars( 3 );
        _spriteE.set_numeric( true );
        _spriteEBox.hide( );
    }

    void battleBG::set( const std::string& p_bgPath, const std::string& p_platPath, u8 p_bg,
                        u8 p_p1, u8 p_p2 ) {
        _lock = true;

        if( _bgIdx != p_bg ) {
            _bgE.set_value( p_bg );
            _bgI.load( p_bgPath + std::to_string( p_bg ) + ".raw" );
            _bgIdx = p_bg;
        }
        if( _plat1 != p_p1 ) {
            _p1E.set_value( p_p1 );
            _p1I.load( p_platPath + std::to_string( p_p1 ) + ".raw", 26, 0, 128 - 26, 48 );
            _plat1 = p_p1;
        }
        if( _plat2 != p_p2 ) {
            _p2E.set_value( p_p2 );
            _p2I.load( p_platPath + std::to_string( p_p2 ) + ".raw" );
            _plat2 = p_p2;
        }

        _lock = false;
    }

    void battleBG::setSprite( const std::string& p_spritePath, u8 p_sId ) {
        _lock = true;
        _spriteI.show( );
        _spriteEBox.show( );

        if( _sIdx != p_sId ) {
            _spriteE.set_value( p_sId );
            _spriteI.load2( p_spritePath + std::to_string( p_sId ) + ".raw" );
            _sIdx = p_sId;
        }

        _lock = false;
    }

    void battleBG::connect( const std::function<void( u8, u8, u8 )>& p_callback ) {
        _bg->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ) );
        } );
        _p1->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ) );
        } );
        _p2->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ) );
        } );
    }

    void battleBG::connect2( const std::function<void( u8, u8, u8, u8 )>& p_callback ) {
        _bg->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ),
                        _spriteE.get_value( ) );
        } );
        _p1->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ),
                        _spriteE.get_value( ) );
        } );
        _p2->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ),
                        _spriteE.get_value( ) );
        } );
        _sprite->signal_value_changed( ).connect( [ this, p_callback ]( ) {
            if( _lock ) { return; }
            p_callback( _bgE.get_value( ), _p1E.get_value( ), _p2E.get_value( ),
                        _spriteE.get_value( ) );
        } );
    }

} // namespace UI
