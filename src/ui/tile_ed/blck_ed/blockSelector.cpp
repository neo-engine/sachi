#include "blockSelector.h"
#include "../../root.h"

namespace UI::TED {
    blockSelector::blockSelector( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _blockSetFrame = Gtk::Frame( "Blocks" );
        _blockSetFrame.set_label_align( Gtk::Align::CENTER );
        _blockSetFrame.set_child( _mapEditorBlockSetBox );

        _mapEditorBlockSetBox.set_vexpand( );
        _mapEditorBlockSetBox.set_margin( MARGIN );

        _editBlock = std::make_shared<editableBlock>( );
        if( _editBlock ) {
            _mapEditorBlockSetBox.append( *_editBlock );
            _editBlock->connect(
                [ this ]( u8 p_newMajBehave ) {
                    if( _model.m_settings.m_tseSelectedBlock >= DATA::MAX_BLOCKS_PER_TILE_SET ) {
                        auto blk
                            = _model.m_settings.m_tseSelectedBlock - DATA::MAX_BLOCKS_PER_TILE_SET;
                        _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ]
                            .m_blockSet.m_blocks[ blk ]
                            .m_bottombehave
                            = p_newMajBehave;
                    } else {
                        auto blk = _model.m_settings.m_tseSelectedBlock;
                        _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ]
                            .m_blockSet.m_blocks[ blk ]
                            .m_bottombehave
                            = p_newMajBehave;
                    }
                    _model.markTileSetsChanged( );
                    _rootWindow.redraw( );
                },
                [ this ]( u8 p_newMinBehave ) {
                    if( _model.m_settings.m_tseSelectedBlock >= DATA::MAX_BLOCKS_PER_TILE_SET ) {
                        auto blk
                            = _model.m_settings.m_tseSelectedBlock - DATA::MAX_BLOCKS_PER_TILE_SET;
                        _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ]
                            .m_blockSet.m_blocks[ blk ]
                            .m_topbehave
                            = p_newMinBehave;
                    } else {
                        auto blk = _model.m_settings.m_tseSelectedBlock;
                        _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ]
                            .m_blockSet.m_blocks[ blk ]
                            .m_topbehave
                            = p_newMinBehave;
                    }
                    _model.markTileSetsChanged( );
                    _rootWindow.redraw( );
                } );
            for( u8 i{ 0 }; i < DATA::BLOCK_LAYERS; ++i ) {
                _editBlock->connectClick(
                    i, [ i, this ]( editableTiles::clickType p_c, u16 p_x, u16 p_y ) {
                        onBlockClicked( p_c, i, p_x, p_y );
                    } );
            }
        }

        auto meScrolledWindow1 = Gtk::ScrolledWindow( );
        meScrolledWindow1.set_child( _ts1widget );
        meScrolledWindow1.set_margin( MARGIN );
        meScrolledWindow1.set_vexpand( );
        meScrolledWindow1.set_halign( Gtk::Align::CENTER );
        meScrolledWindow1.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mapEditorBlockSetBox.append( meScrolledWindow1 );

        _ts1widget.connectClick(
            [ this ]( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                onTSClicked( p_button, p_blockX, p_blockY, 0 );
            } );

        auto meScrolledWindow2 = Gtk::ScrolledWindow( );
        meScrolledWindow2.set_child( _ts2widget );
        meScrolledWindow2.set_margin( MARGIN );
        meScrolledWindow2.set_vexpand( );
        meScrolledWindow2.set_halign( Gtk::Align::CENTER );
        meScrolledWindow2.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mapEditorBlockSetBox.append( meScrolledWindow2 );

        _ts2widget.connectClick(
            [ this ]( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                onTSClicked( p_button, p_blockX, p_blockY, 1 );
            } );
    }

    void blockSelector::redraw( ) {
        auto ts = DATA::tileSet<2>( );
        _model.buildTileSet( &ts, _model.m_settings.m_tseBS1, _model.m_settings.m_tseBS2 );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _model.buildPalette( pals, _model.m_settings.m_tseBS1, _model.m_settings.m_tseBS2 );

        _ts1widget.setScale( _model.m_settings.m_tseScale );
        _ts1widget.setSpacing( _model.m_settings.m_tseSpacing );
        _ts1widget.queue_resize( );
        _ts2widget.setScale( _model.m_settings.m_tseScale );
        _ts2widget.setSpacing( _model.m_settings.m_tseSpacing );
        _ts2widget.queue_resize( );

        _ts1widget.set(
            DATA::mapBlockAtom::computeBlockSet(
                &_model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ].m_blockSet, &ts ),
            pals, _model.m_settings.m_tseBlockSetWidth );
        _ts2widget.set(
            DATA::mapBlockAtom::computeBlockSet(
                &_model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ].m_blockSet, &ts ),
            pals, _model.m_settings.m_tseBlockSetWidth );

        _ts1widget.setDaytime( _model.m_settings.m_tseDayTime );
        _ts2widget.setDaytime( _model.m_settings.m_tseDayTime );

        _ts1widget.draw( );
        _ts2widget.draw( );
        if( _model.m_settings.m_tseSelectedBlock < DATA::MAX_BLOCKS_PER_TILE_SET ) {
            _ts1widget.selectBlock( _model.m_settings.m_tseSelectedBlock );
            _ts2widget.selectBlock( -1 );

        } else {
            _ts1widget.selectBlock( -1 );
            _ts2widget.selectBlock( _model.m_settings.m_tseSelectedBlock
                                    - DATA::MAX_BLOCKS_PER_TILE_SET );
        }

        if( _editBlock ) {
            _editBlock->setScale( _model.m_settings.m_tseScale >= 3 ? _model.m_settings.m_tseScale
                                                                    : 3 );
            _editBlock->setSpacing( _model.m_settings.m_tseSpacing );
            _editBlock->setBlock( blockDataLookup( _model.m_settings.m_tseSelectedBlock ),
                                  _model.m_settings.m_tseSelectedBlock );
            _editBlock->redraw( pals, _model.m_settings.m_tseDayTime );
        }
    }

    void blockSelector::onTSClicked( mapSlice::clickType, u16 p_blockX, u16 p_blockY, u8 p_ts ) {
        u16 block = p_blockY * _model.m_settings.m_blockSetWidth + p_blockX;
        _model.m_settings.m_tseSelectedBlock = block + p_ts * DATA::MAX_BLOCKS_PER_TILE_SET;
        _rootWindow.redraw( );
    }

    void blockSelector::onBlockClicked( editableTiles::clickType p_button, u8 p_layer, u16 p_tX,
                                        u16 p_tY ) {
        auto& block{ _model.m_settings.m_tseSelectedBlock < DATA::MAX_BLOCKS_PER_TILE_SET
                         ? _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ]
                               .m_blockSet.m_blocks[ _model.m_settings.m_tseSelectedBlock ]
                         : _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ]
                               .m_blockSet.m_blocks[ _model.m_settings.m_tseSelectedBlock
                                                     - DATA::MAX_BLOCKS_PER_TILE_SET ] };
        switch( p_button ) {
        default:
        case editableTiles::LEFT_DOUBLE: {
            if( p_layer == 0 ) { // top layer
                block.m_top[ p_tY ][ p_tX ]
                    = { _model.m_settings.m_tseSelectedTile, _model.m_settings.m_tseFlipX,
                        _model.m_settings.m_tseFlipY, _model.m_settings.m_tseSelectedPalette };
            } else if( p_layer == DATA::BLOCK_LAYERS - 1 ) { // bottom layer
                block.m_bottom[ p_tY ][ p_tX ]
                    = { _model.m_settings.m_tseSelectedTile, _model.m_settings.m_tseFlipX,
                        _model.m_settings.m_tseFlipY, _model.m_settings.m_tseSelectedPalette };
            }
            _model.markTileSetsChanged( );
            break;
        }
        case editableTiles::RIGHT_DOUBLE:
            if( p_layer == 0 ) { // top layer
                _model.m_settings.m_tseSelectedTile    = block.m_top[ p_tY ][ p_tX ].m_tileidx;
                _model.m_settings.m_tseFlipX           = block.m_top[ p_tY ][ p_tX ].m_vflip;
                _model.m_settings.m_tseFlipY           = block.m_top[ p_tY ][ p_tX ].m_hflip;
                _model.m_settings.m_tseSelectedPalette = block.m_top[ p_tY ][ p_tX ].m_palno;
            } else if( p_layer == DATA::BLOCK_LAYERS - 1 ) { // bottom layer
                _model.m_settings.m_tseSelectedTile    = block.m_bottom[ p_tY ][ p_tX ].m_tileidx;
                _model.m_settings.m_tseFlipX           = block.m_bottom[ p_tY ][ p_tX ].m_vflip;
                _model.m_settings.m_tseFlipY           = block.m_bottom[ p_tY ][ p_tX ].m_hflip;
                _model.m_settings.m_tseSelectedPalette = block.m_bottom[ p_tY ][ p_tX ].m_palno;
            }
            break;
        }
        _rootWindow.redraw( );
    }

} // namespace UI::TED
