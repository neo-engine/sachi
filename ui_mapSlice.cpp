#include <cstring>

#include "ui_block.h"
#include "ui_mapSlice.h"

namespace UI {
    mapSlice::~mapSlice( ) {
        for( auto& im : _images ) { im->unparent( ); }
    }

    void mapSlice::set( const std::vector<DATA::computedBlock>& p_blocks,
                        DATA::palette p_pals[ 5 * 16 ], u16 p_blocksPerRow ) {
        _blocks       = p_blocks;
        _blocksPerRow = p_blocksPerRow;
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
        if( !_blocksPerRow ) { _blocksPerRow = DATA::SIZE; }
        _height = _blocks.size( ) / _blocksPerRow;
        if( _height * _blocksPerRow < _blocks.size( ) ) { ++_height; }
    }

    void mapSlice::setScale( u16 p_scale ) {
        if( p_scale ) { _currentScale = p_scale; }
    }

    void mapSlice::setSpacing( u16 p_blockSpacing ) {
        _blockSpacing = p_blockSpacing;
    }

    void mapSlice::redraw( u8 p_daytime ) {
        for( auto& im : _images ) { im->unparent( ); }
        _images.clear( );

        for( auto block : _blocks ) {
            auto im = UI::block::createImage( block, _pals, p_daytime );
            im->set_size_request( DATA::BLOCK_SIZE * _currentScale,
                                  DATA::BLOCK_SIZE * _currentScale );
            im->set_parent( *this );
            _images.push_back( im );
        }
    }

    Gtk::SizeRequestMode mapSlice::get_request_mode_vfunc( ) const {
        return Gtk::SizeRequestMode::CONSTANT_SIZE;
    }

    void mapSlice::measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum,
                                  int& p_natural, int& p_minimumBaseline,
                                  int& p_naturalBaseline ) const {
        p_minimumBaseline = -1;
        p_naturalBaseline = -1;

        if( p_orientation == Gtk::Orientation::HORIZONTAL ) {
            p_minimum = _blocksPerRow * _currentScale * DATA::BLOCK_SIZE
                        + ( _blocksPerRow - 1 ) * _blockSpacing;
            p_natural = _blocksPerRow * _currentScale * DATA::BLOCK_SIZE
                        + ( _blocksPerRow - 1 ) * _blockSpacing;
        } else {
            p_minimum
                = _height * _currentScale * DATA::BLOCK_SIZE + ( _height - 1 ) * _blockSpacing;
            p_natural
                = _height * _currentScale * DATA::BLOCK_SIZE + ( _height - 1 ) * _blockSpacing;
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

            u16 x = i % _blocksPerRow, y = i / _blocksPerRow;
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
} // namespace UI
