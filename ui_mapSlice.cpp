#include <cstring>

#include "ui_block.h"
#include "ui_mapSlice.h"

namespace UI {
    std::string toHexString( u8 p_value ) {
        char buffer[ 10 ];
        snprintf( buffer, 5, "%hhX", p_value );
        return std::string( buffer );
    }

    mapSlice::~mapSlice( ) {
        for( auto& im : _images ) { im->unparent( ); }
    }

    void mapSlice::selectBlock( s16 p_blockIdx ) {
        if( _currentSelectionIndex != -1 ) {
            _images[ _currentSelectionIndex ]->remove_overlay( _selectionBox );
        }
        if( p_blockIdx >= 0 ) { _images[ p_blockIdx ]->add_overlay( _selectionBox ); }
        _currentSelectionIndex = p_blockIdx;

        _selectionBox.get_style_context( )->add_class( "mapblock-selected" );
    }

    void mapSlice::updateBlock( const DATA::computedBlock& p_block, u16 p_x, u16 p_y ) {
        auto pos       = p_x + p_y * _blocksPerRow;
        _blocks[ pos ] = { p_block, _blocks[ pos ].second };
        _images[ pos ]->unparent( );
        _images[ pos ] = std::make_shared<Gtk::Overlay>( );
        _images[ pos ]->set_child(
            *UI::block::createImage( _blocks[ pos ].first, _pals, _currentDaytime ) );
        _images[ pos ]->set_parent( *this );
        _images[ pos ]->add_overlay( *_overlayMovement[ pos ] );
    }

    void mapSlice::updateBlockMovement( u8 p_movement, u16 p_x, u16 p_y ) {
        auto pos = p_x + p_y * _blocksPerRow;
        _overlayMovement[ pos ]->get_style_context( )->remove_class(
            block::classForMovement( _blocks[ pos ].second ) );
        _blocks[ pos ] = { _blocks[ pos ].first, p_movement };
        _overlayMovement[ pos ]->set_text( toHexString( p_movement ) );
        _overlayMovement[ pos ]->get_style_context( )->add_class(
            block::classForMovement( _blocks[ pos ].second ) );
    }

    void mapSlice::set( const std::vector<std::pair<DATA::computedBlock, u8>>& p_blocks,
                        DATA::palette p_pals[ 5 * 16 ], u16 p_blocksPerRow ) {
        _blocks       = p_blocks;
        _blocksPerRow = p_blocksPerRow;
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
        if( !_blocksPerRow ) { _blocksPerRow = DATA::SIZE; }
        _height = _blocks.size( ) / _blocksPerRow;
        if( _height * _blocksPerRow < _blocks.size( ) ) { ++_height; }
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

    void mapSlice::redraw( u8 p_daytime, bool p_overlay ) {
        auto oldsel = _currentSelectionIndex;
        selectBlock( -1 );
        _currentDaytime = p_daytime;
        _showOverlay    = p_overlay;
        for( auto& im : _images ) { im->unparent( ); }
        _images.clear( );
        _overlayMovement.clear( );

        for( auto [ block, movement ] : _blocks ) {
            auto im = UI::block::createImage( block, _pals, _currentDaytime );
            im->set_size_request( DATA::BLOCK_SIZE, DATA::BLOCK_SIZE );

            auto overlay = std::make_shared<Gtk::Overlay>( );
            overlay->set_child( *im );
            overlay->set_parent( *this );
            _images.push_back( overlay );

            auto mnt = std::make_shared<Gtk::Label>( toHexString( movement ) );
            mnt->get_style_context( )->add_class( block::classForMovement( movement ) );
            mnt->set_opacity( _overlayOpacity );
            overlay->add_overlay( *mnt );
            if( !p_overlay ) {
                mnt->hide( );
            } else {
                mnt->show( );
            }
            _overlayMovement.push_back( mnt );
        }
        selectBlock( oldsel );
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
