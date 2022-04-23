#include <cstring>

#include "block.h"
#include "mapBankOverview.h"

namespace UI {
    constexpr u16 DOWN_SCALE = 8;

    std::shared_ptr<Gtk::Image> mapBankOverview::createImage( const DATA::computedMapSlice& p_slice,
                                                              u8 p_daytime ) {
        auto btm = new DATA::bitmap( DATA::BLOCK_SIZE * DATA::SIZE / DOWN_SCALE,
                                     DATA::BLOCK_SIZE * DATA::SIZE / DOWN_SCALE );
        DATA::renderMapSlice( &p_slice, btm, 0, 0, DOWN_SCALE, p_daytime );

        //    btm->writeToFile( ( "/tmp/" + std::to_string( cnt++ ) + ".png" ).c_str( ) );

        auto pixbuf = btm->pixbuf( );

        auto res = std::make_shared<Gtk::Image>( );
        res->set( pixbuf );

        delete btm;
        return res;
    }

    mapBankOverview::~mapBankOverview( ) {
        for( auto& im : _images ) { im->unparent( ); }
    }

    void mapBankOverview::selectMap( s16 p_mapIdx ) {
        if( _currentSelectionIndex != -1 ) {
            _images[ _currentSelectionIndex ]->remove_overlay( _selectionBox );
        }
        if( p_mapIdx >= 0 && p_mapIdx < int( _images.size( ) ) ) {
            _images[ p_mapIdx ]->add_overlay( _selectionBox );
            _currentSelectionIndex = p_mapIdx;
        } else {
            _currentSelectionIndex = -1;
        }

        _selectionBox.get_style_context( )->add_class( "mapblock-selected" );
    }

    void mapBankOverview::setScale( u16 p_scale ) {
        if( p_scale ) {
            _mapScale = p_scale;

            _selectionBox.set_size_request( _mapScale * DATA::BLOCK_SIZE - 4,
                                            _mapScale * DATA::BLOCK_SIZE - 4 );
        }
    }

    void mapBankOverview::setSpacing( u16 p_mapSpacing ) {
        _mapSpacing = p_mapSpacing;
    }

    void mapBankOverview::set( const std::vector<std::vector<DATA::computedMapSlice>>& p_bank ) {
        _mapBank = p_bank;
    }

    void mapBankOverview::replaceMap( const DATA::computedMapSlice& p_map, u8 p_mapY, u8 p_mapX ) {
        if( p_mapY >= _mapBank.size( ) || p_mapX >= _mapBank[ p_mapY ].size( ) ) [[unlikely]] {
            return;
        }
        auto oldsel = _currentSelectionIndex;
        selectMap( -1 );

        _mapBank[ p_mapY ][ p_mapX ] = p_map;
        auto imidx                   = p_mapY * _mapBank[ p_mapY ].size( ) + p_mapX;
        if( imidx < _images.size( ) ) {
            auto& oim = _images[ imidx ];
            oim->unparent( );

            auto im = createImage( p_map, _currentDaytime );

            im->set_size_request( DATA::BLOCK_SIZE, DATA::BLOCK_SIZE );

            oim = std::make_shared<Gtk::Overlay>( );
            oim->set_child( *im );
            oim->set_parent( *this );
        }
        selectMap( oldsel );
    }

    void mapBankOverview::redraw( u8 p_daytime ) {
        auto oldsel = _currentSelectionIndex;
        selectMap( -1 );
        _currentDaytime = p_daytime;
        for( auto& im : _images ) { im->unparent( ); }
        _images.clear( );

        for( auto row : _mapBank ) {
            for( auto slice : row ) {
                auto im = createImage( slice, _currentDaytime );
                im->set_size_request( DATA::BLOCK_SIZE, DATA::BLOCK_SIZE );

                auto overlay = std::make_shared<Gtk::Overlay>( );
                overlay->set_child( *im );
                overlay->set_parent( *this );
                _images.push_back( overlay );
            }
        }
        selectMap( oldsel );
    }

    Gtk::SizeRequestMode mapBankOverview::get_request_mode_vfunc( ) const {
        return Gtk::SizeRequestMode::CONSTANT_SIZE;
    }
    void mapBankOverview::measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum,
                                         int& p_natural, int& p_minimumBaseline,
                                         int& p_naturalBaseline ) const {
        p_minimumBaseline = -1;
        p_naturalBaseline = -1;

        if( _mapBank.empty( ) ) {
            p_minimum = 0;
            p_natural = 0;
            return;
        }

        auto slicesPerRow = _mapBank[ 0 ].size( );
        auto height       = _mapBank.size( );

        if( p_orientation == Gtk::Orientation::HORIZONTAL ) {
            p_minimum
                = slicesPerRow * _mapScale * DATA::BLOCK_SIZE + ( slicesPerRow - 1 ) * _mapSpacing;
            p_natural
                = slicesPerRow * _mapScale * DATA::BLOCK_SIZE + ( slicesPerRow - 1 ) * _mapSpacing;
        } else {
            p_minimum = height * _mapScale * DATA::BLOCK_SIZE + ( height - 1 ) * _mapSpacing;
            p_natural = height * _mapScale * DATA::BLOCK_SIZE + ( height - 1 ) * _mapSpacing;
        }
    }

    void mapBankOverview::size_allocate_vfunc( int, int, int p_baseline ) {
        if( _images.empty( ) ) { return; }

        // make sure bordering slices stay glued together, even if we get surplus space
        for( size_t y = 0; y < _mapBank.size( ); ++y ) {
            for( size_t x = 0; x < _mapBank[ 0 ].size( ); ++x ) {
                if( y * _mapBank[ 0 ].size( ) + x >= _images.size( ) ) { continue; }

                auto& im = _images[ y * _mapBank[ 0 ].size( ) + x ];
                if( im == nullptr ) [[unlikely]] { continue; }

                // make dummy calls to measure to suppress warnings (yes we do know how big
                // every block should be.)
                int ignore;
                im->measure( Gtk::Orientation::HORIZONTAL, -1, ignore, ignore, ignore, ignore );

                Gtk::Allocation allo;

                u16 sx = x * _mapScale * DATA::BLOCK_SIZE;
                u16 sy = y * _mapScale * DATA::BLOCK_SIZE;
                sx += x * _mapSpacing;
                sy += y * _mapSpacing;

                allo.set_x( sx );
                allo.set_y( sy );
                auto width  = _mapScale * DATA::BLOCK_SIZE;
                auto height = _mapScale * DATA::BLOCK_SIZE;
                allo.set_width( width );
                allo.set_height( height );
                im->size_allocate( allo, p_baseline );
            }
        }
    }

} // namespace UI
