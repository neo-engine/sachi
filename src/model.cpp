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

    if( res.readOrCreateFsInfo( ) ) { return res; }

    res.invalidateCaches( );

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
                if( nameAsNumber < DIVE_MAP && nameAsNumber > MAX_MAPBANK_NAME + DIVE_MAP ) {
                    message_error( "load FSROOT", std::string( "Skipping potential map bank " )
                                                      + name + ": name (as number) too large." );
                    continue;
                }
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
                if( nameAsNumber < DIVE_MAP && nameAsNumber > MAX_MAPBANK_NAME + DIVE_MAP ) {
                    message_error( "load FSROOT", std::string( "Skipping potential map bank " )
                                                      + name + ": name (as number) too large." );
                    continue;
                }
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

bool model::readOrCreateFsInfo( ) {
    // check if an fsinfo file is present

    FILE* f = fopen( m_fsdata.fsinfoPath( ).c_str( ), "rb" );
    if( !f ) {
        message_log( "load fsinfo", "fsinfo file not found; using default values." );

        // TODO: try to construct info vfile from data present in fsroot

        m_fsdata.m_fsInfo = DATA::fsdataInfo{ };
        return false;
    }

    fread( &m_fsdata.m_fsInfo, sizeof( DATA::fsdataInfo ), 1, f );

    message_log( "load fsinfo", std::to_string( m_fsdata.m_fsInfo.m_maxPkmn ) + " pkmn" );
    message_log( "load fsinfo", std::to_string( m_fsdata.m_fsInfo.m_maxItem ) + " items" );
    message_log( "load fsinfo", std::to_string( m_fsdata.m_fsInfo.m_maxMove ) + " moves" );

    fclose( f );
    return false;
}

std::string parseLogCmd( const std::string& p_cmd ) {
    u16 tmp = -1;
    if( sscanf( p_cmd.c_str( ), "%hu", &tmp ) && tmp != u16( -1 ) ) {
        switch( tmp ) {
        case 129: return "“";
        default: break;
        }
    }
    return std::string( "[" ) + p_cmd + "]";
}

std::string parseMapString( const std::string& p_text ) {
    std::string res = "";
    for( size_t i = 0; i < p_text.size( ); ++i ) {
        if( u8( p_text[ i ] ) == 0xe9 ) {
            res += "é";
            continue;
        } else if( u8( p_text[ i ] ) == '|' ) {
            res += "…";
            continue;
        } else if( u8( p_text[ i ] ) == '"' ) {
            res += "”";
            continue;
        } else if( p_text[ i ] == '[' ) {
            std::string accmd = "";
            while( p_text[ ++i ] != ']' ) { accmd += p_text[ i ]; }
            res += parseLogCmd( accmd );
            continue;
        } else if( p_text[ i ] == '\r' ) {
            res += "[A]\n";
            continue;
        }
        res += p_text[ i ];
    }
    return res;
}

std::string model::getMapString( u16 p_stringId, u8 p_language ) {
    auto  path = m_fsdata.mapStringPath( ) + "." + std::to_string( p_language ) + ".strb";
    FILE* f    = fopen( path.c_str( ), "rb" );

    char buffer[ DATA::MAPSTRING_LEN + 10 ] = { 0 };
    DATA::getString( f, DATA::MAPSTRING_LEN, p_stringId, buffer );
    return parseMapString( std::string( buffer ) );
}

bool model::writeFsInfo( ) {
    // check if an fsinfo file is present

    FILE* f = fopen( m_fsdata.fsinfoPath( ).c_str( ), "wb" );
    if( !f ) {
        message_error( "write fsinfo", "could not write fsinfo file." );

        // TODO: try to construct info vfile from data present in fsroot

        m_fsdata.m_fsInfo = DATA::fsdataInfo{ };
        return true;
    }

    fwrite( &m_fsdata.m_fsInfo, sizeof( DATA::fsdataInfo ), 1, f );
    fclose( f );
    return false;
}

bool model::setCurrentBankOWStatus( u8 p_owStatus ) {
    if( selectedBank( ) == -1 ) { return false; }

    auto& bnk = bank( );

    if( bnk.getOWStatus( ) != p_owStatus ) { markSelectedBankChanged( ); }

    // check that ow status is also properly recorded in fsdata
    if( m_fsdata.m_fsInfo.updateOWStatus( selectedBank( ), p_owStatus ) ) {
        bnk.setOWStatus( p_owStatus );
        return true;
    }
    return false;
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

bool model::readLargeMap( u16 p_bank, u8 p_mapX, u8 p_mapY, u8 p_insertX, u8 p_insertY,
                          std::string p_path ) {
    if( !existsBank( p_bank ) || p_path == "" ) { return false; }

    auto& selb = bank( p_bank );
    FILE* f    = fopen( p_path.c_str( ), "rb" );

    DATA::largeMapSliceHeader                    hd;
    std::vector<std::vector<DATA::mapBlockAtom>> sl;

    if( !DATA::readLargeMap( f, hd, sl ) ) {
        message_error( "load slice", std::string( "Loading map " ) + std::to_string( p_bank ) + "/"
                                         + std::to_string( p_mapY ) + "_" + std::to_string( p_mapX )
                                         + ".map failed. (path " + p_path.c_str( ) + ")" );
    } else {
        m_settings.m_overviewNeedsRedraw = true;
        markSelectedBankChanged( );

        // copy the read map
        auto sx{ ( DATA::SIZE * p_mapX + p_insertX + sl.size( ) - 1 ) / DATA::SIZE };
        auto sy{ ( DATA::SIZE * p_mapY + p_insertY + sl.size( ) - 1 ) / DATA::SIZE };

        while( sy > selectedSizeY( ) ) {
            bank( ).m_info.m_sizeY++;
            bank( ).m_bank.m_slices.push_back(
                std::vector<DATA::mapSlice>( selectedSizeX( ) + 1, DATA::mapSlice( ) ) );
            bank( ).m_bank.m_mapData.push_back(
                std::vector<DATA::mapData>( selectedSizeX( ) + 1, DATA::mapData( ) ) );

            bank( ).m_computedBank.push_back( std::vector<DATA::computedMapSlice>(
                selectedSizeX( ) + 1, DATA::computedMapSlice( ) ) );
        }
        while( sx > selectedSizeX( ) ) {
            bank( ).m_info.m_sizeX++;
            for( u8 y{ 0 }; y <= selectedSizeY( ); ++y ) {
                bank( ).m_bank.m_slices[ y ].push_back( DATA::mapSlice( ) );
                bank( ).m_bank.m_mapData[ y ].push_back( DATA::mapData( ) );
                bank( ).m_computedBank[ y ].push_back( DATA::computedMapSlice( ) );
            }
        }

        for( u16 y{ 0 }; y < sl.size( ); ++y ) {
            for( u16 x{ 0 }; x < sl[ y ].size( ); ++x ) {
                auto px{ x + DATA::SIZE * p_mapX + p_insertX },
                    py{ y + DATA::SIZE * p_mapY + p_insertY };

                auto bx{ px / DATA::SIZE }, by{ py / DATA::SIZE }, mx{ px % DATA::SIZE },
                    my{ py % DATA::SIZE };

                selb.m_bank.m_slices[ by ][ bx ].m_data.m_blocks[ my ][ mx ] = sl[ y ][ x ];
                selb.m_bank.m_slices[ by ][ bx ].m_data.m_tIdx1              = hd.m_tIdx1;
                selb.m_bank.m_slices[ by ][ bx ].m_data.m_tIdx2              = hd.m_tIdx2;
            }
        }
    }

    return true;
}

bool model::checkOrLoadBank( int p_bank, bool p_forceRead ) {
    if( p_bank > DIVE_MAP && p_bank <= DIVE_MAP + MAX_MAPBANK_NAME ) {
        if( !existsBank( p_bank % DIVE_MAP ) ) { return false; }

        if( !existsBank( p_bank ) ) {
            // dive map does not exist yet, copy surface level
            bank( p_bank )          = bank( p_bank % DIVE_MAP );
            p_forceRead             = false;
            bank( p_bank ).m_loaded = true;
            markBankChanged( p_bank );
        }
    } else if( p_bank < 0 || p_bank > MAX_MAPBANK_NAME ) {
        return false;
    }

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

        if( selb.m_info.m_isOWMap ) {
            // write location file
            auto owpath = fs::path( m_fsdata.mapLocationPath( ) )
                          / fs::path( std::to_string( p_bank ) + ".loc.data" );
            fs::create_directories( owpath.parent_path( ) );
            FILE* owf = fopen( owpath.c_str( ), "rb" );

            if( owf ) {
                u8 meta[ 5 ] = { };
                fread( meta, sizeof( u8 ), 5, owf );
                selb.m_mapImageRes    = meta[ 2 ];
                selb.m_mapImageShiftX = meta[ 3 ];
                selb.m_mapImageShiftY = meta[ 4 ];
                fclose( owf );
            }

            // write ow map picture
            selb.m_owMap = DATA::bitmap::fromBGImage(
                ( m_fsdata.owMapPicturePath( ) + std::to_string( p_bank ) + ".raw" ).c_str( ) );
        }

        selb.m_loaded = true;
    }

    return true;
}

bool model::readTileSets( ) {
    // first check if a combined file exists
    bool error{ false };

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
            if( !DATA::readPal( f, res.m_pals, 8 * DAYTIMES ) ) {
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
    u8 ts1{ 0 };
    u8 ts2{ 0 };
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
    u8 ts1{ 0 };
    u8 ts2{ 0 };
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

void model::buildPalette( DATA::palette p_out[ DAYTIMES * 16 ], s8 p_ts1, s8 p_ts2 ) {
    u8 ts1{ 0 };
    u8 ts2{ 0 };
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
    for( u8 dt{ 0 }; dt < DAYTIMES; ++dt ) {
        std::memcpy( &p_out[ 16 * dt ], &m_fsdata.m_blockSets[ ts1 ].m_pals[ 8 * dt ],
                     sizeof( DATA::palette ) * 6 );
        std::memcpy( &p_out[ 16 * dt + 6 ], &m_fsdata.m_blockSets[ ts2 ].m_pals[ 8 * dt ],
                     sizeof( DATA::palette ) * 8 );
        std::memcpy( &p_out[ 16 * dt + 14 ], &m_fsdata.m_blockSets[ ts1 ].m_pals[ 8 * dt + 6 ],
                     sizeof( DATA::palette ) * 2 );
    }
}

u16 tintColor( u16 p_color, u16 p_tint, u8 p_tintFactor = 20 ) {
    return ( p_color & ( 1 << 15 ) )
           | ( ( p_color & 31 )
               + ( ( p_tint & 31 ) - ( p_color & 31 ) ) * 10 / ( p_tintFactor - 3 ) )
           | ( ( ( ( p_color >> 5 ) & 31 )
                 + ( ( ( p_tint >> 5 ) & 31 ) - ( ( p_color >> 5 ) & 31 ) ) * 10
                       / ( p_tintFactor + 2 ) )
               << 5 )
           | ( ( ( ( p_color >> 10 ) & 31 )
                 + ( ( ( p_tint >> 10 ) & 31 ) - ( ( p_color >> 10 ) & 31 ) ) * 10 / p_tintFactor )
               << 10 );
}

u16 swapColor( u16 p_color ) {
    auto mx
        = std::max( ( p_color & 31 ), std::max( ( p_color >> 5 ) & 31, ( p_color >> 10 ) & 31 ) );
    auto mn
        = std::min( ( p_color & 31 ), std::min( ( p_color >> 5 ) & 31, ( p_color >> 10 ) & 31 ) );

    return ( p_color & ( 1 << 15 ) ) | ( mn << 10 ) | mx | ( mn << 5 );
}

constexpr u16 tints[ 5 ] = {
    0x0000, // day (no tint)
    0x3672, // dusk
    0x24a4, // evening
    0x1c44, // night
    0x3f1c, // morning
};
constexpr u16 tint_fac[ 5 ] = { 400, 25, 35, 18, 30 };

void model::recomputeDNS( u8 p_tsIdx, bool p_override ) {
    if( !m_fsdata.m_blockSets.count( p_tsIdx ) ) { return; }

    auto& bs = m_fsdata.m_blockSets[ p_tsIdx ];
    for( u8 i{ 0 }; i < 8; ++i ) {
        for( u8 j{ 0 }; j < 16; ++j ) {
            u16 color = bs.m_pals[ i ].m_pal[ j ];
            for( u8 k{ 1 }; k < 5; ++k ) {
                if( bs.m_pals[ i + 8 * k ].m_pal[ j ] && !p_override ) {
                    // don't overwrite existing pals
                    continue;
                }
                bs.m_pals[ i + 8 * k ].m_pal[ j ]
                    = ( k == 1 )
                          ? tintColor( swapColor( color ), tints[ k ], tint_fac[ k ] )
                          : tintColor( color, tints[ k ],
                                       ( j <= 3 ? 5 : 0 ) + ( j == 15 ? -5 : 0 ) + tint_fac[ k ] );
            }
        }
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

bool model::readTileSet( u16 p_bsId, std::string p_path ) {
    auto& res = m_fsdata.m_blockSets[ p_bsId ];
    auto  f   = fopen( p_path.c_str( ), "r" );
    if( !DATA::readTiles( f, res.m_tileSet.m_tiles ) ) {
        message_error( "readTileSets", ( "Reading tile set "s ) + p_path + " failed." );
        return true;
    }
    fclose( f );
    return false;
}

bool model::writeTileSet( u16 p_bsId, std::string p_path ) {
    if( !m_fsdata.m_blockSets.count( p_bsId ) ) { return true; }

    bool error{ false };
    auto f = fopen( p_path.c_str( ), "wb" );
    error |= !DATA::writeTiles( f, m_fsdata.m_blockSets[ p_bsId ].m_tileSet.m_tiles );
    fclose( f );

    return error;
}

bool model::readBlockSet( u16 p_bsId, std::string p_path ) {
    auto& res = m_fsdata.m_blockSets[ p_bsId ];
    auto  f   = fopen( p_path.c_str( ), "r" );
    if( !DATA::readBlocks( f, res.m_blockSet.m_blocks ) ) {
        message_error( "readTileSets", ( "Reading block set "s ) + p_path + " failed." );
        return true;
    }
    fclose( f );
    return false;
}

bool model::writeBlockSet( u16 p_bsId, std::string p_path ) {
    if( !m_fsdata.m_blockSets.count( p_bsId ) ) { return true; }

    bool error{ false };
    auto f = fopen( p_path.c_str( ), "wb" );
    error |= !DATA::writeBlocks( f, m_fsdata.m_blockSets[ p_bsId ].m_blockSet.m_blocks );
    fclose( f );

    return error;
}

bool model::writeTileSets( ) {
    if( m_fsdata.m_blockSetNames.empty( ) ) { return true; }
    bool error{ false };

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
    bool            error{ false };
    std::error_code ec;

    auto path = fs::path( m_fsdata.mapPath( ) ) / fs::path( std::to_string( p_bank ) + ".bank" );
    fs::create_directories( path.parent_path( ) );
    FILE* f = fopen( path.c_str( ), "wb" );

    error = !DATA::writeMapBank( f, &info.m_info, &info.m_bank );

    if( info.m_info.m_isOWMap ) {
        // write location file
        auto owpath = fs::path( m_fsdata.mapLocationPath( ) )
                      / fs::path( std::to_string( p_bank ) + ".loc.data" );
        fs::create_directories( owpath.parent_path( ) );
        FILE* owf = fopen( owpath.c_str( ), "wb" );

        size_t scale = DATA::SIZE / DATA::MAP_LOCATION_RES;

        u8 meta[] = { u8( info.getSizeX( ) * scale ), u8( info.getSizeY( ) * scale ),
                      u8( info.m_mapImageRes ), u8( info.m_mapImageShiftX ),
                      u8( info.m_mapImageShiftY ) };

        u16* tmploc = new u16[ meta[ 0 ] * meta[ 1 ] ];

        // construct location data

        for( u8 y{ 0 }; y < meta[ 1 ]; ++y ) {
            for( u8 x{ 0 }; x < meta[ 0 ]; ++x ) {
                size_t pos = y * meta[ 0 ] + x;

                tmploc[ pos ] = info.m_bank.m_mapData[ y / scale ][ x / scale ]
                                    .m_locationIds[ y % scale ][ x % scale ];
            }
        }

        fwrite( &meta, sizeof( u8 ), 5, owf );
        fwrite( &info.m_info.m_defaultLocation, sizeof( u16 ), 1, owf );
        fwrite( tmploc, sizeof( u16 ), meta[ 0 ] * meta[ 1 ], owf );

        delete[] tmploc;
        fclose( owf );

        // write ow map picture
        info.m_owMap.dumpToFile(
            ( m_fsdata.owMapPicturePath( ) + std::to_string( p_bank ) + ".raw" ).c_str( ), 191, 1 );
    }

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
    bool error{ false };
    for( const auto& [ bank, info ] : m_fsdata.m_mapBanks ) {
        if( info.getStatus( ) == STATUS_NEW || info.getStatus( ) == STATUS_EDITED_UNSAVED ) {
            // something changed here, save the maps
            error |= writeMapBank( bank );
        }
    }

    if( tileStatus( ) == STATUS_EDITED_UNSAVED ) { error |= writeTileSets( ); }
    error |= writeFsInfo( );
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
        m_settings.m_overviewNeedsRedraw = true;
        markSelectedBankChanged( );
    }
    while( selectedMapX( ) > selectedSizeX( ) ) {
        bank( ).m_info.m_sizeX++;
        for( u8 y{ 0 }; y <= selectedSizeY( ); ++y ) {
            bank( ).m_bank.m_slices[ y ].push_back( DATA::mapSlice( ) );
            bank( ).m_bank.m_mapData[ y ].push_back( DATA::mapData( ) );
            bank( ).m_computedBank[ y ].push_back( DATA::computedMapSlice( ) );
        }
        m_settings.m_overviewNeedsRedraw = true;
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

const DATA::mapData::wildPkmnData& model::encounterData( u8 p_encSlot ) const {
    return mapData( ).m_pokemon[ p_encSlot ];
}

DATA::mapData::wildPkmnData& model::encounterData( u8 p_encSlot ) {
    return mapData( ).m_pokemon[ p_encSlot ];
}

const model::stringCache& model::pkmnNames( ) {
    if( m_pkmnNameCache.m_valid ) { return m_pkmnNameCache; }

    m_pkmnNameCache.m_lastRefresh++;
    m_pkmnNameCache.m_strings.clear( );

    // open pkmn name file
    auto  path = m_fsdata.pkmnNamePath( ) + ".0.strb";
    FILE* f    = fopen( path.c_str( ), "rb" );

    if( !f ) {
        m_pkmnNameCache.m_valid = false;
        return m_pkmnNameCache;
    }

    char tmp[ PKMN_NAMELENGTH + 10 ];
    for( u16 i{ 0 }; i <= maxPkmn( ); ++i ) {
        memset( tmp, 0, sizeof( tmp ) );
        fread( tmp, PKMN_NAMELENGTH, 1, f );
        m_pkmnNameCache.m_strings.push_back( parseMapString( std::string{ tmp } ) );
    }

    m_pkmnNameCache.m_valid = true;
    fclose( f );
    return m_pkmnNameCache;
}

const model::stringCache& model::locationNames( ) {
    if( m_locationNameCache.m_valid ) { return m_locationNameCache; }

    m_locationNameCache.m_lastRefresh++;
    m_locationNameCache.m_strings.clear( );

    // open pkmn name file
    auto  path = m_fsdata.locationNamePath( ) + ".0.strb";
    FILE* f    = fopen( path.c_str( ), "rb" );

    if( !f ) {
        m_locationNameCache.m_valid = false;
        return m_locationNameCache;
    }

    char tmp[ LOCATION_NAMELENGTH + 10 ];
    for( u16 i{ 0 };; ++i ) {
        memset( tmp, 0, sizeof( tmp ) );
        if( !fread( tmp, LOCATION_NAMELENGTH, 1, f ) ) { break; }
        m_locationNameCache.m_strings.push_back( parseMapString( std::string{ tmp } ) );
    }

    m_locationNameCache.m_valid = true;
    fclose( f );
    return m_locationNameCache;
}

const model::stringCache& model::itemNames( ) {
    if( m_itemNameCache.m_valid ) { return m_itemNameCache; }

    m_itemNameCache.m_lastRefresh++;
    m_itemNameCache.m_strings.clear( );

    // open pkmn name file
    auto  path = m_fsdata.itemNamePath( ) + ".0.strb";
    FILE* f    = fopen( path.c_str( ), "rb" );

    if( !f ) {
        m_itemNameCache.m_valid = false;
        return m_itemNameCache;
    }

    char tmp[ ITEM_NAMELENGTH + 10 ];
    for( u16 i{ 0 };; ++i ) {
        memset( tmp, 0, sizeof( tmp ) );
        if( !fread( tmp, ITEM_NAMELENGTH, 1, f ) ) { break; }
        m_itemNameCache.m_strings.push_back( parseMapString( std::string{ tmp } ) );
    }

    m_itemNameCache.m_valid = true;
    fclose( f );
    return m_itemNameCache;
}

const model::stringCache& model::moveNames( ) {
    if( m_moveNameCache.m_valid ) { return m_moveNameCache; }

    m_moveNameCache.m_lastRefresh++;
    m_moveNameCache.m_strings.clear( );

    // open pkmn name file
    auto  path = m_fsdata.moveNamePath( ) + ".0.strb";
    FILE* f    = fopen( path.c_str( ), "rb" );

    if( !f ) {
        m_moveNameCache.m_valid = false;
        return m_moveNameCache;
    }

    char tmp[ MOVE_NAMELENGTH + 10 ];
    for( u16 i{ 0 };; ++i ) {
        memset( tmp, 0, sizeof( tmp ) );
        if( !fread( tmp, MOVE_NAMELENGTH, 1, f ) ) { break; }
        m_moveNameCache.m_strings.push_back( parseMapString( std::string{ tmp } ) );
    }

    m_moveNameCache.m_valid = true;
    fclose( f );
    return m_moveNameCache;
}

const model::stringCache& model::abilityNames( ) {
    if( m_abilityNameCache.m_valid ) { return m_abilityNameCache; }

    m_abilityNameCache.m_lastRefresh++;
    m_abilityNameCache.m_strings.clear( );

    // open pkmn name file
    auto  path = m_fsdata.abilityNamePath( ) + ".0.strb";
    FILE* f    = fopen( path.c_str( ), "rb" );

    if( !f ) {
        m_abilityNameCache.m_valid = false;
        return m_abilityNameCache;
    }

    char tmp[ ABILITY_NAMELENGTH + 10 ];
    for( u16 i{ 0 };; ++i ) {
        memset( tmp, 0, sizeof( tmp ) );
        if( !fread( tmp, ABILITY_NAMELENGTH, 1, f ) ) { break; }
        m_abilityNameCache.m_strings.push_back( parseMapString( std::string{ tmp } ) );
    }

    m_abilityNameCache.m_valid = true;
    fclose( f );
    return m_abilityNameCache;
}

void model::recomputeBankPic( ) {
    auto& bnk   = bank( );
    auto  scale = DATA::SIZE * DATA::BLOCK_SIZE / ( 2 * bnk.m_mapImageRes );

    auto btm = DATA::bitmap{ 256, 192 };

    // render each map slice of the current bank
    for( u8 y{ 0 }; y <= bnk.getSizeY( ); ++y ) {
        for( u8 x{ 0 }; x <= bnk.getSizeX( ); ++x ) {
            auto tmp = DATA::bitmap{ DATA::SIZE * DATA::BLOCK_SIZE, DATA::SIZE * DATA::BLOCK_SIZE };
            DATA::renderMapSlice( &computedSlice( selectedBank( ), y, x ), &tmp, 0, 0, 1, 0 );

            // scale down

            for( u8 by{ 0 }; by < DATA::SIZE * DATA::BLOCK_SIZE / scale; ++by ) {
                for( u8 bx{ 0 }; bx < DATA::SIZE * DATA::BLOCK_SIZE / scale; ++bx ) {
                    DATA::pixel res = tmp( bx * scale, by * scale );
                    for( u8 y2{ 0 }; y2 < scale; ++y2 ) {
                        for( u8 x2{ 0 }; x2 < scale; ++x2 ) {
                            res ^= tmp( bx * scale + x2, by * scale + y2 );
                        }
                    }
                    res.round( 16 );
                    btm( bnk.m_mapImageShiftX + x * 2 * bnk.m_mapImageRes + bx,
                         bnk.m_mapImageShiftY + y * 2 * bnk.m_mapImageRes + by )
                        = res;
                }
            }
        }
    }

    auto bgcol = btm( bnk.m_mapImageShiftX, bnk.m_mapImageShiftY );

    for( u8 y{ 0 }; y < 192; ++y ) {
        for( u16 x{ 0 }; x < 256; ++x ) {
            if( btm( x, y ) == DATA::pixel{ 0, 0, 0 } ) { btm( x, y ) = bgcol; }
        }
    }

    bnk.m_owMap = std::move( btm );
}

auto routecol    = DATA::pixel( 230, 200, 30, 100 );
auto searoutecol = DATA::pixel( 7, 211, 255, 100 );
auto citycol     = DATA::pixel( 255, 68, 119, 100 );
auto specialcol  = DATA::pixel( 0, 255, 0, 100 );
auto bordercol   = DATA::pixel( 255, 255, 255, 20 );

DATA::pixel model::colorForLocation( u16 p_loc ) {

    /*
        if( !strcmp( p_str, "none" ) ) return 0;
    if( !strcmp( p_str, "route" ) ) return 0;
    if( !strcmp( p_str, "coastroute" ) ) return 11;

    // water
    if( !strcmp( p_str, "water" ) ) return 2;
    if( !strcmp( p_str, "beach" ) ) return 3;

    // special
    if( !strcmp( p_str, "cave" ) ) return 4;
    if( !strcmp( p_str, "forest" ) ) return 5;
    if( !strcmp( p_str, "special" ) ) return 10;
    if( !strcmp( p_str, "coast" ) ) return 7;
    if( !strcmp( p_str, "inside" ) ) return 1;

    // city
    if( !strcmp( p_str, "forestcity" ) ) return 12;
    if( !strcmp( p_str, "city" ) ) return 6;
    if( !strcmp( p_str, "town" ) ) return 8;
    if( !strcmp( p_str, "field" ) ) return 9;
*/

    switch( DATA::frameForLocation( m_fsdata.locationDataPath( ).c_str( ), p_loc ) ) {
    case 2:
    case 3: return searoutecol;
    case 4:
    case 5:
    case 10:
    case 7:
    case 1: return specialcol;
    case 12:
    case 6:
    case 8:
    case 9: return citycol;
    default: return routecol;
    }
}

void model::recomputeBankLocationOverlay( ) {
    auto& bnk   = bank( );
    auto  scale = DATA::SIZE * DATA::BLOCK_SIZE / ( 2 * bnk.m_mapImageRes );

    auto divs = DATA::SIZE / DATA::MAP_LOCATION_RES;

#define locs( y, x )                                     \
    bnk.m_bank.m_mapData[ ( y ) / divs ][ ( x ) / divs ] \
        .m_locationIds[ ( y ) % divs ][ ( x ) % divs ]

    for( u16 y = 0; y < divs * bnk.getSizeY( ); ++y ) {
        for( u16 x = 0; x < divs * bnk.getSizeX( ); ++x ) {
            if( !locs( y, x ) ) { continue; }
            u8 border = 0;

            if( !y || locs( y, x ) != locs( y - 1, x ) ) { border |= 1; }
            if( !x || locs( y, x ) != locs( y, x - 1 ) ) { border |= 2; }
            if( y + 1 >= divs * bnk.getSizeY( ) || locs( y, x ) != locs( y + 1, x ) ) {
                border |= 4;
            }
            if( x + 1 >= divs * bnk.getSizeX( ) || locs( y, x ) != locs( y, x + 1 ) ) {
                border |= 8;
            }

            u8 borsize = std::max( 1, DATA::BLOCK_SIZE / scale );
            if( borsize == 1 && std::popcount( border ) >= 3
                && colorForLocation( locs( y, x ) ) != routecol ) {
                borsize = 0;
            }
            if( borsize == 1 && ( border == 0b101 || border == 0b1010 )
                && colorForLocation( locs( y, x ) ) == specialcol ) {
                borsize = 0;
            }

            DATA::tintRectangle(
                bnk.m_owMap,
                bnk.m_mapImageShiftX + x * ( DATA::SIZE / divs ) * DATA::BLOCK_SIZE / scale,
                bnk.m_mapImageShiftY + y * ( DATA::SIZE / divs ) * DATA::BLOCK_SIZE / scale,
                bnk.m_mapImageShiftX + ( x + 1 ) * ( DATA::SIZE / divs ) * DATA::BLOCK_SIZE / scale,
                bnk.m_mapImageShiftY + ( y + 1 ) * ( DATA::SIZE / divs ) * DATA::BLOCK_SIZE / scale,
                colorForLocation( locs( y, x ) ), borsize, bordercol, border );
        }
    }
#undef locs

    for( u8 y{ 0 }; y < 192; ++y ) {
        for( u16 x{ 0 }; x < 256; ++x ) { bnk.m_owMap( x, y ).round( 32 ); }
    }
}
