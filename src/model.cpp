#include <cstring>

#include "data/fs/util.h"
#include "data/util.h"
#include "defines.h"
#include "log.h"
#include "model.h"

bool model::loadNewFsRoot( const std::string& p_path ) {
    if( !DATA::checkOrCreatePath( p_path ) ) { return false; }
    if( !DATA::checkOrCreatePath( p_path + "/MAPS/" ) ) { return false; }

    message_log( "load FSROOT", std::string( "Loading FSROOT " ) + p_path );

    m_fsdata.m_fsrootPath = p_path;
    m_fsdata.m_mapBanks.clear( );

    /*
    m_fsdata.m_blockSets.clear( );
    _blockSetNames.clear( );
    */

    selectBank( -1 );

    // traverse MAP_PATH and search for all dirs that are named with a number
    // (assuming they are a map bank)

    std::error_code ec;
    for( auto& p : fs::directory_iterator( m_fsdata.mapPath( ), ec ) ) {
        if( p.is_directory( ec ) && !ec ) {
            std::string name         = p.path( ).filename( );
            int         nameAsNumber = -1;
            try {
                nameAsNumber = std::stoi( name );
            } catch( ... ) { continue; }

            if( nameAsNumber < 0 || nameAsNumber > MAX_MAPBANK_NAME ) {
                message_error( "load FSROOT", std::string( "Skipping potential map bank " ) + name
                                                  + ": name (as number) too large." );
                continue;
            }

            auto info = DATA::exploreMapBank( p.path( ) );
            addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_mapMode );
        } else if( p.is_regular_file( ec ) && !ec ) {
            // try to open the file and read the map bank information from it
            std::string name         = p.path( ).filename( );
            int         nameAsNumber = -1;
            try {
                nameAsNumber = std::stoi( name );
            } catch( ... ) { continue; }

            if( nameAsNumber < 0 || nameAsNumber > MAX_MAPBANK_NAME ) {
                message_error( "load FSROOT", std::string( "Skipping potential map bank " ) + name
                                                  + ": name (as number) too large." );
                continue;
            }

            DATA::mapBankInfo info;
            FILE*             f = fopen( p.path( ).c_str( ), "rb" );
            if( !f ) {
                message_error( "load FSROOT", std::string( "Skipping potential map bank " ) + name
                                                  + ": file not readible" );
                continue;
            }

            fread( &info, sizeof( DATA::mapBankInfo ), 1, f );
            fclose( f );

            addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_mapMode );
        }
    }

    // load all tilesets and blocksets
    // if( readTileSets( ) ) { return; }

    return true;
}

void model::addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, u8 p_mapMode, status p_status ) {
    if( m_fsdata.m_mapBanks.count( p_bank ) ) { return; }

    auto nmc                      = fsdata::mapBankContainer{ };
    nmc.m_info                    = DATA::mapBankInfo( p_sizeX, p_sizeY, p_mapMode );
    nmc.m_status                  = p_status;
    m_fsdata.m_mapBanks[ p_bank ] = std::move( nmc );
}

bool model::readMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path,
                          bool p_readMapData ) {
    if( !existsBank( p_bank ) ) { return false; }

    auto& selb = bank( p_bank );
    auto  sl   = DATA::mapSlice( );
    auto  dt   = DATA::mapData( );

    auto path = fs::path( m_fsdata.mapPath( ) );

    if( !selb.isCombined( ) || p_path != "" ) {
        path /= std::to_string( p_bank );
        if( selb.isScattered( ) ) { path /= std::to_string( p_mapY ); }
        path /= ( std::to_string( p_mapY ) + "_" + std::to_string( p_mapX ) + ".map" );

        if( p_path != "" ) { path = p_path; }

        FILE* f = fopen( path.c_str( ), "rb" );

        if( !DATA::readMapSlice( f, &sl, p_mapX, p_mapY ) ) {
            message_log( "load slice", std::string( "Loading map " ) + std::to_string( p_bank )
                                           + "/" + std::to_string( p_mapY ) + "_"
                                           + std::to_string( p_mapX ) + ".map failed. (path "
                                           + path.c_str( ) + ")" );
        } else {
            selb.m_bank.m_slices[ p_mapY ][ p_mapX ] = std::move( sl );
        }

        if( p_readMapData ) {
            path += ".data";
            f = fopen( path.c_str( ), "rb" );
            if( !DATA::readMapData( f, &dt ) ) {
                message_log( "load slice",
                             std::string( "Loading map data " ) + std::to_string( p_bank ) + "/"
                                 + std::to_string( p_mapY ) + "_" + std::to_string( p_mapX )
                                 + ".map failed. (path " + path.c_str( ) + ")" );
            } else {
                selb.m_bank.m_mapData[ p_mapY ][ p_mapX ] = std::move( dt );
            }
        }
    } else {
        path /= ( std::to_string( p_bank ) + ".bank" );

        // Load map bank, FSEEK to correct position
        FILE* f = fopen( path.c_str( ), "rb" );

        if( !DATA::readMapSliceAndData( f, &sl, &dt, p_mapX, p_mapY ) ) {
            message_log( "load slice",
                         std::string( "Loading map and map data " ) + std::to_string( p_bank ) + "/"
                             + std::to_string( p_mapY ) + "_" + std::to_string( p_mapX )
                             + ".map failed. (path " + path.c_str( ) + ")" );
        } else {
            selb.m_bank.m_slices[ p_mapY ][ p_mapX ] = std::move( sl );
            if( p_readMapData ) { selb.m_bank.m_mapData[ p_mapY ][ p_mapX ] = std::move( dt ); }
        }
    }
    return true;
}

bool model::checkOrLoadBank( int p_bank, bool p_forceRead ) {
    if( p_bank < 0 || p_bank > MAX_MAPBANK_NAME ) { return false; }

    if( !existsBank( p_bank ) ) { return false; }

    auto& selb = bank( p_bank );
    message_log( "load bank", std::string( "Loading map bank " ) + std::to_string( p_bank ) );

    // Load all maps of the bank into mem
    if( !selb.m_loaded || p_forceRead ) {
        // Load the *bank file
        auto path
            = fs::path( m_fsdata.mapPath( ) ) / fs::path( std::to_string( p_bank ) + ".bank" );
        FILE* f = fopen( path.c_str( ), "rb" );

        if( !DATA::readMapBank( f, &selb.m_info, &selb.m_bank ) ) {
            message_log( "load bank", std::string( "Map bank file " ) + std::to_string( p_bank )
                                          + ".bank does not exist (path " + path.c_str( )
                                          + "). Trying on my own." );
            // If the file does not exist, selb.m_info should already exist and
            // contain useful information, so we are good
        }
        if( f ) { fclose( f ); }

        if( !selb.isCombined( ) ) {
            // not a combined map bank, we need to read the data ourselves
            selb.m_bank.m_slices = std::vector<std::vector<DATA::mapSlice>>(
                selb.getSizeY( ) + 1,
                std::vector<DATA::mapSlice>( selb.getSizeX( ) + 1, DATA::mapSlice( ) ) );
            selb.m_bank.m_mapData = std::vector<std::vector<DATA::mapData>>(
                selb.getSizeY( ) + 1,
                std::vector<DATA::mapData>( selb.getSizeX( ) + 1, DATA::mapData( ) ) );
            for( u16 y = 0; y <= selb.getSizeY( ); ++y ) {
                for( u16 x = 0; x <= selb.getSizeX( ); ++x ) { readMapSlice( p_bank, x, y ); }
            }
        }

        selb.m_computedBank = std::vector<std::vector<DATA::computedMapSlice>>(
            selb.getSizeY( ) + 1, std::vector<DATA::computedMapSlice>(
                                      selb.getSizeX( ) + 1, DATA::computedMapSlice( ) ) );
        for( u16 y = 0; y <= selb.getSizeY( ); ++y ) {
            for( u16 x = 0; x <= selb.getSizeX( ); ++x ) {
                auto bs = DATA::blockSet<2>( );
                buildBlockSet( &bs, selb.m_bank.m_slices[ y ][ x ].m_data.m_tIdx1,
                               selb.m_bank.m_slices[ y ][ x ].m_data.m_tIdx2 );
                auto ts = DATA::tileSet<2>( );
                buildTileSet( &ts, selb.m_bank.m_slices[ y ][ x ].m_data.m_tIdx1,
                              selb.m_bank.m_slices[ y ][ x ].m_data.m_tIdx2 );

                buildPalette( selb.m_computedBank[ y ][ x ].m_pals,
                              selb.m_bank.m_slices[ y ][ x ].m_data.m_tIdx1,
                              selb.m_bank.m_slices[ y ][ x ].m_data.m_tIdx2 );
                selb.m_computedBank[ y ][ x ].m_computedBlocks
                    = selb.m_bank.m_slices[ y ][ x ].compute( &bs, &ts );
            }
        }

        selb.m_loaded = true;
    }

    return true;
}

void model::buildBlockSet( DATA::blockSet<2>* p_out, s8 p_ts1, s8 p_ts2 ) {
    u8 ts1 = 0;
    u8 ts2 = 0;
    if( p_ts1 != -1 ) {
        ts1 = p_ts1;
    } else {
        ts1 = slice( ).m_data.m_tIdx1;
    }
    if( p_ts2 != -1 ) {
        ts2 = p_ts2;
    } else {
        ts2 = slice( ).m_data.m_tIdx2;
    }
    std::memcpy( p_out->m_blocks, m_fsdata.m_blockSets[ ts1 ].m_blockSet.m_blocks,
                 sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
    std::memcpy( &p_out->m_blocks[ DATA::MAX_BLOCKS_PER_TILE_SET ],
                 m_fsdata.m_blockSets[ ts2 ].m_blockSet.m_blocks,
                 sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
}

void model::buildTileSet( DATA::tileSet<2>* p_out, s8 p_ts1, s8 p_ts2 ) {
    u8 ts1 = 0;
    u8 ts2 = 0;
    if( p_ts1 != -1 ) {
        ts1 = p_ts1;
    } else {
        ts1 = slice( ).m_data.m_tIdx1;
    }
    if( p_ts2 != -1 ) {
        ts2 = p_ts2;
    } else {
        ts2 = slice( ).m_data.m_tIdx2;
    }
    std::memcpy( p_out->m_tiles, m_fsdata.m_blockSets[ ts1 ].m_tileSet.m_tiles,
                 sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
    std::memcpy( &p_out->m_tiles[ DATA::MAX_TILES_PER_TILE_SET ],
                 m_fsdata.m_blockSets[ ts2 ].m_tileSet.m_tiles,
                 sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
}

void model::buildPalette( DATA::palette p_out[ 5 * 16 ], s8 p_ts1, s8 p_ts2 ) {
    u8 ts1 = 0;
    u8 ts2 = 0;
    if( p_ts1 != -1 ) {
        ts1 = p_ts1;
    } else {
        ts1 = slice( ).m_data.m_tIdx1;
    }
    if( p_ts2 != -1 ) {
        ts2 = p_ts2;
    } else {
        ts2 = slice( ).m_data.m_tIdx2;
    }
    for( u8 dt = 0; dt < DAYTIMES; ++dt ) {
        std::memcpy( &p_out[ 16 * dt ], &m_fsdata.m_blockSets[ ts1 ].m_pals[ 8 * dt ],
                     sizeof( DATA::palette ) * 8 );
        std::memcpy( &p_out[ 16 * dt + 6 ], &m_fsdata.m_blockSets[ ts2 ].m_pals[ 8 * dt ],
                     sizeof( DATA::palette ) * 8 );
    }
}

void model::createBlockSet( u8 p_tsIdx ) {
    if( m_fsdata.m_blockSets.count( p_tsIdx ) ) { return; }

    m_fsdata.m_blockSetNames.insert( p_tsIdx );
    m_fsdata.m_blockSets[ p_tsIdx ] = fsdata::blockSetInfo( );

    m_fsdata.m_mapBankStrList = std::vector<Glib::ustring>( );

    for( auto bsname : m_fsdata.m_blockSetNames ) {
        m_fsdata.m_mapBankStrList.push_back( std::to_string( bsname ) );
        m_fsdata.m_blockSets[ bsname ].m_stringListItem = m_fsdata.m_mapBankStrList.size( ) - 1;
    }
    // m_fsdata.m_mapBankStrList.splice( 0, m_fsdata.m_mapBankStrList.get_n_items( ),
    //                                   std::move( bsnames ) );

    markTileSetsChanged( );
}
