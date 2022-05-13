#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "../../log.h"
#include "../root.h"
#include "tileSetSettings.h"

namespace UI {
    tileSetSettings::tileSetSettings( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _tsSettingsBox.set_margin( MARGIN );

        auto shbox1f1 = Gtk::Frame( "General Settings" );
        shbox1f1.set_label_align( Gtk::Align::CENTER );

        auto sboxv1 = Gtk::Box( Gtk::Orientation::VERTICAL );
        shbox1f1.set_child( sboxv1 );
        sboxv1.set_margin( MARGIN );

        auto shbox1 = Gtk::CenterBox( );
        shbox1.set_hexpand( true );
        auto shbox1l = Gtk::Label( "Tile Set Mode" );
        shbox1.set_start_widget( shbox1l );

        _tsSettingsModeToggles
            = std::make_shared<switchButton>( std::vector<std::string>{ "_Simple", "_Combined" } );

        if( _tsSettingsModeToggles ) {
            shbox1.set_end_widget( *_tsSettingsModeToggles );
            _tsSettingsModeToggles->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) != -1 ) { return; }
                _model.m_fsdata.m_tileSetMode = p_newChoice;
                _model.markTileSetsChanged( );
                _rootWindow.redraw( );
            } );
        }

        sboxv1.append( shbox1 );
        _tsSettingsBox.append( shbox1f1 );
    }

    void tileSetSettings::redraw( ) {
        if( _model.selectedBank( ) != -1 ) { return; }

        if( _tsSettingsModeToggles ) {
            _tsSettingsModeToggles->choose( _model.m_fsdata.m_tileSetMode );
        }
    }

} // namespace UI
