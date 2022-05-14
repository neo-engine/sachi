#include <gtkmm/centerbox.h>
#include <gtkmm/scrolledwindow.h>

#include "../../../defines.h"
#include "../../pure/util.h"
#include "../../root.h"
#include "actionBar.h"

namespace UI::TED {
    actionBar::actionBar( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {
        auto abBOBox = Gtk::CenterBox( );

        _abSlBO1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSlBO2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSlBO3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSlBO4 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSlBO5 = Gtk::Box( Gtk::Orientation::HORIZONTAL );

        auto abStartBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abStartBox.set_valign( Gtk::Align::CENTER );
        abStartBox.append( _abSlBO1 );
        abStartBox.append( _abSlBO4 );

        auto abCenterBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abCenterBox.set_valign( Gtk::Align::CENTER );
        abCenterBox.append( _abSlBO2 );
        abCenterBox.append( _abSlBO5 );

        auto abEndBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abEndBox.set_valign( Gtk::Align::CENTER );
        abEndBox.append( _abSlBO3 );

        abBOBox.set_start_widget( abStartBox );
        abBOBox.set_center_widget( abCenterBox );
        abBOBox.set_end_widget( abEndBox );

        _actionBar.set_child( abBOBox );
        _actionBar.set_margin_top( MARGIN );

        auto abSAdjBO1
            = Gtk::Adjustment::create( _model.m_settings.m_tseSpacing, 0.0, 9.0, 1.0, 1.0, 0.0 );
        auto abSAdjBO2
            = Gtk::Adjustment::create( _model.m_settings.m_tseScale, 1.0, 8.0, 1.0, 1.0, 0.0 );
        auto abSAdj3
            = Gtk::Adjustment::create( _model.m_settings.m_tseDayTime, 0.0, 4.0, 1.0, 1.0, 0.0 );
        auto abSAdj4 = Gtk::Adjustment::create( _model.m_settings.m_tseBlockSetWidth, 1.0, 32.0,
                                                1.0, 1.0, 0.0 );
        auto abSAdj5 = Gtk::Adjustment::create( _model.m_settings.m_tseTileSetWidth, 1.0, 32.0, 1.0,
                                                1.0, 0.0 );

        _setting1 = Gtk::SpinButton( abSAdjBO1 );
        _setting1.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_tseSpacing = _setting1.get_value_as_int( );
            _rootWindow.redraw( );
            _setting1.grab_focus( );
        } );
        _setting1.set_margin_start( MARGIN );
        _setting1.set_width_chars( 1 );
        _setting1.set_max_width_chars( 1 );

        _setting2 = Gtk::SpinButton( abSAdjBO2 );
        _setting2.set_margin_start( MARGIN );
        _setting2.set_width_chars( 1 );
        _setting2.set_max_width_chars( 1 );
        _setting2.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_tseScale = _setting2.get_value_as_int( );
            _rootWindow.redraw( );
            _setting2.grab_focus( );
        } );

        _setting3 = Gtk::SpinButton( abSAdj3 );
        _setting3.set_margin_start( MARGIN );
        _setting3.set_wrap( );
        _setting3.set_width_chars( 1 );
        _setting3.set_max_width_chars( 1 );
        _setting3.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_tseDayTime = _setting3.get_value_as_int( );
            _rootWindow.redraw( );
            _setting3.grab_focus( );
        } );

        _setting4 = Gtk::SpinButton( abSAdj4 );
        _setting4.set_margin_start( MARGIN );
        _setting4.set_width_chars( 1 );
        _setting4.set_max_width_chars( 1 );
        _setting4.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_tseBlockSetWidth = _setting4.get_value_as_int( );
            _rootWindow.redraw( );
            _setting4.grab_focus( );
        } );

        _setting5 = Gtk::SpinButton( abSAdj5 );
        _setting5.set_margin_start( MARGIN );
        _setting5.set_width_chars( 1 );
        _setting5.set_max_width_chars( 1 );
        _setting5.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_tseTileSetWidth = _setting5.get_value_as_int( );
            _rootWindow.redraw( );
            _setting5.grab_focus( );
        } );

        auto abSlBO1 = Gtk::Image( );
        abSlBO1.set_from_icon_name( "view-grid-symbolic" );
        abSlBO1.set_margin_start( MARGIN );
        auto abSlBO2 = Gtk::Image( );
        abSlBO2.set_from_icon_name( "edit-find-symbolic" );
        abSlBO2.set_margin_start( MARGIN );
        auto abSlBO3 = Gtk::Image( );
        abSlBO3.set_from_icon_name( "weather-clear-symbolic" );
        abSlBO3.set_margin_start( MARGIN );
        auto abSlBO4 = Gtk::Image( );
        abSlBO4.set_from_icon_name( "window-restore-symbolic" );
        abSlBO4.set_margin_start( MARGIN );
        auto abSlBO5 = Gtk::Image( );
        abSlBO5.set_from_icon_name( "zoom-fit-best-symbolic" );
        abSlBO5.set_margin_start( MARGIN );
        _abSlBO1.append( abSlBO1 );
        _abSlBO1.append( _setting1 );
        _abSlBO2.append( abSlBO2 );
        _abSlBO2.append( _setting2 );
        _abSlBO3.append( abSlBO3 );
        _abSlBO3.append( _setting3 );
        _abSlBO4.append( abSlBO4 );
        _abSlBO4.append( _setting4 );
        _abSlBO5.append( abSlBO5 );
        _abSlBO5.append( _setting5 );
    }

    void actionBar::hide( ) {
        _actionBar.hide( );
    }

    void actionBar::show( ) {
        _actionBar.show( );
    }

    bool actionBar::isVisible( ) {
        return _actionBar.is_visible( );
    }

    void actionBar::redraw( ) {
        switch( _currentMode ) {
        case blockEditor::TSEMODE_EDIT_PALETTES:
            if( _abSlBO4.is_visible( ) ) { _abSlBO4.hide( ); }
            if( _abSlBO5.is_visible( ) ) { _abSlBO5.hide( ); }
            break;
        case blockEditor::TSEMODE_EDIT_TILES:
            if( _abSlBO4.is_visible( ) ) { _abSlBO4.hide( ); }
            if( !_abSlBO5.is_visible( ) ) { _abSlBO5.show( ); }
            break;
        default:
            if( !_abSlBO4.is_visible( ) ) { _abSlBO4.show( ); }
            if( !_abSlBO5.is_visible( ) ) { _abSlBO5.show( ); }
            break;
        }
    }

    void actionBar::setNewEditMode( blockEditor::tseDisplayMode p_newMode ) {
        _currentMode = p_newMode;
        redraw( );
    }

} // namespace UI::TED
