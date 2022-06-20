#include <cstring>

#include "mapSlice.h"

namespace UI {
    std::string toHexString( u8 p_value ) {
        char buffer[ 10 ];
        snprintf( buffer, 5, "%hhX", p_value );
        return std::string( buffer );
    }

    mapSlice::~mapSlice( ) {
        for( auto& im : _images ) { im->unparent( ); }
    }

    void mapSlice::updateBlockMovement( u8 p_oldvalue, u8 p_movement, u16 p_x, u16 p_y ) {
        auto pos = p_x + p_y * getWidth( );
        _overlayMovement[ pos ]->get_style_context( )->remove_class(
            block::classForMovement( p_oldvalue ) );
        _overlayMovement[ pos ]->set_text( toHexString( p_movement ) );
        _overlayMovement[ pos ]->get_style_context( )->add_class(
            block::classForMovement( p_movement ) );
    }

    void mapSlice::selectBlock( s16 p_blockIdx ) {
        if( _currentSelectionIndex > -1 && _currentSelectionIndex < (int) _images.size( )
            && _images[ _currentSelectionIndex ] ) {
            _images[ _currentSelectionIndex ]->remove_overlay( _selectionBox );
        }
        if( p_blockIdx >= 0 && p_blockIdx < (int) _images.size( ) ) {
            _images[ p_blockIdx ]->add_overlay( _selectionBox );
        }
        if( p_blockIdx >= -1 && p_blockIdx < (int) _images.size( ) ) {
            _currentSelectionIndex = p_blockIdx;
        }

        _selectionBox.get_style_context( )->add_class( "mapblock-selected" );
    }

    void mapSlice::setScale( u16 p_scale ) {
        if( p_scale ) {
            _currentScale = p_scale;

            _selectionBox.set_size_request( _currentScale * DATA::BLOCK_SIZE - 4,
                                            _currentScale * DATA::BLOCK_SIZE - 4 );
        }
    }

    void mapSlice::setSpacing( u16 p_blockSpacing ) {
        _blockSpacing = p_blockSpacing;
    }

    void mapSlice::setOverlayOpacity( double p_newValue ) {
        _overlayOpacity = p_newValue;
        for( auto mnt : _overlayMovement ) { mnt->set_opacity( _overlayOpacity ); }
    }

    void mapSlice::setOverlayHidden( bool p_hidden ) {
        _showOverlay = !p_hidden;
        for( auto i : _overlayMovement ) {
            if( _showOverlay ) {
                i->show( );
            } else {
                i->hide( );
            }
        }
    }

    void mapSlice::redrawBlock( u16 p_blockIdx ) {
        _images[ p_blockIdx ]->unparent( );
        _images[ p_blockIdx ] = std::make_shared<Gtk::Overlay>( );

        _imageData[ p_blockIdx ] = computeImageData( p_blockIdx );
        auto im                  = Gtk::Image( );
        im.set( _imageData[ p_blockIdx ] );
        _images[ p_blockIdx ]->set_child( im );
        _images[ p_blockIdx ]->set_parent( *this );

        auto movement                  = computeMovementData( p_blockIdx );
        _overlayMovement[ p_blockIdx ] = std::make_shared<Gtk::Label>( toHexString( movement ) );
        if( colorMovement( ) ) {
            _overlayMovement[ p_blockIdx ]->get_style_context( )->add_class(
                block::classForMovement( movement ) );
        }
        _overlayMovement[ p_blockIdx ]->set_opacity( _overlayOpacity );

        _images[ p_blockIdx ]->add_overlay( *_overlayMovement[ p_blockIdx ] );
    }

    void mapSlice::draw( ) {
        auto oldsel = _currentSelectionIndex;
        selectBlock( -1 );
        for( auto& im : _images ) { im->unparent( ); }
        _images.clear( );
        _imageData.clear( );
        _overlayMovement.clear( );

        auto numblocks = getWidth( ) * getHeight( );

        for( u16 pos{ 0 }; pos < numblocks; ++pos ) {
            auto pb = computeImageData( pos );
            _imageData.push_back( pb );
            auto im = Gtk::Image( );
            im.set( pb );
            im.set_size_request( DATA::BLOCK_SIZE, DATA::BLOCK_SIZE );
            auto overlay = std::make_shared<Gtk::Overlay>( );
            overlay->set_child( im );
            overlay->set_parent( *this );
            _images.push_back( overlay );

            auto movement = computeMovementData( pos );
            auto mnt      = std::make_shared<Gtk::Label>( toHexString( movement ) );
            if( colorMovement( ) ) {
                mnt->get_style_context( )->add_class( block::classForMovement( movement ) );
            }
            mnt->set_opacity( _overlayOpacity );
            overlay->add_overlay( *mnt );
            if( !_showOverlay ) {
                mnt->hide( );
            } else {
                mnt->show( );
            }

            _overlayMovement.push_back( mnt );
        }
        selectBlock( oldsel );
    }

    Gtk::SizeRequestMode mapSlice::get_request_mode_vfunc( ) const {
        return Gtk::SizeRequestMode::CONSTANT_SIZE;
    }

    void mapSlice::measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum,
                                  int& p_natural, int& p_minimumBaseline,
                                  int& p_naturalBaseline ) const {
        p_minimumBaseline = -1;
        p_naturalBaseline = -1;

        if( _images.empty( ) ) {
            p_minimum = 0;
            p_natural = 0;
            return;
        }

        if( p_orientation == Gtk::Orientation::HORIZONTAL ) {
            p_minimum = getWidth( ) * _currentScale * DATA::BLOCK_SIZE
                        + ( getWidth( ) - 1 ) * _blockSpacing;
            p_natural = getWidth( ) * _currentScale * DATA::BLOCK_SIZE
                        + ( getWidth( ) - 1 ) * _blockSpacing;
        } else {
            p_minimum = getHeight( ) * _currentScale * DATA::BLOCK_SIZE
                        + ( getHeight( ) - 1 ) * _blockSpacing;
            p_natural = getHeight( ) * _currentScale * DATA::BLOCK_SIZE
                        + ( getHeight( ) - 1 ) * _blockSpacing;
        }
    }

    void mapSlice::size_allocate_vfunc( int, int, int p_baseline ) {
        // make sure bordering blocks stay glued together, even if we get surplus space
        for( size_t i = 0; i < _images.size( ); ++i ) {
            auto& im = _images[ i ];

            // make dummy calls to measure to suppress warnings (yes we do know how big
            // every block should be.)
            int ignore;
            im->measure( Gtk::Orientation::HORIZONTAL, -1, ignore, ignore, ignore, ignore );

            Gtk::Allocation allo;

            u16 x = i % getWidth( ), y = i / getWidth( );
            u16 sx = x * _currentScale * DATA::BLOCK_SIZE;
            u16 sy = y * _currentScale * DATA::BLOCK_SIZE;
            sx += x * _blockSpacing;
            sy += y * _blockSpacing;

            allo.set_x( sx );
            allo.set_y( sy );
            auto width  = _currentScale * DATA::BLOCK_SIZE;
            auto height = _currentScale * DATA::BLOCK_SIZE;
            allo.set_width( width );
            allo.set_height( height );
            im->size_allocate( allo, p_baseline );
        }
    }

    void lookupMapSlice::updateBlock( const DATA::mapBlockAtom& p_block, u16 p_x, u16 p_y ) {
        auto pos{ p_x + p_y * _blocksPerRow };
        _blocks[ pos ] = p_block;
        redrawBlock( pos );
    }

    void lookupMapSlice::updateBlockMovement( u8 p_movement, u16 p_x, u16 p_y ) {
        auto pos{ p_x + p_y * _blocksPerRow };
        mapSlice::updateBlockMovement( _blocks[ pos ].m_movedata, p_movement, p_x, p_y );
        _blocks[ pos ].m_movedata = p_movement;
    }

    void lookupMapSlice::set(
        const std::vector<DATA::mapBlockAtom>&                                   p_blocks,
        const std::function<std::shared_ptr<Gdk::Pixbuf>( DATA::mapBlockAtom )>& p_lookupFunction,
        u16                                                                      p_blocksPerRow ) {
        _blocks         = p_blocks;
        _blocksPerRow   = p_blocksPerRow;
        _lookupFunction = p_lookupFunction;
        if( !_blocksPerRow ) { _blocksPerRow = DATA::SIZE; }
        _height = _blocks.size( ) / _blocksPerRow;
        if( _height * _blocksPerRow < _blocks.size( ) ) { ++_height; }
    }

    void computedMapSlice::updateBlock( const DATA::computedBlock& p_block, u16 p_x, u16 p_y ) {
        auto pos{ p_x + p_y * _blocksPerRow };
        _blocks[ pos ] = { p_block, _blocks[ pos ].second };
        redrawBlock( pos );
    }

    void computedMapSlice::updateBlockMovement( u8 p_movement, u16 p_x, u16 p_y ) {
        auto pos{ p_x + p_y * _blocksPerRow };
        mapSlice::updateBlockMovement( _blocks[ pos ].second, p_movement, p_x, p_y );
        _blocks[ pos ] = { _blocks[ pos ].first, p_movement };
    }

    void computedMapSlice::set( const std::vector<std::pair<DATA::computedBlock, u8>>& p_blocks,
                                DATA::palette p_pals[ 5 * 16 ], u16 p_blocksPerRow ) {
        _blocks       = p_blocks;
        _blocksPerRow = p_blocksPerRow;
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
        if( !_blocksPerRow ) { _blocksPerRow = DATA::SIZE; }
        _height = _blocks.size( ) / _blocksPerRow;
        if( _height * _blocksPerRow < _blocks.size( ) ) { ++_height; }
    }

    void tileSetMapSlice::set( const DATA::tileSet<1>& p_tiles, DATA::palette p_pals[ 5 * 16 ],
                               u16 p_tilesPerRow ) {
        _tiles       = p_tiles;
        _tilesPerRow = p_tilesPerRow;
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
        if( !_tilesPerRow ) { _tilesPerRow = DATA::SIZE; }
        _height = DATA::MAX_TILES_PER_TILE_SET / _tilesPerRow;
        if( _height * _tilesPerRow < DATA::MAX_TILES_PER_TILE_SET ) { ++_height; }
    }

    void tileSlice::set( const DATA::tile& p_tile, DATA::palette p_pals[ 5 * 16 ] ) {
        _tile = p_tile;
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
    }

    void colorSlice::set( const std::vector<u16>& p_colors, u16 p_colorsPerRow ) {
        _data         = p_colors;
        _colorsPerRow = p_colorsPerRow;
        if( !_colorsPerRow ) { _colorsPerRow = _data.size( ); }
        _height = _data.size( ) / _colorsPerRow;
        if( _height * _colorsPerRow < _data.size( ) ) { ++_height; }
    }

} // namespace UI
