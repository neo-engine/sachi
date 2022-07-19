#include "movementSelector.h"
#include "../../root.h"

namespace UI::MED {
    movementSelector::movementSelector( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {
        _movementFrame = Gtk::Frame( "Movements" );
        _movementFrame.set_margin_start( MARGIN );
        _movementFrame.set_label_align( Gtk::Align::CENTER );

        auto meScrolledWindow = Gtk::ScrolledWindow( );
        meScrolledWindow.set_margin( MARGIN );
        meScrolledWindow.set_child( _movementWidget );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _movementWidget.setOverlayOpacity( .9 );
        _movementWidget.set( DATA::mapBlockAtom::computeMovementSet( ), pals, 1 );
        _movementWidget.setOverlayHidden( false );
        _movementWidget.draw( );
        _movementWidget.setScale( 2 );
        _movementWidget.queue_resize( );
        _movementWidget.connectClick( [ this ]( mapSlice::clickType, u16, u16 p_blockY ) {
            _model.updateSelectedBlock( { 0, DATA::mapBlockAtom::MOVEMENT_ORDER[ p_blockY ] } );
            _rootWindow.redraw( );
        } );

        meScrolledWindow.set_margin( MARGIN );
        meScrolledWindow.set_vexpand( );
        meScrolledWindow.set_halign( Gtk::Align::CENTER );
        meScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _movementFrame.set_child( meScrolledWindow );
    }

    void movementSelector::updateSelection( ) {

        for( u8 i = 0; i < DATA::MAX_MOVEMENTS; ++i ) {
            if( DATA::mapBlockAtom::MOVEMENT_ORDER[ i ]
                == _model.m_settings.m_currentlySelectedBlock.m_movedata ) {
                _movementWidget.selectBlock( i );
                break;
            }
        }
    }

    void movementSelector::redraw( ) {
        _movementWidget.setScale(
            _model.m_settings.m_blockScale > 1 ? _model.m_settings.m_blockScale : 2 );
        _movementWidget.setSpacing( _model.m_settings.m_blockSpacing );
        _movementWidget.queue_resize( );

        updateSelection( );
    }
} // namespace UI::MED
