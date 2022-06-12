#include <gtkmm/centerbox.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>

#include "dropDown.h"

namespace UI {
    dropDown::dropDown( const std::vector<std::string>& p_choices, u64 p_defaultChoice )
        : _choices{ p_choices }, _currentSelection{ p_defaultChoice } {

        Gtk::CenterBox buttonBox{ };
        buttonBox.set_hexpand( );
        buttonBox.set_start_widget( _selectedText );
        _selectedText.set_ellipsize( Pango::EllipsizeMode::END );
        _selectedText.set_xalign( 0.0 );

        auto icon = Gtk::Image{ };
        icon.set_from_icon_name( "pan-down-symbolic" );
        buttonBox.set_end_widget( icon );

        _dropDown.set_child( buttonBox );

        Gtk::Box bx{ Gtk::Orientation::VERTICAL };
        bx.append( _sw );
        _sw.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _sw.set_child( _popoverBox );

        for( u64 i{ 0 }; i < _choices.size( ); ++i ) {
            Gtk::Label lb{ _choices[ i ] };
            Gtk::Box   bb{ Gtk::Orientation::HORIZONTAL };
            bb.append( lb );

            auto ic = Gtk::Image{ };
            ic.set_from_icon_name( "object-select-symbolic" );
            bb.append( ic );
            _popoverBtnSelect.push_back( std::move( ic ) );

            auto btn = std::make_shared<Gtk::Button>( );
            if( !btn ) { break; }

            btn->set_child( bb );
            _popoverBox.append( *btn );
            _popoverBox.set_hexpand( );
            btn->get_style_context( )->add_class( "flat" );
            btn->set_halign( Gtk::Align::FILL );
            btn->set_hexpand( true );

            _popoverButtons.push_back( btn );
        }

        bx.set_halign( Gtk::Align::FILL );
        bx.set_valign( Gtk::Align::FILL );
        _popover.set_child( bx );
        _popover.set_valign( Gtk::Align::FILL );
        _popover.set_has_arrow( false );
        _popover.set_parent( _dropDown );

        _popover.signal_closed( ).connect( [ this ]( ) {
            _dropDown.set_active( false );
            _dropDown.grab_focus( );
        } );

        _dropDown.signal_clicked( ).connect(
            [ this ]( ) {
                _popoverButtons[ _currentSelection ]->grab_focus( );
                _popover.popup( );
            },
            false );
        _sw.set_size_request( 70, 400 );

        choose( p_defaultChoice );
    }

    void dropDown::connect( const std::function<void( u64 )>& p_choiceChangedCallback ) {
        for( u64 i{ 0 }; i < _popoverButtons.size( ); ++i ) {
            _popoverButtons[ i ]->signal_clicked( ).connect(
                [ this, i, p_choiceChangedCallback ]( ) {
                    choose( i );
                    _popover.popdown( );
                    if( p_choiceChangedCallback ) { p_choiceChangedCallback( i ); }
                } );
        }
    }

    void pkmnDropDown::refreshModel( model& p_model ) {
        auto sc = p_model.pkmnNames( );
        if( sc.m_lastRefresh <= _lastRefresh ) { return; }
        _lastRefresh = sc.m_lastRefresh;
        _choices     = sc.m_strings;
        _popoverBtnSelect.clear( );

        for( auto btn : _popoverButtons ) {
            if( btn ) { _popoverBox.remove( *btn ); }
        }
        _popoverButtons.clear( );

        if( _currentSelection >= _choices.size( ) ) { _currentSelection = _choices.size( ) - 1; }

        for( u64 i{ 0 }; i < _choices.size( ); ++i ) {
            Gtk::Label lb{ _choices[ i ] };
            Gtk::Box   bb{ Gtk::Orientation::HORIZONTAL };
            bb.append( lb );

            auto ic = Gtk::Image{ };
            ic.set_from_icon_name( "object-select-symbolic" );
            bb.append( ic );
            _popoverBtnSelect.push_back( std::move( ic ) );

            auto btn = std::make_shared<Gtk::Button>( );
            if( !btn ) { break; }

            btn->set_child( bb );
            _popoverBox.append( *btn );
            _popoverBox.set_hexpand( );
            btn->get_style_context( )->add_class( "flat" );
            btn->set_halign( Gtk::Align::FILL );
            btn->set_hexpand( true );

            _popoverButtons.push_back( btn );
        }
    }
} // namespace UI
