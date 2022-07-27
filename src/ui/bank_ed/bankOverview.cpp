#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "../../log.h"
#include "../root.h"
#include "bankOverview.h"
#include "bnk_ov/mapBankOverview.h"

namespace UI {
    bankOverview::bankOverview( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mapOverviewBox.set_margin( MARGIN );

        /*
        _bankOverviewModeToggles
            = std::make_shared<switchButton>( std::vector<std::string>{ "_Blocks", "Loca_tions" } );

        if( _bankOverviewModeToggles ) {
            _mapOverviewBox.append( *_bankOverviewModeToggles );
            _bankOverviewModeToggles->connect( [ this ]( u8 p_newChoice ) {
                setNewMapOverviewMode( bankOverviewMode( p_newChoice ) );
            } );
        }
        */

        auto boswbox = Gtk::Box( );
        boswbox.set_halign( Gtk::Align::CENTER );
        boswbox.set_valign( Gtk::Align::CENTER );
        boswbox.set_expand( true );

        auto bankOverviewSW = Gtk::ScrolledWindow( );
        bankOverviewSW.set_margin( MARGIN );
        bankOverviewSW.set_policy( Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC );
        bankOverviewSW.set_child( boswbox );
        boswbox.append( _mapBankOverview );

        _mapBankOverview.connectClick(
            [ this ]( BOV::mapBankOverview::clickType p_ct, u16 p_mapX, u16 p_mapY ) {
                switch( p_ct ) {
                case BOV::mapBankOverview::LEFT_DOUBLE: {
                    // load map
                    _rootWindow.onUnloadMap( _model.selectedBank( ), _model.selectedMapY( ),
                                             _model.selectedMapX( ) );
                    _rootWindow.loadMap( _model.selectedBank( ), p_mapY, p_mapX );
                    break;
                }
                case BOV::mapBankOverview::MIDDLE_DOUBLE: {
                    // copy selected map to position
                    _model.copySelectedMapTo( p_mapY, p_mapX );
                    _mapBankOverview.replaceMap( _model.computedSlice( ), p_mapY, p_mapX );
                    break;
                }
                case BOV::mapBankOverview::RIGHT_DOUBLE: {
                    // copy selected map' wild pkmn and meta data to position
                    _model.copySelectedMapMetaTo( p_mapY, p_mapX );
                    break;
                }
                }
            } );

        _mapOverviewBox.append( bankOverviewSW );

        _actionBar = std::make_shared<BOV::actionBar>( p_model, p_root );
        if( _actionBar ) { _mapOverviewBox.append( *_actionBar ); }
    }

    void bankOverview::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        _mapBankOverview.setScale( _model.m_settings.m_bankOverviewScale );
        _mapBankOverview.setSpacing( _model.m_settings.m_bankOverviewSpacing );
        _mapBankOverview.queue_resize( );

        if( _model.m_settings.m_overviewNeedsRedraw ) { render( ); }

        _mapBankOverview.selectMap( _model.selectedMapX( ), _model.selectedMapY( ) );
        _mapBankOverview.grab_focus( );
    }

    void bankOverview::render( ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        auto cb = _model.bank( );

        if( !cb.m_loaded ) { return; }

        _mapBankOverview.set( cb.m_computedBank );

        _mapBankOverview.redraw( _model.m_settings.m_bankOverviewDayTime );
        _model.m_settings.m_overviewNeedsRedraw = false;
    }

    void bankOverview::setNewMapOverviewMode( bankOverviewMode p_newMode ) {
        _mapOverviewMode = p_newMode;
        redraw( );
    }
} // namespace UI
