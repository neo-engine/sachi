#include "blockStamp.h"
#include "../../root.h"

namespace UI::MED {
    blockStamp::blockStamp( model& p_model, root& p_root, mapEditor& p_parent )
        : _model{ p_model }, _rootWindow{ p_root }, _mapEditor{ p_parent } {
    }

    void blockStamp::reset( ) {
        if( _blockStampDialog && !_blockStampDialogInvalid ) { _blockStampDialog->hide( ); }
        _blockStampDialogInvalid = true;
    }

    void blockStamp::create( ) {
        if( _blockStampDialogInvalid ) { // Block stamp dialog
            _blockStampDialog
                = std::make_shared<Gtk::Dialog>( "Block Stamp", _rootWindow, false, true );
            _blockStampDialog->get_content_area( )->append( _blockStampMap );
            _blockStampDialog->signal_close_request( ).connect(
                [ this ]( ) -> bool {
                    _blockStampDialogInvalid = true;
                    return false;
                },
                false );
            _blockStampDialogInvalid = false;
        }
    }

    void blockStamp::update( const std::vector<DATA::mapBlockAtom>& p_blockData, u16 p_width ) {
        _blockStampWidth = p_width;
        _blockStampData  = p_blockData;

        _blockStampMap.set(
            _blockStampData,
            [ this ]( DATA::mapBlockAtom p_block ) {
                return _mapEditor.blockSetLookup( p_block.m_blockidx );
            },
            _blockStampWidth );
        _blockStampMap.draw( );
        _blockStampMap.setOverlayHidden( _currentMapDisplayMode != mapEditor::MODE_EDIT_MOVEMENT );
        _blockStampDialog->show( );
    }

    void blockStamp::redraw( ) {
        _blockStampMap.draw( );
        _blockStampMap.setOverlayHidden( _currentMapDisplayMode != mapEditor::MODE_EDIT_MOVEMENT );
        _blockStampMap.setSpacing( _model.m_settings.m_blockSpacing );
        _blockStampMap.setScale( _model.m_settings.m_blockScale );

        _blockStampMap.queue_resize( );
    }
} // namespace UI::MED
