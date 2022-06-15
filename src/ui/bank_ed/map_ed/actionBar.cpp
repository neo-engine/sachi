#include <gtkmm/centerbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>

#include "../../../defines.h"
#include "../../pure/util.h"
#include "../../root.h"
#include "../mapEditor.h"
#include "actionBar.h"

namespace UI::MED {
    actionBar::actionBar( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {
        // map editor action box
        // +--------------------+-----+----------------------------+
        // | Map Grid [ 0 |+|-] | Map | Blockset width   [ 8 |+|-] |
        // | Scale    [ 1 |+|-] | Nav | Neigh map blocks [ 8 |+|-] |
        // | Day Time [Day (2)] |     | Neigh distance   [ 9 |+|-] |
        // +--------------------+-----+----------------------------+
        auto abScrolledWindow = Gtk::ScrolledWindow{ };
        _mapEditorActionBar.set_child( abScrolledWindow );
        _mapEditorActionBar.set_margin_top( MARGIN );
        abScrolledWindow.set_policy( Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER );
        auto mapEditorActions = Gtk::CenterBox( );

        abScrolledWindow.set_child( mapEditorActions );

        // action bar start
        auto abStartBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abStartBox.set_valign( Gtk::Align::CENTER );
        mapEditorActions.set_start_widget( abStartBox );
        _abSb1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSb2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSb3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abStartBox.append( _abSb1 );
        abStartBox.append( _abSb2 );
        abStartBox.append( _abSb3 );

        auto abSAdj1
            = Gtk::Adjustment::create( _model.m_settings.m_blockSpacing, 0.0, 9.0, 1.0, 1.0, 0.0 );
        auto abSAdj2
            = Gtk::Adjustment::create( _model.m_settings.m_blockScale, 1.0, 8.0, 1.0, 1.0, 0.0 );
        auto abSAdj3 = Gtk::Adjustment::create( _model.m_settings.m_currentDayTime, 0.0, 4.0, 1.0,
                                                1.0, 0.0 );

        _mapEditorSettings1 = Gtk::SpinButton( abSAdj1 );
        _mapEditorSettings1.signal_value_changed( ).connect( [ & ]( ) {
            auto value                       = _mapEditorSettings1.get_value_as_int( );
            _model.m_settings.m_blockSpacing = value;
            _rootWindow.redraw( );
            _mapEditorSettings1.grab_focus( );
        } );
        _mapEditorSettings1.set_margin_start( MARGIN );
        _mapEditorSettings1.set_width_chars( 1 );
        _mapEditorSettings1.set_max_width_chars( 1 );

        _mapEditorSettings2 = Gtk::SpinButton( abSAdj2 );
        _mapEditorSettings2.set_margin_start( MARGIN );
        _mapEditorSettings2.set_width_chars( 1 );
        _mapEditorSettings2.set_max_width_chars( 1 );
        _mapEditorSettings2.signal_value_changed( ).connect( [ & ]( ) {
            auto value                     = _mapEditorSettings2.get_value_as_int( );
            _model.m_settings.m_blockScale = value;
            _rootWindow.redraw( );
            _mapEditorSettings2.grab_focus( );
        } );

        _mapEditorSettings3 = Gtk::SpinButton( abSAdj3 );
        _mapEditorSettings3.set_margin_start( MARGIN );
        _mapEditorSettings3.set_wrap( );
        _mapEditorSettings3.set_width_chars( 1 );
        _mapEditorSettings3.set_max_width_chars( 1 );
        _mapEditorSettings3.signal_value_changed( ).connect( [ & ]( ) {
            auto value                         = _mapEditorSettings3.get_value_as_int( );
            _model.m_settings.m_currentDayTime = value;
            _rootWindow.redraw( );
            _mapEditorSettings3.grab_focus( );
        } );

        auto abSl1 = Gtk::Image( );
        abSl1.set_from_icon_name( "view-grid-symbolic" );
        abSl1.set_margin_start( MARGIN );
        auto abSl2 = Gtk::Image( );
        abSl2.set_from_icon_name( "edit-find-symbolic" );
        abSl2.set_margin_start( MARGIN );
        auto abSl3 = Gtk::Image( );
        abSl3.set_from_icon_name( "weather-clear-symbolic" );
        abSl3.set_margin_start( MARGIN );
        _abSb1.append( abSl1 );
        _abSb1.append( _mapEditorSettings1 );
        _abSb2.append( abSl2 );
        _abSb2.append( _mapEditorSettings2 );
        _abSb3.append( abSl3 );
        _abSb3.append( _mapEditorSettings3 );

        // action bar center
        auto mapNavGrid = Gtk::Grid( );
        for( u8 x = 0; x < 3; ++x ) {
            for( u8 y = 0; y < 3; ++y ) {
                if( x == 1 && y == 1 ) {
                    auto im = Gtk::Image( );
                    im.set_from_icon_name( "image-x-generic-symbolic" );
                    mapNavGrid.attach( im, x, y );
                    continue;
                }
                _mapNavButton[ x ][ y ] = createButton( "", "", [ this, y, x ]( ) {
                    _rootWindow.moveToMap( s8( y ) - 1, s8( x ) - 1 );
                } );
                _mapNavButton[ x ][ y ]->set_has_frame( false );
                if( x == 1 && y == 2 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-down-symbolic" );
                } else if( x == 1 && y == 0 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-up-symbolic" );
                } else if( x == 0 && y == 1 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-start-symbolic" );
                } else if( x == 2 && y == 1 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-end-symbolic" );
                }
                mapNavGrid.attach( *_mapNavButton[ x ][ y ], x, y );
            }
        }
        mapEditorActions.set_center_widget( mapNavGrid );

        // action bar end

        auto abEndBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abEndBox.set_valign( Gtk::Align::CENTER );
        mapEditorActions.set_end_widget( abEndBox );
        _abEb1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abEb2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abEb3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abEndBox.append( _abEb1 );
        abEndBox.append( _abEb2 );
        abEndBox.append( _abEb3 );

        auto abEAdj1 = Gtk::Adjustment::create( _model.m_settings.m_blockSetWidth, 1.0, 32.0, 1.0,
                                                1.0, 0.0 );
        auto abEAdj2 = Gtk::Adjustment::create( _model.m_settings.m_adjacentBlocks, 0.0, 32.0, 1.0,
                                                1.0, 0.0 );
        auto abEAdj3 = Gtk::Adjustment::create( _model.m_settings.m_neighborSpacing, 0.0, 50.0, 1.0,
                                                1.0, 0.0 );

        _mapEditorSettings4 = Gtk::SpinButton( abEAdj1 );
        _mapEditorSettings4.set_margin_start( MARGIN );
        _mapEditorSettings4.set_width_chars( 1 );
        _mapEditorSettings4.set_max_width_chars( 1 );
        _mapEditorSettings4.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_blockSetWidth = _mapEditorSettings4.get_value_as_int( );
            _rootWindow.redraw( );
            _mapEditorSettings4.grab_focus( );
        } );

        _mapEditorSettings5 = Gtk::SpinButton( abEAdj2 );
        _mapEditorSettings5.set_margin_start( MARGIN );
        _mapEditorSettings5.set_width_chars( 1 );
        _mapEditorSettings5.set_max_width_chars( 1 );
        _mapEditorSettings5.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_adjacentBlocks = _mapEditorSettings5.get_value_as_int( );
            printf( "val changed %d\n", _model.m_settings.m_adjacentBlocks );
            _rootWindow.redraw( );
            _mapEditorSettings5.grab_focus( );
        } );

        _mapEditorSettings6 = Gtk::SpinButton( abEAdj3 );
        _mapEditorSettings6.set_margin_start( MARGIN );
        _mapEditorSettings6.set_width_chars( 1 );
        _mapEditorSettings6.set_max_width_chars( 1 );

        _mapEditorSettings6.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_neighborSpacing = _mapEditorSettings6.get_value_as_int( );
            _rootWindow.redraw( );
            _mapEditorSettings6.grab_focus( );
        } );

        auto abEl1 = Gtk::Image( );
        abEl1.set_from_icon_name( "window-restore-symbolic" );
        auto abEl2 = Gtk::Image( );
        abEl2.set_from_icon_name( "process-stop-symbolic" );
        auto abEl3 = Gtk::Image( );
        abEl3.set_from_icon_name( "application-x-appliance-symbolic" );
        _abEb1.append( abEl1 );
        _abEb1.append( _mapEditorSettings4 );
        _abEb2.append( abEl2 );
        _abEb2.append( _mapEditorSettings5 );
        _abEb3.append( abEl3 );
        _abEb3.append( _mapEditorSettings6 );
    }

    void actionBar::hide( ) {
        _mapEditorActionBar.hide( );
    }

    void actionBar::show( ) {
        _mapEditorActionBar.show( );
    }

    bool actionBar::isVisible( ) {
        return _mapEditorActionBar.is_visible( );
    }

    void actionBar::redraw( ) {
        switch( _currentMode ) {
        case mapEditor::MODE_EDIT_TILES:
            if( !_abEb1.is_visible( ) ) { _abEb1.show( ); }
            if( !_abEb2.is_visible( ) ) { _abEb2.show( ); }
            if( !_abEb3.is_visible( ) ) { _abEb3.show( ); }
            if( !_abSb1.is_visible( ) ) { _abSb1.show( ); }
            if( !_abSb2.is_visible( ) ) { _abSb2.show( ); }
            if( !_abSb3.is_visible( ) ) { _abSb3.show( ); }
            break;
        case mapEditor::MODE_EDIT_MOVEMENT:
        case mapEditor::MODE_EDIT_LOCATIONS:
        case mapEditor::MODE_EDIT_EVENTS:
            if( _abEb1.is_visible( ) ) { _abEb1.hide( ); }
            if( !_abEb2.is_visible( ) ) { _abEb2.show( ); }
            if( !_abEb3.is_visible( ) ) { _abEb3.show( ); }
            if( !_abSb1.is_visible( ) ) { _abSb1.show( ); }
            if( !_abSb2.is_visible( ) ) { _abSb2.show( ); }
            if( !_abSb3.is_visible( ) ) { _abSb3.show( ); }
            break;
        default:
            if( _abEb1.is_visible( ) ) { _abEb1.hide( ); }
            if( _abEb2.is_visible( ) ) { _abEb2.hide( ); }
            if( _abEb3.is_visible( ) ) { _abEb3.hide( ); }
            if( _abSb1.is_visible( ) ) { _abSb1.hide( ); }
            if( _abSb2.is_visible( ) ) { _abSb2.hide( ); }
            if( _abSb3.is_visible( ) ) { _abSb3.hide( ); }
            break;
        }
    }

    void actionBar::setNewMapEditMode( mapEditor::mapDisplayMode p_newMode ) {
        _currentMode = p_newMode;
        redraw( );
    }

} // namespace UI::MED
