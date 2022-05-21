#include "tileSelector.h"
#include "../../root.h"

namespace UI::TED {
    tileSelector::tileSelector( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        // _tileSetFrame = Gtk::Frame( "Tiles" );
        // _tileSetFrame.set_label_align( Gtk::Align::CENTER );
        _tileSetFrame.set_child( _mainBox );

        _mainBox.set_margin( MARGIN );
        _mainBox.set_vexpand( );

        _tileInfo = std::make_shared<tileInfo>( );
        if( _tileInfo ) {
            _mainBox.append( *_tileInfo );
            _tileInfo->connect(
                [ this ]( u8 p_newPal ) {
                    _model.m_settings.m_tseSelectedPalette = p_newPal;
                    _rootWindow.redraw( );
                },
                [ this ]( u8 p_flipX ) {
                    _model.m_settings.m_tseFlipX = p_flipX;
                    _rootWindow.redraw( );
                },
                [ this ]( u8 p_flipY ) {
                    _model.m_settings.m_tseFlipY = p_flipY;
                    _rootWindow.redraw( );
                } );
        }

        auto meScrolledWindow1 = Gtk::ScrolledWindow( );
        meScrolledWindow1.set_child( _ts1widget );
        meScrolledWindow1.set_margin( MARGIN );
        meScrolledWindow1.set_vexpand( );
        meScrolledWindow1.set_halign( Gtk::Align::CENTER );
        meScrolledWindow1.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mainBox.append( meScrolledWindow1 );

        _ts1widget.connectClick(
            [ this ]( mapSlice::clickType p_button, u16 p_tileX, u16 p_tileY ) {
                onTSClicked( p_button, p_tileX, p_tileY, 0 );
            } );

        auto meScrolledWindow2 = Gtk::ScrolledWindow( );
        meScrolledWindow2.set_child( _ts2widget );
        meScrolledWindow2.set_margin( MARGIN );
        meScrolledWindow2.set_vexpand( );
        meScrolledWindow2.set_halign( Gtk::Align::CENTER );
        meScrolledWindow2.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mainBox.append( meScrolledWindow2 );

        _ts2widget.connectClick(
            [ this ]( mapSlice::clickType p_button, u16 p_tileX, u16 p_tileY ) {
                onTSClicked( p_button, p_tileX, p_tileY, 1 );
            } );
    }

    void tileSelector::redraw( ) {
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _model.buildPalette( pals, _model.m_settings.m_tseBS1, _model.m_settings.m_tseBS2 );

        _ts1widget.setScale( _model.m_settings.m_tseScale ? _model.m_settings.m_tseScale - 1 : 1 );
        _ts1widget.setSpacing( _model.m_settings.m_tseSpacing );
        _ts1widget.queue_resize( );
        _ts2widget.setScale( _model.m_settings.m_tseScale ? _model.m_settings.m_tseScale - 1 : 1 );
        _ts2widget.setSpacing( _model.m_settings.m_tseSpacing );
        _ts2widget.queue_resize( );

        _ts1widget.set( _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ].m_tileSet, pals,
                        _model.m_settings.m_tseTileSetWidth );
        _ts2widget.set( _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ].m_tileSet, pals,
                        _model.m_settings.m_tseTileSetWidth );

        _ts1widget.setPal( _model.m_settings.m_tseSelectedPalette );
        _ts2widget.setPal( _model.m_settings.m_tseSelectedPalette );
        _ts1widget.setDaytime( _model.m_settings.m_tseDayTime );
        _ts2widget.setDaytime( _model.m_settings.m_tseDayTime );

        _ts1widget.draw( );
        _ts2widget.draw( );
        if( _model.m_settings.m_tseSelectedTile < DATA::MAX_TILES_PER_TILE_SET ) {
            _ts1widget.selectBlock( _model.m_settings.m_tseSelectedTile );
            _ts2widget.selectBlock( -1 );
        } else {
            _ts1widget.selectBlock( -1 );
            _ts2widget.selectBlock( _model.m_settings.m_tseSelectedTile
                                    - DATA::MAX_TILES_PER_TILE_SET );
        }

        if( _tileInfo ) {
            _tileInfo->setScale( _model.m_settings.m_tseScale + 1 );
            _tileInfo->setTile( { tileDataLookup( _model.m_settings.m_tseSelectedTile ),
                                  _model.m_settings.m_tseFlipX, _model.m_settings.m_tseFlipY,
                                  _model.m_settings.m_tseSelectedPalette },
                                _model.m_settings.m_tseSelectedTile );
            _tileInfo->redraw( pals, _model.m_settings.m_tseDayTime );
        }
    }

    void tileSelector::onTSClicked( mapSlice::clickType, u16 p_tileX, u16 p_tileY, u8 p_ts ) {
        u16 tile = p_tileY * _model.m_settings.m_tseTileSetWidth + p_tileX;
        _model.m_settings.m_tseSelectedTile = tile + p_ts * DATA::MAX_TILES_PER_TILE_SET;
        _rootWindow.redraw( );
    }
} // namespace UI::TED
