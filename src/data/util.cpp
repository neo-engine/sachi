#include "util.h"

namespace DATA {
    mapBankInfo exploreMapBank( const fs::path& p_path ) {
        std::error_code ec;

        u8 sx = 0, sy = 0;
        u8 mapMode = 0;
        for( auto& p : fs::directory_iterator( p_path, ec ) ) {
            if( !p.is_directory( ec ) || ec ) { continue; }
            // check if the directory has a number as name
            std::string name         = p.path( ).filename( );
            int         nameAsNumber = -1;
            try {
                nameAsNumber = std::stoi( name );
            } catch( ... ) { continue; }

            if( nameAsNumber >= 0 && nameAsNumber <= int( MAX_MAPY ) ) {
                mapMode = 1;
                sy      = std::max( u8( nameAsNumber ), sy );
            }
        }
        if( mapMode == 1 ) {
            // Check all subdirs [0, sy] for max horizontal dim
            for( u16 i = 0; i < sy; ++i ) {
                try {
                    auto tmpath = p_path / std::to_string( i );
                    for( auto& p : fs::directory_iterator( tmpath ) ) {
                        if( !p.is_regular_file( ec ) || ec ) { continue; }
                        // check if the file has a name that corresponds to a map
                        u8 mx, my;
                        if( sscanf( p.path( ).filename( ).c_str( ), MAPNAME_FORMAT.c_str( ), &my,
                                    &mx )
                            == 2 ) {
                            sx = std::max( sx, mx );
                            sy = std::max( sy, my );
                        }
                    }
                } catch( ... ) { continue; }
            }

        } else if( mapMode == 0 ) {
            for( auto& p : fs::directory_iterator( p_path ) ) {
                if( !p.is_regular_file( ec ) || ec ) { continue; }
                // check if the file has a name that corresponds to a map
                u8 mx, my;
                if( sscanf( p.path( ).filename( ).c_str( ), MAPNAME_FORMAT.c_str( ), &my, &mx )
                    == 2 ) {
                    sx = std::max( sx, mx );
                    sy = std::max( sy, my );
                }
            }
        }

        return { sx, sy, mapMode };
    }

    bool checkOrCreatePath( const std::string& p_path ) {
        std::error_code ec;
        auto            p = fs::path( p_path, fs::path::generic_format );
        if( ec ) {
            fprintf( stderr, "[ERROR] Could not check if path exists: %s\n",
                     ec.message( ).c_str( ) );
            return false;
        }
        if( !fs::exists( p, ec ) ) {
            fs::create_directories( p, ec );
            fprintf( stderr, "[LOG] Creating directory %s.\n", p_path.c_str( ) );
            if( ec ) {
                fprintf( stderr, "[ERROR] Creating directory failed: %s\n",
                         ec.message( ).c_str( ) );
                return false;
            }
        }
        return true;
    }
} // namespace DATA
