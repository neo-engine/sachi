#include <cstring>

#include "data/fs/util.h"
#include "data/util.h"
#include "defines.h"
#include "log.h"
#include "model.h"

model model::readFromPath( const std::string& p_path ) {
    model res;
    if( !DATA::checkOrCreatePath( p_path ) ) { return res; }
    if( !DATA::checkOrCreatePath( p_path + "/MAPS/" ) ) { return res; }

    message_log( "load FSROOT", std::string( "Loading FSROOT " ) + p_path );

    res.m_fsdata.m_fsrootPath = p_path;

    // traverse m_fsdata.mapPath() and search for all dirs that are named with a number
    // (assuming they are a map bank)

    std::error_code ec;
    for( auto& p : fs::directory_iterator( res.m_fsdata.mapPath( ), ec ) ) {
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
            res.addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_mapMode );
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

            res.addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_mapMode );
        }
    }

    // load all tilesets and blocksets
    if( res.readTileSets( ) ) { return res; }

    res.m_good = true;
    return res;
}

void model::addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, u8 p_mapMode, status p_status ) {
    if( m_fsdata.m_mapBanks.count( p_bank ) ) { return; }

    auto nmc     = fsdata::mapBankContainer{ };
    nmc.m_info   = DATA::mapBankInfo( p_sizeX, p_sizeY, p_mapMode );
    nmc.m_status = p_status;

    nmc.m_bank.m_slices = std::vector<std::vector<DATA::mapSlice>>(
        nmc.getSizeY( ) + 1,
        std::vector<DATA::mapSlice>( nmc.getSizeX( ) + 1, DATA::mapSlice( ) ) );
    nmc.m_bank.m_mapData = std::vector<std::vector<DATA::mapData>>(
        nmc.getSizeY( ) + 1, std::vector<DATA::mapData>( nmc.getSizeX( ) + 1, DATA::mapData( ) ) );

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
            message_error( "load slice", std::string( "Loading map " ) + std::to_string( p_bank )
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
                message_error( "load slice",
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
            message_error( "load slice",
                           std::string( "Loading map and map data " ) + std::to_string( p_bank )
                               + "/" + std::to_string( p_mapY ) + "_" + std::to_string( p_mapX )
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
            for( u16 y{ 0 }; y <= selb.getSizeY( ); ++y ) {
                for( u16 x{ 0 }; x <= selb.getSizeX( ); ++x ) { readMapSlice( p_bank, x, y ); }
            }
        }

        selb.m_computedBank = std::vector<std::vector<DATA::computedMapSlice>>(
            selb.getSizeY( ) + 1, std::vector<DATA::computedMapSlice>(
                                      selb.getSizeX( ) + 1, DATA::computedMapSlice( ) ) );
        for( u16 y{ 0 }; y <= selb.getSizeY( ); ++y ) {
            for( u16 x{ 0 }; x <= selb.getSizeX( ); ++x ) {
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

bool model::readTileSets( ) {
    // first check if a combined file exists
    bool error = false;

    FILE* f;
    auto  pth = fs::path( m_fsdata.mapPath( ) ) / "tileset.tsb";
    f         = fopen( pth.c_str( ), "rb" );

    if( f ) {
        m_fsdata.m_tileSetMode = DATA::TILEMODE_COMBINED;

        // read header
        auto header = DATA::blockSetBankHeader( );
        fread( &header, sizeof( DATA::blockSetBankHeader ), 1, f );

        for( auto i{ 0 }; i < header.m_blockSetCount; ++i ) {
            auto bInfo = fsdata::blockSetInfo( );
            error |= !DATA::readTiles( f, bInfo.m_tileSet.m_tiles );
            error |= !DATA::readBlocks( f, bInfo.m_blockSet.m_blocks );
            if( header.m_dayTimeCount <= DAYTIMES ) {
                error |= !DATA::readPal( f, bInfo.m_pals, 8 * header.m_dayTimeCount );
            } else {
                error |= !DATA::readPal( f, bInfo.m_pals, 8 * DAYTIMES );
                DATA::readNop( f, sizeof( u16 ) * 16 * 8 * ( header.m_dayTimeCount - DAYTIMES ) );
            }

            m_fsdata.m_blockSets[ i ] = bInfo;
            m_fsdata.m_blockSetNames.insert( i );
        }

        message_log( "readTileSets", "Read " + std::to_string( header.m_blockSetCount )
                                         + " TS from combined tile set." );
        fclose( f );
    } else {
        if( !DATA::checkOrCreatePath( m_fsdata.tilesetPath( ) ) ) { return true; }
        if( !DATA::checkOrCreatePath( m_fsdata.blocksetPath( ) ) ) { return true; }
        if( !DATA::checkOrCreatePath( m_fsdata.palettePath( ) ) ) { return true; }
        m_fsdata.m_tileSetMode = DATA::TILEMODE_DEFAULT;
        std::error_code ec;

        for( auto& p : fs::directory_iterator( m_fsdata.blocksetPath( ) ) ) {
            if( !p.is_regular_file( ec ) || ec ) { continue; }

            u8 bsname;
            if( 1 != sscanf( p.path( ).filename( ).c_str( ), BLOCKSET_FORMAT.c_str( ), &bsname ) ) {
                continue;
            }

            auto res = fsdata::blockSetInfo( );
            f        = fopen( p.path( ).c_str( ), "r" );
            if( !DATA::readBlocks( f, res.m_blockSet.m_blocks ) ) {
                message_error( "readTileSets",
                               ( "Reading block set "s ) + std::to_string( bsname ) + " failed." );
                continue;
            }
            fclose( f );

            // read corresponding tileset
            auto tspath
                = fs::path( m_fsdata.tilesetPath( ) ) / ( std::to_string( bsname ) + ".ts" );
            f = fopen( tspath.c_str( ), "r" );
            if( !DATA::readTiles( f, res.m_tileSet.m_tiles ) ) {
                message_error( "readTileSets",
                               ( "Reading tile set "s ) + std::to_string( bsname ) + " failed." );
                // continue;
            } else {
                fclose( f );
            }

            // read corresponding palettes
            auto palpath
                = fs::path( m_fsdata.palettePath( ) ) / ( std::to_string( bsname ) + ".p2l" );
            f = fopen( palpath.c_str( ), "r" );
            if( !DATA::readPal( f, res.m_pals, 8 * 5 ) ) {
                message_error( "readTileSets",
                               ( "Reading palette "s ) + std::to_string( bsname ) + " failed." );
                // continue;
            } else {
                fclose( f );
            }

            // fprintf( stderr, "[LOG] Loaded blockset %hhu.\n", bsname );

            m_fsdata.m_blockSets[ bsname ] = res;
            m_fsdata.m_blockSetNames.insert( bsname );
        }
    }

    m_fsdata.m_mapBankStrList.clear( );

    for( auto bsname : m_fsdata.m_blockSetNames ) {
        m_fsdata.m_mapBankStrList.push_back( std::to_string( bsname ) );
        m_fsdata.m_blockSets[ bsname ].m_stringListItem = m_fsdata.m_mapBankStrList.size( ) - 1;
    }

    return error;
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

bool model::writeMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path,
                           bool p_writeMapData ) {
    if( !existsBank( p_bank ) ) { return true; }

    auto        path = fs::path( m_fsdata.mapPath( ) ) / std::to_string( p_bank );
    const auto& info = bank( p_bank );
    if( info.isScattered( ) ) { path /= std::to_string( p_mapY ); }
    path /= std::to_string( p_mapY ) + "_" + std::to_string( p_mapX ) + ".map";

    if( p_path != "" ) { path = p_path; }

    fs::create_directories( path.parent_path( ) );
    FILE* f = fopen( path.c_str( ), "w" );
    if( !DATA::writeMapSlice( f, &info.m_bank.m_slices[ p_mapY ][ p_mapX ] ) ) {
        message_error( "writeMapSlice", ( "Writing map "s ) + std::to_string( p_bank ) + ( "/"s )
                                            + std::to_string( p_mapY ) + ( "_"s )
                                            + std::to_string( p_mapX ) + ( ".map to "s )
                                            + path.string( ) + ( " failed."s ) );
        return true;
    }
    if( p_writeMapData ) {
        path += ".data";
        f = fopen( path.c_str( ), "wb" );
        if( !DATA::writeMapData( f, &info.m_bank.m_mapData[ p_mapY ][ p_mapX ] ) ) {
            message_error( "writeMapSlice", ( "Writing map data "s ) + std::to_string( p_bank )
                                                + ( "/"s ) + std::to_string( p_mapY ) + ( "_"s )
                                                + std::to_string( p_mapX ) + ( ".map.data to "s )
                                                + path.string( ) + ( " failed."s ) );
            return true;
        }
    }

    return false;
}

bool model::writeTileSets( ) {
    if( m_fsdata.m_blockSetNames.empty( ) ) { return true; }
    bool error = false;

    switch( m_fsdata.m_tileSetMode ) {
    default: {
    case DATA::TILEMODE_DEFAULT:
        // store everything in separate files
        if( !DATA::checkOrCreatePath( m_fsdata.tilesetPath( ) ) ) { return true; }
        if( !DATA::checkOrCreatePath( m_fsdata.blocksetPath( ) ) ) { return true; }
        if( !DATA::checkOrCreatePath( m_fsdata.palettePath( ) ) ) { return true; }
        for( const auto& [ name, bInfo ] : m_fsdata.m_blockSets ) {
            FILE* f;

            auto tspath = fs::path( m_fsdata.tilesetPath( ) ) / ( std::to_string( name ) + ".ts" );
            f           = fopen( tspath.c_str( ), "wb" );
            error |= !DATA::writeTiles( f, bInfo.m_tileSet.m_tiles );
            fclose( f );

            auto blpath
                = fs::path( m_fsdata.blocksetPath( ) ) / ( std::to_string( name ) + ".bvd" );
            f = fopen( blpath.c_str( ), "wb" );
            error |= !DATA::writeBlocks( f, bInfo.m_blockSet.m_blocks );
            fclose( f );

            auto palpath
                = fs::path( m_fsdata.palettePath( ) ) / ( std::to_string( name ) + ".p2l" );
            f = fopen( palpath.c_str( ), "wb" );
            error |= !DATA::writePal( f, bInfo.m_pals, 8 * DAYTIMES );
            fclose( f );
        }
        break;
    }
    case DATA::TILEMODE_COMBINED: {
        // store everything in a single file
        FILE* f;
        auto  pth = fs::path( m_fsdata.mapPath( ) ) / "tileset.tsb";
        f         = fopen( pth.c_str( ), "wb" );

        // compute header
        auto header            = DATA::blockSetBankHeader( );
        header.m_blockSetCount = *m_fsdata.m_blockSetNames.crbegin( ) + 1;
        header.m_dayTimeCount  = DAYTIMES;

        fwrite( &header, sizeof( DATA::blockSetBankHeader ), 1, f );

        for( auto i{ 0 }; i < header.m_blockSetCount; ++i ) {
            if( m_fsdata.m_blockSets.count( i ) ) {
                const auto& bInfo = m_fsdata.m_blockSets[ i ];
                error |= !DATA::writeTiles( f, bInfo.m_tileSet.m_tiles );
                error |= !DATA::writeBlocks( f, bInfo.m_blockSet.m_blocks );
                error |= !DATA::writePal( f, bInfo.m_pals, 8 * header.m_dayTimeCount );
            } else {
                // write dummy values
                // TODO: directly write dummy values?
                auto bInfo = fsdata::blockSetInfo( );
                error |= !DATA::writeTiles( f, bInfo.m_tileSet.m_tiles );
                error |= !DATA::writeBlocks( f, bInfo.m_blockSet.m_blocks );
                error |= !DATA::writePal( f, bInfo.m_pals, 8 * header.m_dayTimeCount );
            }
        }
        fclose( f );
        break;
    }
    }

    if( !error ) { markTileSetsChanged( STATUS_SAVED ); }

    return error;
}

bool model::writeMapBank( u16 p_bank ) {
    if( !existsBank( p_bank ) ) { return true; }
    auto& info = bank( p_bank );

    message_log( "writeMapBank", "Saving map bank " + std::to_string( p_bank ) + ".",
                 LOGLEVEL_STATUS );
    bool            error = false;
    std::error_code ec;

    auto path = fs::path( m_fsdata.mapPath( ) ) / fs::path( std::to_string( p_bank ) + ".bank" );
    fs::create_directories( path.parent_path( ) );
    FILE* f = fopen( path.c_str( ), "wb" );

    error = !DATA::writeMapBank( f, &info.m_info, &info.m_bank );

    if( !info.isCombined( ) ) {
        for( u8 y{ 0 }; y <= info.getSizeY( ); ++y ) {
            for( u8 x{ 0 }; x <= info.getSizeX( ); ++x ) {
                error |= writeMapSlice( p_bank, x, y );
                if( info.isScattered( ) ) {
                    // delete old map files

                    auto p = fs::path( m_fsdata.mapPath( ) ) / fs::path( std::to_string( p_bank ) )
                             / ( std::to_string( y ) + "_" + std::to_string( x ) + ".map" );
                    fs::remove( p, ec );
                    if( ec ) {
                        message_error( "writeMapBank",
                                       ( "Could not delete old map file: "s ) + ec.message( ) );
                    }
                    p += ".data";
                    fs::remove( p, ec );
                    if( ec ) {
                        message_error( "writeMapBank", ( "Could not delete old map data file: "s )
                                                           + ec.message( ) );
                    }
                }
            }
            if( !info.isScattered( ) ) {
                // delete any folder created when the map bank was scattered
                auto p = fs::path( m_fsdata.mapPath( ) ) / fs::path( std::to_string( p_bank ) )
                         / fs::path( std::to_string( y ) );
                fs::remove_all( p, ec );
                if( ec ) {
                    message_error( "writeMapBank",
                                   ( "Could not delete old map bank folder: "s ) + ec.message( ) );
                }
            }
        }
    } else {
        // clean up the directory
        auto p = fs::path( m_fsdata.mapPath( ) ) / fs::path( std::to_string( p_bank ) );
        fs::remove_all( p, ec );
        if( ec ) {
            message_error( "writeMapBank",
                           ( "Could not delete old map bank folder: "s ) + ec.message( ) );
        }
    }
    if( !error ) { markBankChanged( p_bank, STATUS_SAVED ); }

    fclose( f );
    return error;
}

bool model::writeFsRoot( ) {
    bool error = false;
    for( const auto& [ bank, info ] : m_fsdata.m_mapBanks ) {
        if( info.getStatus( ) == STATUS_NEW || info.getStatus( ) == STATUS_EDITED_UNSAVED ) {
            // something changed here, save the maps
            error |= writeMapBank( bank );
        }
    }

    if( tileStatus( ) == STATUS_EDITED_UNSAVED ) { error |= writeTileSets( ); }
    return error;
}

void model::selectMap( s16 p_mapX, s16 p_mapY ) {
    m_settings.m_selectedMapX = p_mapX;
    m_settings.m_selectedMapY = p_mapY;

    while( selectedMapY( ) > selectedSizeY( ) ) {
        bank( ).m_info.m_sizeY++;
        bank( ).m_bank.m_slices.push_back(
            std::vector<DATA::mapSlice>( selectedSizeX( ) + 1, DATA::mapSlice( ) ) );
        bank( ).m_bank.m_mapData.push_back(
            std::vector<DATA::mapData>( selectedSizeX( ) + 1, DATA::mapData( ) ) );

        bank( ).m_computedBank.push_back( std::vector<DATA::computedMapSlice>(
            selectedSizeX( ) + 1, DATA::computedMapSlice( ) ) );
        markSelectedBankChanged( );
    }
    while( selectedMapX( ) > selectedSizeX( ) ) {
        bank( ).m_info.m_sizeX++;
        for( u8 y{ 0 }; y <= selectedSizeY( ); ++y ) {
            bank( ).m_bank.m_slices[ y ].push_back( DATA::mapSlice( ) );
            bank( ).m_bank.m_mapData[ y ].push_back( DATA::mapData( ) );
            bank( ).m_computedBank[ y ].push_back( DATA::computedMapSlice( ) );
        }
        markSelectedBankChanged( );
    }
}

void model::setTileSet( u8 p_tsIdx, u8 p_newTileSet ) {
    auto& cs = slice( );

    if( p_tsIdx == 0 && cs.m_data.m_tIdx1 != p_newTileSet ) {
        cs.m_data.m_tIdx1 = p_newTileSet;
        markSelectedBankChanged( );
    }
    if( p_tsIdx == 1 && cs.m_data.m_tIdx2 != p_newTileSet ) {
        cs.m_data.m_tIdx2 = p_newTileSet;
        markSelectedBankChanged( );
    }
}
