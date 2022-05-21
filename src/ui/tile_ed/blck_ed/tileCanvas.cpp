#include <queue>

#include "../../root.h"
#include "tileCanvas.h"

namespace UI::TED {
    tileCanvas::tileCanvas( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _frame = Gtk::Frame( );
        _frame.set_child( _mainBox );

        _mainBox.set_margin( MARGIN );
        _mainBox.set_vexpand( );

        auto f2{ Gtk::Frame( "Pal" ) };
        f2.set_child( _pal );

        auto meScrolledWindow1 = Gtk::ScrolledWindow( );
        meScrolledWindow1.set_child( _tile );
        meScrolledWindow1.set_margin( MARGIN );
        meScrolledWindow1.set_vexpand( );
        meScrolledWindow1.set_halign( Gtk::Align::CENTER );
        meScrolledWindow1.set_valign( Gtk::Align::CENTER );
        meScrolledWindow1.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::NEVER );
        _mainBox.append( meScrolledWindow1 );

        auto meScrolledWindow2 = Gtk::ScrolledWindow( );
        meScrolledWindow2.set_child( f2 );
        meScrolledWindow2.set_vexpand( );
        meScrolledWindow2.set_halign( Gtk::Align::CENTER );
        meScrolledWindow2.set_valign( Gtk::Align::CENTER );
        meScrolledWindow2.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::NEVER );
        _mainBox.append( meScrolledWindow2 );

        _tile.set_hexpand( );
        _tile.connectClick( [ this ]( mapSlice::clickType p_button, u16 p_tileX, u16 p_tileY ) {
            onTileClicked( p_button, p_tileX, p_tileY );
        } );

        _pal.connectClick( [ this ]( mapSlice::clickType p_button, u16 p_tileX, u16 p_tileY ) {
            onPalClicked( p_button, p_tileX, p_tileY );
        } );
    }

    void tileCanvas::redraw( ) {
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _model.buildPalette( pals, _model.m_settings.m_tseBS1, _model.m_settings.m_tseBS2 );

        _tile.setScale( _model.m_settings.m_tseScale );
        _tile.setSpacing( _model.m_settings.m_tseSpacing );
        _tile.queue_resize( );

        auto ps{ _model.m_settings.m_tseScale < 2
                     ? 2
                     : ( _model.m_settings.m_tseScale > 3 ? 3 : _model.m_settings.m_tseScale ) };

        _pal.setScale( ps );
        _pal.setSpacing( _model.m_settings.m_tseSpacing );
        _pal.queue_resize( );

        if( _model.m_settings.m_tseSelectedTile < DATA::MAX_TILES_PER_TILE_SET ) {
            _tile.set( _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ]
                           .m_tileSet.m_tiles[ _model.m_settings.m_tseSelectedTile ],
                       pals );
        } else {
            _tile.set(
                _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ]
                    .m_tileSet
                    .m_tiles[ _model.m_settings.m_tseSelectedTile - DATA::MAX_TILES_PER_TILE_SET ],
                pals );
        }
        std::vector<u16> pal = std::vector<u16>( 16, 0 );
        for( u8 c{ 0 }; c < 16; ++c ) {
            pal[ c ] = pals[ 16 * _model.m_settings.m_tseDayTime
                             + _model.m_settings.m_tseSelectedPalette ]
                           .m_pal[ c ];
        }
        _pal.set( std::move( pal ), 1 );
        _pal.setOverlayHidden( false );

        _tile.setPal( _model.m_settings.m_tseSelectedPalette );
        _tile.setDaytime( _model.m_settings.m_tseDayTime );
        _tile.setOverlayHidden( _model.m_settings.m_tseTileOverlay );

        _tile.draw( );
        _pal.draw( );
        _tile.selectBlock( _model.m_settings.m_tseSelectedTileIdx );
        _pal.selectBlock( _model.m_settings.m_tseSelectedPalIdx );
    }

    void tileCanvas::onTileClicked( mapSlice::clickType p_clickType, u16 p_tileX, u16 p_tileY ) {
        u16 tile                               = p_tileY * tileSlice::TILE_SIZE + p_tileX;
        _model.m_settings.m_tseSelectedTileIdx = tile;
        auto& tdata = ( _model.m_settings.m_tseSelectedTile < DATA::MAX_TILES_PER_TILE_SET )
                          ? _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS1 ]
                                .m_tileSet.m_tiles[ _model.m_settings.m_tseSelectedTile ]
                          : _model.m_fsdata.m_blockSets[ _model.m_settings.m_tseBS2 ]
                                .m_tileSet.m_tiles[ _model.m_settings.m_tseSelectedTile
                                                    - DATA::MAX_TILES_PER_TILE_SET ];

        switch( p_clickType ) {
        default:
        case mapSlice::LEFT: { // change tile color to selected color
            tdata.set( p_tileX, p_tileY, _model.m_settings.m_tseSelectedPalIdx );
            _model.markTileSetsChanged( );
            break;
        }
        case mapSlice::MIDDLE: {
            auto oldcl{ tdata.at( p_tileX, p_tileY ) };

            std::queue<std::pair<u8, u8>> q;
            q.push( { p_tileX, p_tileY } );
            while( !q.empty( ) ) {
                auto [ cx, cy ] = q.front( );
                q.pop( );
                tdata.set( cx, cy, _model.m_settings.m_tseSelectedPalIdx );
                for( s8 x{ -1 }; x <= 1; ++x ) {
                    for( s8 y{ -1 }; y <= 1; ++y ) {
                        if( cx + x < 0 || cx + x > 8 || cy + y < 0 || cy + y > 8 ) { continue; }
                        if( tdata.at( cx + x, cy + y ) == oldcl ) { q.push( { cx + x, cy + y } ); }
                    }
                }
            }
            _model.markTileSetsChanged( );
            break;
        }
        case mapSlice::RIGHT: { // select color
            _model.m_settings.m_tseSelectedPalIdx = tdata.at( p_tileX, p_tileY );
            break;
        }
        }

        _rootWindow.redraw( );
    }

    void tileCanvas::onPalClicked( mapSlice::clickType, u16 p_tileX, u16 p_tileY ) {
        u16 tile                              = p_tileY * 1 + p_tileX;
        _model.m_settings.m_tseSelectedPalIdx = tile;
        _rootWindow.redraw( );
    }
} // namespace UI::TED
