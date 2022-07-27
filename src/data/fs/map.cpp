#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>

#include "../../defines.h"
#include "../../log.h"
#include "../fs.h"
#include "../maprender.h"
#include "util.h"

namespace DATA {
    bool readTiles( FILE* p_file, tile* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        read( p_file, p_tileSet + p_startIdx, sizeof( tile ) * p_size, 1 );
        return true;
    }

    bool readPal( FILE* p_file, palette* p_palette, u8 p_count ) {
        if( p_file == 0 ) return false;
        read( p_file, p_palette, sizeof( u16 ) * 16, p_count );
        return true;
    }

    bool readBlocks( FILE* p_file, block* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        readNop( p_file, 4 );
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottom, 4 * sizeof( blockAtom ), 1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_top, 4 * sizeof( blockAtom ), 1 );
        }
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottombehave, sizeof( u8 ), 1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_topbehave, sizeof( u8 ), 1 );
        }
        return true;
    }

    bool writeTiles( FILE* p_file, const tile* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        write( p_file, p_tileSet + p_startIdx, sizeof( tile ) * p_size, 1 );
        return true;
    }

    bool writePal( FILE* p_file, const palette* p_palette, u8 p_count ) {
        if( p_file == 0 ) return false;
        write( p_file, p_palette, sizeof( u16 ) * 16, p_count );
        return true;
    }

    bool writeBlocks( FILE* p_file, const block* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        u8 magic[ 4 ] = { 0, 2, 0, 0 };
        fwrite( magic, 1, 4, p_file );
        for( u16 i = 0; i < p_size; ++i ) {
            write( p_file, &( p_tileSet + p_startIdx + i )->m_bottom, 4 * sizeof( blockAtom ), 1 );
            write( p_file, &( p_tileSet + p_startIdx + i )->m_top, 4 * sizeof( blockAtom ), 1 );
        }
        for( u16 i = 0; i < p_size; ++i ) {
            write( p_file, &( p_tileSet + p_startIdx + i )->m_bottombehave, sizeof( u8 ), 1 );
            write( p_file, &( p_tileSet + p_startIdx + i )->m_topbehave, sizeof( u8 ), 1 );
        }
        return true;
    }

    bool readMapSlice( FILE* p_mapFile, mapSlice* p_result, u16 p_x, u16 p_y, bool p_close ) {
        if( p_mapFile == 0 ) return false;

        p_result->m_x = p_x;
        p_result->m_y = p_y;

        //    printf( "constructing slice\n" );
        fread( &p_result->m_data, sizeof( mapSliceData ), 1, p_mapFile );
        if( p_close ) { fclose( p_mapFile ); }

        return true;
    }

    bool readLargeMap( FILE* p_mapFile, largeMapSliceHeader& p_header,
                       std::vector<std::vector<mapBlockAtom>>& p_result, bool p_close ) {
        if( p_mapFile == 0 ) return false;

        // properly read the data
        fread( &p_header, sizeof( largeMapSliceHeader ), 1, p_mapFile );
        // read border
        mapBlockAtom tmp;
        for( u16 i{ 0 }; i < p_header.m_borderSizeX * p_header.m_borderSizeY; ++i ) {
            fread( &tmp, sizeof( mapBlockAtom ), 1, p_mapFile );
        }
        // read map
        p_result.clear( );
        for( u16 y{ 0 }; y < p_header.m_sizeY; ++y ) {
            std::vector<mapBlockAtom> v{ };
            for( u16 x{ 0 }; x < p_header.m_sizeX; ++x ) {
                fread( &tmp, sizeof( mapBlockAtom ), 1, p_mapFile );
                v.push_back( tmp );
            }
            p_result.push_back( v );
        }

        if( p_close ) { fclose( p_mapFile ); }
        return true;
    }

    bool writeMapSlice( FILE* p_mapFile, const mapSlice* p_map, bool p_close ) {
        if( p_mapFile == 0 ) return false;
        fwrite( &p_map->m_data, sizeof( mapSliceData ), 1, p_mapFile );
        if( p_close ) { fclose( p_mapFile ); }

        return true;
    }

    bool readMapData( FILE* p_file, mapData* p_result, bool p_close ) {
        if( !p_file ) {
            std::memset( p_result, 0, sizeof( mapData ) );
            return false;
        }
        fread( p_result, sizeof( mapData ), 1, p_file );
        if( p_close ) { fclose( p_file ); }
        return true;
    }

    bool writeMapData( FILE* p_file, const mapData* p_data, bool p_close ) {
        if( !p_file || !p_data ) { return false; }
        fwrite( p_data, sizeof( mapData ), 1, p_file );
        if( p_close ) { fclose( p_file ); }
        return true;
    }

    bool readMapSliceAndData( FILE* p_mapFile, mapSlice* p_slice, mapData* p_data, u16 p_x,
                              u16 p_y ) {
        if( p_mapFile == 0 ) return false;

        fprintf( stderr, "%hu_%hu.map\n", p_y, p_x );

        mapBankInfo info;
        if( fseek( p_mapFile, 0, SEEK_SET ) ) { return false; }
        fread( &info, sizeof( mapBankInfo ), 1, p_mapFile );

        if( fseek( p_mapFile,
                   sizeof( mapBankInfo )
                       + ( ( info.m_sizeX + 1 ) * p_y + p_x )
                             * ( sizeof( mapSliceData ) + sizeof( mapData ) ),
                   SEEK_SET ) ) {
            return false;
        }

        if( !readMapSlice( p_mapFile, p_slice, p_x, p_y, false ) ) { return false; }
        if( !readMapData( p_mapFile, p_data, false ) ) { return false; }

        return true;
    }

    bool writeMapSliceAndData( FILE* p_mapFile, const mapBankInfo& p_info, const mapSlice* p_slice,
                               const mapData* p_data, u16 p_x, u16 p_y ) {
        if( p_mapFile == 0 ) return false;

        if( fseek( p_mapFile,
                   sizeof( mapBankInfo )
                       + ( ( p_info.m_sizeX + 1 ) * p_y + p_x )
                             * ( sizeof( mapSliceData ) + sizeof( mapData ) ),
                   SEEK_SET ) ) {
            return false;
        }

        if( !writeMapSlice( p_mapFile, p_slice, false ) ) { return false; }
        if( !writeMapData( p_mapFile, p_data, false ) ) { return false; }
        return true;
    }

    bool readMapBank( FILE* p_mapFile, mapBankInfo* p_info, mapBank* p_out ) {
        if( p_mapFile == 0 ) return false;
        if( fseek( p_mapFile, 0, SEEK_SET ) ) { return false; }
        fread( p_info, sizeof( mapBankInfo ), 1, p_mapFile );

        if( p_info->m_mapMode != MAPMODE_COMBINED ) { return true; }

        // the following could be read faster by using a struct and arrays, probably;
        // anyway, it doesn't really matter here

        p_out->m_slices.assign( p_info->m_sizeY + 1,
                                std::vector<mapSlice>( p_info->m_sizeX + 1, mapSlice( ) ) );
        p_out->m_mapData.assign( p_info->m_sizeY + 1,
                                 std::vector<mapData>( p_info->m_sizeX + 1, mapData( ) ) );

        for( u8 y = 0; y <= p_info->m_sizeY; ++y ) {
            for( u8 x = 0; x <= p_info->m_sizeX; ++x ) {
                mapSlice sl;
                mapData  dt;
                if( !readMapSlice( p_mapFile, &sl, x, y, false ) ) { return false; }
                if( !readMapData( p_mapFile, &dt, false ) ) { return false; }
                p_out->m_slices[ y ][ x ]  = sl;
                p_out->m_mapData[ y ][ x ] = dt;
            }
        }
        return true;
    }

    bool writeMapBank( FILE* p_mapFile, const mapBankInfo* p_info, const mapBank* p_bank ) {
        if( p_mapFile == 0 ) return false;
        if( fseek( p_mapFile, 0, SEEK_SET ) ) { return false; }

        if( p_info->m_sizeY + 1LU != p_bank->m_slices.size( )
            || p_info->m_sizeY + 1LU != p_bank->m_mapData.size( ) ) {
            message_error( "writeMapBank",
                           std::string( "Writing map bank failed. Should have height " )
                               + std::to_string( p_info->m_sizeY ) + ", but has heights "
                               + std::to_string( p_bank->m_slices.size( ) ) + " and "
                               + std::to_string( p_bank->m_mapData.size( ) ) + "." );
            return false;
        }

        fwrite( p_info, sizeof( mapBankInfo ), 1, p_mapFile );

        if( p_info->m_mapMode != MAPMODE_COMBINED ) { return true; }

        for( u8 y = 0; y <= p_info->m_sizeY; ++y ) {
            if( p_info->m_sizeX + 1LU != p_bank->m_slices[ y ].size( )
                || p_info->m_sizeX + 1LU != p_bank->m_mapData[ y ].size( ) ) {
                message_error( "writeMapBank",
                               std::string( "Writing map bank failed. Row " ) + std::to_string( y )
                                   + " have width " + std::to_string( p_info->m_sizeX )
                                   + ", but has heights "
                                   + std::to_string( p_bank->m_slices[ y ].size( ) ) + " and "
                                   + std::to_string( p_bank->m_mapData[ y ].size( ) ) + "." );

                return false;
            }

            for( u8 x = 0; x <= p_info->m_sizeX; ++x ) {
                if( !writeMapSlice( p_mapFile, &p_bank->m_slices[ y ][ x ], false ) ) {
                    return false;
                }
                if( !writeMapData( p_mapFile, &p_bank->m_mapData[ y ][ x ], false ) ) {
                    return false;
                }
            }
        }
        return true;
    }
} // namespace DATA
