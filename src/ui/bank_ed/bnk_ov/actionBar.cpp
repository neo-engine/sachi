#include <gtkmm/centerbox.h>
#include <gtkmm/scrolledwindow.h>

#include "../../../defines.h"
#include "../../root.h"
#include "../../util.h"
#include "../mapEditor.h"
#include "actionBar.h"

namespace UI::BOV {
    actionBar::actionBar( model& p_model, root& p_root )
        : _model( p_model ), _rootWindow( p_root ) {

        auto abBOBox = Gtk::CenterBox( );
        _abSlBO1     = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSlBO2     = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _abSlBO3     = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abBOBox.set_start_widget( _abSlBO1 );
        abBOBox.set_center_widget( _abSlBO2 );
        abBOBox.set_end_widget( _abSlBO3 );

        _mapBankOverviewActionBar.set_child( abBOBox );

        auto abSAdjBO1 = Gtk::Adjustment::create( _model.m_settings.m_bankOverviewSpacing, 0.0, 9.0,
                                                  1.0, 1.0, 0.0 );
        auto abSAdjBO2 = Gtk::Adjustment::create( _model.m_settings.m_bankOverviewScale, 1.0, 8.0,
                                                  1.0, 1.0, 0.0 );
        auto abSAdj3   = Gtk::Adjustment::create( _model.m_settings.m_bankOverviewDayTime, 0.0, 4.0,
                                                  1.0, 1.0, 0.0 );

        _mapBankOverviewSettings1 = Gtk::SpinButton( abSAdjBO1 );
        _mapBankOverviewSettings1.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_bankOverviewSpacing = _mapBankOverviewSettings1.get_value_as_int( );
            _rootWindow.redraw( );
            _mapBankOverviewSettings1.grab_focus( );
        } );
        _mapBankOverviewSettings1.set_margin_start( MARGIN );
        _mapBankOverviewSettings1.set_width_chars( 1 );
        _mapBankOverviewSettings1.set_max_width_chars( 1 );

        _mapBankOverviewSettings2 = Gtk::SpinButton( abSAdjBO2 );
        _mapBankOverviewSettings2.set_margin_start( MARGIN );
        _mapBankOverviewSettings2.set_width_chars( 1 );
        _mapBankOverviewSettings2.set_max_width_chars( 1 );
        _mapBankOverviewSettings2.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_bankOverviewScale = _mapBankOverviewSettings2.get_value_as_int( );
            _rootWindow.redraw( );
            _mapBankOverviewSettings2.grab_focus( );
        } );

        _mapBankOverviewSettings3 = Gtk::SpinButton( abSAdj3 );
        _mapBankOverviewSettings3.set_margin_start( MARGIN );
        _mapBankOverviewSettings3.set_wrap( );
        _mapBankOverviewSettings3.set_width_chars( 1 );
        _mapBankOverviewSettings3.set_max_width_chars( 1 );
        _mapBankOverviewSettings3.signal_value_changed( ).connect( [ & ]( ) {
            _model.m_settings.m_bankOverviewDayTime = _mapBankOverviewSettings3.get_value_as_int( );
            _model.m_settings.m_overviewNeedsRedraw = true;
            _rootWindow.redraw( );
            _mapBankOverviewSettings3.grab_focus( );
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
        _abSlBO1.append( abSlBO1 );
        _abSlBO1.append( _mapBankOverviewSettings1 );
        _abSlBO2.append( abSlBO2 );
        _abSlBO2.append( _mapBankOverviewSettings2 );
        _abSlBO3.append( abSlBO3 );
        _abSlBO3.append( _mapBankOverviewSettings3 );
    }

    void actionBar::hide( ) {
        _mapBankOverviewActionBar.hide( );
    }

    void actionBar::show( ) {
        _mapBankOverviewActionBar.show( );
    }

    bool actionBar::isVisible( ) {
        return _mapBankOverviewActionBar.is_visible( );
    }

    void actionBar::redraw( ) {
    }

} // namespace UI::BOV
