// Creates a combined image of the map files. Optionally shrinks the image and marks
// routes, etc

#include <bit>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <png.h>
#include "data_bitmap.h"
#include "data_fs.h"
#include "data_locationNames.h"
#include "data_maprender.h"

std::string MAP_PATH;
std::string TILESET_PATH;
std::string BLOCKSET_PATH;
std::string PALETTE_PATH;
std::string MAPDATA_PATH;

namespace DATA {
    void printColor( u16 p_color, u16 p_data ) {
        fprintf( stderr, "\x1b[48;2;%u;%u;%um%04hx\x1b[0;00m", red( p_color ), blue( p_color ),
                 green( p_color ), p_data );
    }

    void palette::dump( ) const {
        for( uint8_t j = 0; j < 16; ++j ) { printColor( m_pal[ j ], m_pal[ j ] ); }
        fprintf( stderr, "\n" );
    }

    void tile::dump( ) const {
        for( u8 i = 0; i < 4; ++i ) {
            for( u8 j = 0; j < 8; ++j ) { fprintf( stderr, "%02x", m_tile[ 8 * i + j ] ); }
            fprintf( stderr, "\n" );
        }
        fprintf( stderr, "\n" );
    }

    void renderTile( const tile* p_tile, const palette* p_pal, bool p_flipX, bool p_flipY,
                     bitmap* p_out, u32 p_x, u32 p_y, u16 p_scale ) {
        // a tile is a 8 by 8 pixel palette indexed image; each pixel occupies 4 bits

        for( u8 x = 0; x < 8 / p_scale; x += p_scale ) {
            for( u8 y = 0; y < 8 / p_scale; y += p_scale ) {
                u8 px = x, py = y;
                if( p_flipX ) { px = 7 - x; }
                if( p_flipY ) { py = 7 - y; }

                // get palette idx for this pixel
                u8 idx = p_tile->m_tile[ 4 * py + px / 2 ];
                if( !p_flipX ) {
                    if( x & 1 ) {
                        idx >>= 4;
                    } else {
                        idx &= 15;
                    }
                } else {
                    if( x & 1 ) {
                        idx &= 15;
                    } else {
                        idx >>= 4;
                    }
                }

                if( !idx ) { continue; } // first pal entry is "transparent"

                // get color for the palette idx
                u16 color = p_pal->m_pal[ idx ];

                ( *p_out )( p_x + x / p_scale, p_y + y / p_scale )
                    = pixel( red( color ), green( color ), blue( color ) );
            }
        }
    }

    void renderBlock( const block* p_block, const tileSet<>* p_tiles,
                      const palette p_pals[ 16 * 5 ], bitmap* p_out, u32 p_x, u32 p_y, u16 p_scale,
                      u8 p_time ) {
        u16 tilesize = 8 / p_scale;
        for( u16 x = 0; x < 2; ++x ) {
            for( u16 y = 0; y < 2; ++y ) {
                // render the bottom tile first
                blockAtom ba = p_block->m_bottom[ y ][ x ];
                renderTile( &p_tiles->m_tiles[ ba.m_tileidx ], &p_pals[ 16 * p_time + ba.m_palno ],
                            ba.m_vflip, ba.m_hflip, p_out, p_x + x * tilesize, p_y + y * tilesize,
                            p_scale );

                // draw top tile on top of bottom tile
                ba = p_block->m_top[ y ][ x ];
                renderTile( &p_tiles->m_tiles[ ba.m_tileidx ], &p_pals[ 16 * p_time + ba.m_palno ],
                            ba.m_vflip, ba.m_hflip, p_out, p_x + x * tilesize, p_y + y * tilesize,
                            p_scale );
            }
        }
    }

    void renderBlock( const computedBlock* p_block, const palette p_pals[ 16 * 5 ], bitmap* p_out,
                      u32 p_x, u32 p_y, u16 p_scale, u8 p_time ) {
        u16 tilesize = 8 / p_scale;
        for( u16 x = 0; x < 2; ++x ) {
            for( u16 y = 0; y < 2; ++y ) {
                // render the bottom tile first
                auto ba = p_block->m_bottom[ y ][ x ];
                renderTile( &ba.m_tile, &p_pals[ 16 * p_time + ba.m_palno ], ba.m_vflip, ba.m_hflip,
                            p_out, p_x + x * tilesize, p_y + y * tilesize, p_scale );

                // draw top tile on top of bottom tile
                ba = p_block->m_top[ y ][ x ];
                renderTile( &ba.m_tile, &p_pals[ 16 * p_time + ba.m_palno ], ba.m_vflip, ba.m_hflip,
                            p_out, p_x + x * tilesize, p_y + y * tilesize, p_scale );
            }
        }
    }

    /*
     * @brief: renders the given mapSlice and outputs starting at the specified upper left
     * corner of the given bitmap.
     */
    void renderMapSlice( const mapSlice* p_mapSlice, const blockSet<>* p_blockSet,
                         const tileSet<>* p_tileSet, const palette p_pals[ 16 * 5 ], bitmap* p_out,
                         u32 p_x, u32 p_y, u16 p_scale, u8 p_time ) {
        u16 blocksize = BLOCK_SIZE / p_scale;

        for( u16 x = 0; x < SIZE; ++x ) {
            for( u16 y = 0; y < SIZE; ++y ) {
                renderBlock( &p_blockSet->m_blocks[ p_mapSlice->m_blocks[ y ][ x ].m_blockidx ],
                             p_tileSet, p_pals, p_out, p_x + blocksize * x, p_y + blocksize * y,
                             p_scale, p_time );
            }
        }
    }

    /*
     * @brief: renders the given mapSlice and outputs starting at the specified upper left
     * corner of the given bitmap.
     */
    void renderMapSlice( const computedMapSlice* p_mapSlice, bitmap* p_out, u32 p_x, u32 p_y,
                         u16 p_scale, u8 p_time ) {
        u16 blocksize = BLOCK_SIZE / p_scale;
        u16 pos       = 0;

        for( u16 y = 0; y < SIZE; ++y ) {
            for( u16 x = 0; x < SIZE; ++x, ++pos ) {
                renderBlock( &p_mapSlice->m_computedBlocks[ pos ].first, p_mapSlice->m_pals, p_out,
                             p_x + blocksize * x, p_y + blocksize * y, p_scale, p_time );
            }
        }
    }

    constexpr pixel tint( const pixel& p_target, const pixel& p_tint, u8 ) {
        return p_target & p_tint;
    }

    mapSlice SLICE;

#define TINT_FORCE 1
    void tintRectangle( bitmap& p_out, u32 p_tx, u32 p_ty, u32 p_bx, u32 p_by, pixel p_tint,
                        u32 p_borderStrength, pixel p_border, u8 p_borderSides ) {

        u32 topb   = ( p_borderSides & 1 ) ? p_borderStrength : 0;
        u32 leftb  = ( p_borderSides & 2 ) ? p_borderStrength : 0;
        u32 downb  = ( p_borderSides & 4 ) ? p_borderStrength : 0;
        u32 rightb = ( p_borderSides & 8 ) ? p_borderStrength : 0;

        for( u32 y = p_ty + topb; y < p_by - downb; ++y ) {
            for( u32 x = p_tx + leftb; x < p_bx - rightb; ++x ) {
                p_out( x, y ) = tint( p_out( x, y ), p_tint, TINT_FORCE );
            }
        }

        // edges
        for( u32 y = p_ty + topb; y < p_by - downb; ++y ) {
            for( u32 x = p_tx; x < p_tx + leftb; ++x ) {
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
        for( u32 y = p_ty + topb; y < p_by - downb; ++y ) {
            for( u32 x = p_bx - rightb; x < p_bx; ++x ) {
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
        for( u32 y = p_ty; y < p_ty + topb; ++y ) {
            for( u32 x = p_tx + leftb; x < p_bx - rightb; ++x ) {
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
        for( u32 y = p_by - downb; y < p_by; ++y ) {
            for( u32 x = p_tx + leftb; x < p_bx - rightb; ++x ) {
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }

        // corners
        for( u32 y = p_ty; y < p_ty + topb; ++y ) {
            for( u32 x = p_tx; x < p_tx + leftb; ++x ) {
                if( p_borderStrength > 1 && x - p_tx + y - p_ty <= p_borderStrength ) { continue; }
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
        for( u32 y = p_by - downb; y < p_by; ++y ) {
            for( u32 x = p_tx; x < p_tx + leftb; ++x ) {
                if( p_borderStrength > 1 && x - p_tx + p_by - y <= p_borderStrength ) { continue; }
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
        for( u32 y = p_ty; y < p_ty + topb; ++y ) {
            for( u32 x = p_bx - rightb; x < p_bx; ++x ) {
                if( p_borderStrength > 1 && p_bx - x + y - p_ty <= p_borderStrength ) { continue; }
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
        for( u32 y = p_by - downb; y < p_by; ++y ) {
            for( u32 x = p_bx - rightb; x < p_bx; ++x ) {
                if( p_borderStrength > 1 && p_bx - x + p_by - y <= p_borderStrength ) { continue; }
                p_out( x, y ) = tint( p_out( x, y ), p_border, TINT_FORCE );
            }
        }
    }
} // namespace DATA

/*
int main( int p_argc, char* p_argv[] ) {
    if( p_argc < 9 ) {
        printf(
            "Too few arguments.\nUsage: %s MAP_BANK_PATH TILE_SETS_PATH BLOCKS_PATH "
            " PALETTES_PATH #MAPS_X #MAPS_Y MAP_BANK OUTPUT_FILENAME [TIME_OF_DAY] [SCALE]
[GRID_SIZE] [GRID_SUBDIVS] [MAPDATA_PATH]", p_argv[ 0 ] );
    }

    MAP_PATH      = std::string( p_argv[ 1 ] );
    TILESET_PATH  = std::string( p_argv[ 2 ] );
    BLOCKSET_PATH = std::string( p_argv[ 3 ] );
    PALETTE_PATH  = std::string( p_argv[ 4 ] );

    u16 mapsX, mapsY, bank;
    sscanf( p_argv[ 5 ], "%hu", &mapsX );
    sscanf( p_argv[ 6 ], "%hu", &mapsY );
    sscanf( p_argv[ 7 ], "%hu", &bank );

    u8 scale = 1, time = 0, oldscale = 1;
    if( p_argc >= 10 ) { sscanf( p_argv[ 9 ], "%hhu", &time ); }
    if( p_argc >= 11 ) { sscanf( p_argv[ 10 ], "%hhu", &oldscale ); }

    u32 grid = 0, divs = 0;
    if( p_argc >= 12 ) { sscanf( p_argv[ 11 ], "%u", &grid ); }
    if( p_argc >= 13 ) { sscanf( p_argv[ 12 ], "%u", &divs ); }
    bool mapdata = false;
    if( p_argc >= 14 ) {
        mapdata      = true;
        MAPDATA_PATH = std::string( p_argv[ 13 ] );
    }

    u32 sizeX = mapsX * SIZE * BLOCK_SIZE / scale;
    u32 sizeY = mapsY * SIZE * BLOCK_SIZE / scale;

    bitmap out( sizeX, sizeY );
    char   buffer[ 50 ];

    for( u16 x = 0; x < mapsX; ++x ) {
        for( u16 y = 0; y < mapsY; ++y ) {
            // try to open map x|y

            FILE* mapF;

            // fprintf( stderr, "Map %d/%d,%d\n", bank, y, x );

            if( bank == 10 ) {
                snprintf( buffer, 49, "%hhu/%hu/%hu_%hu", bank, y, y, x );
                mapF = open( MAP_PATH.c_str( ), buffer, ".map" );
            } else {
                snprintf( buffer, 49, "%hhu/%hu_%hu", bank, y, x );
                mapF = open( MAP_PATH.c_str( ), buffer, ".map" );
            }
            if( !mapF ) {
                fprintf( stderr, "Map %d/%d,%d does not exist.\n", bank, y, x );
                continue;
            }

            //            printf( "file opened\n" );

            // construct slice
            constructMapSlice( mapF, &SLICE, x, y );

            //            printf( "slice constructed\n" );

            // draw slice
            renderMapSlice( &SLICE, &out, x * SIZE * BLOCK_SIZE / scale,
                            y * SIZE * BLOCK_SIZE / scale, scale, time );

            //            printf( "slice printed\n" );
        }
    }

    if( oldscale > 1 ) {
        printf( "Scaling down with factor %hhu.\n", oldscale );
        bitmap sout( sizeX / oldscale, sizeY / oldscale );

        // combine oldscale * oldscale pixel into a single new one

        for( u32 x = 0; x < sizeX / oldscale; ++x ) {
            for( u32 y = 0; y < sizeY / oldscale; ++y ) {
                pixel res = out( x * oldscale, y * oldscale );
                for( u32 x2 = 0; x2 < oldscale; ++x2 ) {
                    for( u32 y2 = 0; y2 < oldscale; ++y2 ) {
                        res ^= out( x * oldscale + x2, y * oldscale + y2 );
                    }
                }
                sout( x, y ) = res;
            }
        }
        out   = sout;
        scale = oldscale;
    }

    if( mapdata ) {
        mapData                       tmp;
        std::vector<std::vector<u16>> locs
            = std::vector<std::vector<u16>>( divs * sizeY, std::vector<u16>( divs * sizeX, 0 )
);

        auto bordercol = pixel( 255, 255, 255, 20 );

        for( u16 x = 0; x < mapsX; ++x ) {
            for( u16 y = 0; y < mapsY; ++y ) {
                // try to open map x|y

                FILE* mapF;

                // fprintf( stderr, "Map %d/%d,%d\n", bank, y, x );

                snprintf( buffer, 49, "%hhu/%hu/%hu_%hu", bank, y, y, x );
                mapF = open( MAPDATA_PATH.c_str( ), buffer, ".map.data" );
                if( !mapF ) {
                    fprintf( stderr, "Map data for %d/%d,%d does not exist.\n", bank, y, x );
                    continue;
                }

                readMapData( mapF, tmp );

                if( tmp.m_baseLocationId ) {
                    for( u8 x2 = 0; x2 < divs; ++x2 ) {
                        for( u8 y2 = 0; y2 < divs; ++y2 ) {
                            locs[ divs * y + y2 ][ divs * x + x2 ] = tmp.m_baseLocationId;
                        }
                    }
                }
                for( u8 i = 0; i < tmp.m_extraLocationCount; ++i ) {
                    for( u8 x2 = tmp.m_extraLocations[ i ].m_left * divs / SIZE;
                         x2 <= tmp.m_extraLocations[ i ].m_right * divs / SIZE; ++x2 ) {
                        for( u8 y2 = tmp.m_extraLocations[ i ].m_top * divs / SIZE;
                             y2 <= tmp.m_extraLocations[ i ].m_bottom * divs / SIZE; ++y2 ) {
                            locs[ divs * y + y2 ][ divs * x + x2 ]
                                = tmp.m_extraLocations[ i ].m_locationId;
                        }
                    }
                }
            }
        }

        printf( "{\n" );
        for( u16 y = 0; y < divs * mapsY; ++y ) {
            printf( "    {" );
            for( u16 x = 0; x < divs * mapsX; ++x ) { printf( " %hu,", locs[ y ][ x ] ); }
            printf( " },\n" );
        }
        printf( "}\n" );

        for( u16 y = 0; y < divs * mapsY; ++y ) {
            for( u16 x = 0; x < divs * mapsX; ++x ) {
                //                printf( "%04u ", locs[ y ][ x ] );
                if( !locs[ y ][ x ] ) { continue; }
                u8 border = 0;

                if( !y || locs[ y ][ x ] != locs[ y - 1 ][ x ] ) { border |= 1; }
                if( !x || locs[ y ][ x ] != locs[ y ][ x - 1 ] ) { border |= 2; }
                if( y + 1 >= divs * mapsY || locs[ y ][ x ] != locs[ y + 1 ][ x ] ) { border |=
4; } if( x + 1 >= divs * mapsX || locs[ y ][ x ] != locs[ y ][ x + 1 ] ) { border |= 8; }

                u8 borsize = std::max( 1, BLOCK_SIZE / scale );
                if( borsize == 1 && std::popcount( border ) >= 3
                    && colorForLocation( locs[ y ][ x ] ) != colorForLocation( L_ROUTE_110 ) ) {
                    borsize = 0;
                }
                if( borsize == 1 && ( border == 0b101 || border == 0b1010 )
                    && colorForLocation( locs[ y ][ x ] ) == colorForLocation( L_ISLAND_CAVE ) )
{ borsize = 0;
                }

                tintRectangle( out, x * ( SIZE / divs ) * BLOCK_SIZE / scale,
                               y * ( SIZE / divs ) * BLOCK_SIZE / scale,
                               ( x + 1 ) * ( SIZE / divs ) * BLOCK_SIZE / scale,
                               ( y + 1 ) * ( SIZE / divs ) * BLOCK_SIZE / scale,
                               colorForLocation( locs[ y ][ x ] ), borsize, bordercol, border );
            }
            //          printf( "\n" );
        }
    }

    if( grid ) {
        grid = ( grid * BLOCK_SIZE ) / scale;
        printf( "Drawing grid every %u pixel with %u subdivisions.\n", grid, divs );

        for( u32 x = 0; x < mapsX * SIZE * BLOCK_SIZE / scale; ++x ) {
            for( u32 y = 0; y < mapsY * SIZE * BLOCK_SIZE / scale; ++y ) {
                if( ( ( x % grid ) == 0 || ( y % grid ) == 0 ) ) {
                    out( x, y ) = pixel( 255, 255, 255 );
                    continue;
                }
                if( grid / divs ) {
                    if( ( x % ( grid / divs ) ) == 0 || ( y % ( grid / divs ) ) == 0 ) {
                        out( x, y ) = pixel( 150, 150, 150 );
                        continue;
                    }
                }
            }
        }
    }

    out.writeToFile( p_argv[ 8 ] );
    return 0;
}
*/
