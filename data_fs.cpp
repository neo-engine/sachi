#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>

#include "data_fs.h"
#include "data_maprender.h"

namespace DATA {
    char TMP_BUFFER[ 100 ];
    char TMP_BUFFER_SHORT[ 50 ];

    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fread( p_buffer, p_size, p_count, p_stream );
    }
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fwrite( p_buffer, p_size, p_count, p_stream );
    }

    bool readNop( FILE* p_file, u32 p_cnt ) {
        u8 tmp;
        for( u32 i = 0; i < p_cnt; ++i ) {
            if( !read( p_file, &tmp, sizeof( u8 ), 1 ) ) return false;
        }
        return true;
    }
    bool writeNop( FILE* p_file, u32 p_cnt ) {
        u8 tmp = 0;
        for( u32 i = 0; i < p_cnt; ++i ) {
            if( !write( p_file, &tmp, sizeof( u8 ), 1 ) ) return false;
        }
        return true;
    }

    FILE* open( const char* p_path, const char* p_name, const char* p_ext, const char* p_mode ) {
        snprintf( TMP_BUFFER, 99, "%s%s%s", p_path, p_name, p_ext );
        return fopen( TMP_BUFFER, p_mode );
    }
    FILE* open( const char* p_path, u16 p_value, const char* p_ext, const char* p_mode ) {
        snprintf( TMP_BUFFER, 99, "%s%d%s", p_path, p_value, p_ext );
        return fopen( TMP_BUFFER, p_mode );
    }

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

    bool readMapSlice( FILE* p_mapFile, mapSlice* p_result, u16 p_x, u16 p_y, bool p_close ) {
        if( p_mapFile == 0 ) return false;

        p_result->m_x = p_x;
        p_result->m_y = p_y;

        //    printf( "constructing slice\n" );

        readNop( p_mapFile, 8 ); // map size ignored

        u8 tsidx1, tsidx2;

        read( p_mapFile, &tsidx1, sizeof( u8 ), 1 );
        readNop( p_mapFile, 3 );
        read( p_mapFile, &tsidx2, sizeof( u8 ), 1 );
        readNop( p_mapFile, 3 );

        // printf( "red tsidx %hhu %hhu\n", tsidx1, tsidx2 );

        // border size
        u8 b1, b2;
        //    readNop( p_mapFile, 4 );
        read( p_mapFile, &b1, sizeof( u8 ), 1 );
        read( p_mapFile, &b2, sizeof( u8 ), 1 );
        readNop( p_mapFile, 2 );

        if( b1 && b2 ) {
            read( p_mapFile, p_result->m_blocks, sizeof( mapBlockAtom ), b1 * b2 ); // Border blocks
        }

        read( p_mapFile, p_result->m_blocks, sizeof( mapBlockAtom ), SIZE * SIZE );
        if( p_close ) { fclose( p_mapFile ); }

        p_result->m_tIdx1 = tsidx1;
        p_result->m_tIdx2 = tsidx2;

        return true;
    }

    bool writeMapSlice( FILE* p_mapFile, const mapSlice* p_map, bool p_close ) {
        if( p_mapFile == 0 ) return false;
        write( p_mapFile, &SIZE, sizeof( u8 ), 1 );
        writeNop( p_mapFile, 3 );
        write( p_mapFile, &SIZE, sizeof( u8 ), 1 );
        writeNop( p_mapFile, 3 );

        write( p_mapFile, &p_map->m_tIdx1, sizeof( u8 ), 1 );
        writeNop( p_mapFile, 3 );
        write( p_mapFile, &p_map->m_tIdx2, sizeof( u8 ), 1 );
        writeNop( p_mapFile, 3 );

        // border size
        writeNop( p_mapFile, 4 );

        write( p_mapFile, p_map->m_blocks, sizeof( mapBlockAtom ), SIZE * SIZE );
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
        if( !p_file ) { return false; }
        fwrite( p_data, sizeof( mapData ), 1, p_file );
        if( p_close ) { fclose( p_file ); }
        return true;
    }

    bool readMapSliceAndData( FILE* p_mapFile, mapSlice* p_slice, mapData* p_data, u16 p_x,
                              u16 p_y ) {
        if( p_mapFile == 0 ) return false;

        mapBankInfo info;
        if( fseek( p_mapFile, 0, SEEK_SET ) ) { return false; }
        fread( &info, sizeof( mapBankInfo ), 1, p_mapFile );

        if( fseek( p_mapFile,
                   sizeof( mapBankInfo )
                       + ( ( info.m_sizeX + 1 ) * p_y + p_x )
                             * ( sizeof( mapSlice ) + sizeof( mapData ) ),
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
                             * ( sizeof( mapSlice ) + sizeof( mapData ) ),
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
        fwrite( p_info, sizeof( mapBankInfo ), 1, p_mapFile );

        if( p_info->m_mapMode != MAPMODE_COMBINED ) { return true; }

        for( u8 y = 0; y <= p_info->m_sizeY; ++y ) {
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

    size_t getLength( u8 p_c ) {
        if( p_c < 0x80 )
            return 1;
        else if( !( p_c & 0x20 ) )
            return 2;
        else if( !( p_c & 0x10 ) )
            return 3;
        else if( !( p_c & 0x08 ) )
            return 4;
        else if( !( p_c & 0x04 ) )
            return 5;
        else
            return 6;
    }

    char getValue( char* p_text, size_t* p_readIdx ) {
        size_t len = getLength( static_cast<u8>( p_text[ *p_readIdx ] ) );

        if( len == 1 ) {
            char c = p_text[ *p_readIdx ];
            ( *p_readIdx )++;
            return c;
        }

        unsigned int v = ( p_text[ *p_readIdx ] & ( 0xff >> ( len + 1 ) ) ) << ( ( len - 1 ) * 6 );
        ( *p_readIdx )++;
        for( len--; len > 0; len-- ) {
            v |= ( static_cast<u8>( p_text[ *p_readIdx ] ) - 0x80 ) << ( ( len - 1 ) * 6 );
            ( *p_readIdx )++;
        }
        return ( v > 0xff ) ? 0 : (char) v;
    }

    char* fixEncoding( char* p_utf8 ) {
        for( size_t readIndex = 0, writeIndex = 0;; writeIndex++ ) {
            if( p_utf8[ readIndex ] == 0 ) {
                p_utf8[ writeIndex ] = 0;
                break;
            }

            char c = getValue( p_utf8, &readIndex );
            if( c == 0 ) {
                fprintf( stderr, "[%s] Invalid character '%c'\n", p_utf8, p_utf8[ readIndex ] );
                c = '_';
            }
            p_utf8[ writeIndex ] = c;
        }

        return p_utf8;
    }

    u8 getType( char* p_str ) {
        if( !strcmp( p_str, "Normal" ) ) return 0;
        if( !strcmp( p_str, "Fight" ) ) return 1;
        if( !strcmp( p_str, "Fighting" ) ) return 1;
        if( !strcmp( p_str, "Flying" ) ) return 2;
        if( !strcmp( p_str, "Poison" ) ) return 3;
        if( !strcmp( p_str, "Ground" ) ) return 4;
        if( !strcmp( p_str, "Rock" ) ) return 5;
        if( !strcmp( p_str, "Bug" ) ) return 6;
        if( !strcmp( p_str, "Ghost" ) ) return 7;
        if( !strcmp( p_str, "Steel" ) ) return 8;
        if( !strcmp( p_str, "???" ) ) return 9;
        if( !strcmp( p_str, "Water" ) ) return 10;
        if( !strcmp( p_str, "Fire" ) ) return 11;
        if( !strcmp( p_str, "Grass" ) ) return 12;
        if( !strcmp( p_str, "Electric" ) ) return 13;
        if( !strcmp( p_str, "Electr" ) ) return 13;
        if( !strcmp( p_str, "Psychic" ) ) return 14;
        if( !strcmp( p_str, "Ice" ) ) return 15;
        if( !strcmp( p_str, "Dragon" ) ) return 16;
        if( !strcmp( p_str, "Dark" ) ) return 17;
        if( !strcmp( p_str, "Fairy" ) ) return 18;

        fprintf( stderr, "Found bad type %s.\n", p_str );
        return 9;
    }

    u8 getContestType( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "Tough" ) ) return 1;
        if( !strcmp( p_str, "Clever" ) ) return 2;
        if( !strcmp( p_str, "Beautiful" ) ) return 3;
        if( !strcmp( p_str, "Cool" ) ) return 4;
        if( !strcmp( p_str, "Cute" ) ) return 5;

        fprintf( stderr, "Found bad contest type %s.\n", p_str );
        return 0;
    }

    u8 getLevelUpType( char* p_str ) {
        if( !strcmp( p_str, "Medium Fast" ) ) return 0;
        if( !strcmp( p_str, "Erratic" ) ) return 1;
        if( !strcmp( p_str, "Fluctuating" ) ) return 2;
        if( !strcmp( p_str, "Medium Slow" ) ) return 3;
        if( !strcmp( p_str, "Fast" ) ) return 4;
        if( !strcmp( p_str, "Slow" ) ) return 5;

        fprintf( stderr, "Found bad level up type %s.\n", p_str );
        return 0;
    }

    u8 getEggGroup( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "Monster" ) ) return 1;
        if( !strcmp( p_str, "Water 1" ) ) return 2;

        if( !strcmp( p_str, "Bug" ) ) return 4;
        if( !strcmp( p_str, "Flying" ) ) return 5;
        if( !strcmp( p_str, "Field" ) ) return 6;
        if( !strcmp( p_str, "Fairy" ) ) return 7;
        if( !strcmp( p_str, "Grass" ) ) return 8;
        if( !strcmp( p_str, "Human-Like" ) ) return 9;
        if( !strcmp( p_str, "Water 3" ) ) return 10;
        if( !strcmp( p_str, "Mineral" ) ) return 11;
        if( !strcmp( p_str, "Amorphous" ) ) return 12;
        if( !strcmp( p_str, "Water 2" ) ) return 13;
        if( !strcmp( p_str, "Ditto" ) ) return 14;
        if( !strcmp( p_str, "Dragon" ) ) return 15;
        if( !strcmp( p_str, "Undiscovered" ) ) return 16;

        fprintf( stderr, "Found bad egg group %s.\n", p_str );
        return 0;
    }

    u8 getExpType( char* p_str ) {
        if( !strcmp( p_str, "Medium Fast" ) ) return 0;
        if( !strcmp( p_str, "Erratic" ) ) return 1;
        if( !strcmp( p_str, "Fluctuating" ) ) return 2;
        if( !strcmp( p_str, "Medium Slow" ) ) return 3;
        if( !strcmp( p_str, "Fast" ) ) return 4;
        if( !strcmp( p_str, "Slow" ) ) return 5;

        fprintf( stderr, "Found bad exp type %s.\n", p_str );
        return 0;
    }

    u8 getGender( char* p_str ) {
        if( !strcmp( p_str, "genderless" ) ) return 255;
        if( !strcmp( p_str, "always female" ) ) return 254;
        if( !strcmp( p_str, "1m7f" ) ) return 223;
        if( !strcmp( p_str, "1m3f" ) ) return 191;
        if( !strcmp( p_str, "1m1f" ) ) return 127;
        if( !strcmp( p_str, "3m1f" ) ) return 63;
        if( !strcmp( p_str, "7m1f" ) ) return 31;
        if( !strcmp( p_str, "always male" ) ) return 0;

        fprintf( stderr, "Found bad gender value %s.\n", p_str );
        return 255;
    }

    u8 getColor( char* p_str ) {
        if( !strcmp( p_str, "Red" ) ) return 0;
        if( !strcmp( p_str, "Blue" ) ) return 1;
        if( !strcmp( p_str, "Yellow" ) ) return 2;
        if( !strcmp( p_str, "Green" ) ) return 3;
        if( !strcmp( p_str, "Black" ) ) return 4;
        if( !strcmp( p_str, "Brown" ) ) return 5;
        if( !strcmp( p_str, "Purple" ) ) return 6;
        if( !strcmp( p_str, "Gray" ) ) return 7;
        if( !strcmp( p_str, "White" ) ) return 8;
        if( !strcmp( p_str, "Pink" ) ) return 9;

        fprintf( stderr, "Found bad color %s.\n", p_str );
        return 0;
    }

    u8 getShape( char* p_str ) {
        if( !strcmp( p_str, "Pomaceous" ) ) return 1;
        if( !strcmp( p_str, "Caudal" ) ) return 2;
        if( !strcmp( p_str, "Ichthyic" ) ) return 3;
        if( !strcmp( p_str, "Brachial" ) ) return 4;
        if( !strcmp( p_str, "Alvine" ) ) return 5;
        if( !strcmp( p_str, "Sciurine" ) ) return 6;
        if( !strcmp( p_str, "Crural" ) ) return 7;
        if( !strcmp( p_str, "Mensal" ) ) return 8;
        if( !strcmp( p_str, "Alar" ) ) return 9;
        if( !strcmp( p_str, "Cilial" ) ) return 10;
        if( !strcmp( p_str, "Polycephalic" ) ) return 11;
        if( !strcmp( p_str, "Anthropomorphic" ) ) return 12;
        if( !strcmp( p_str, "Lepidopterous" ) ) return 13;
        if( !strcmp( p_str, "Chitinous" ) ) return 14;

        fprintf( stderr, "Found bad shape %s.\n", p_str );
        return 0;
    }

    u8 getItemType( char* p_str ) {
        if( !strcmp( p_str, "used" ) ) return 0;
        if( !strcmp( p_str, "?" ) ) return 0;
        if( !strcmp( p_str, "pokeball" ) ) return 1; // Pokeball
        if( !strcmp( p_str, "medicine" ) ) return 2; // Item used as medicine
        if( !strcmp( p_str, "medicine-hold" ) )
            return 34; // Item used as medicine with effect when held
        if( !strcmp( p_str, "battleItem" ) ) return 3;   // Item with effect in battle
        if( !strcmp( p_str, "collectibles" ) ) return 4; // Item with no special effect
        if( !strcmp( p_str, "usables" ) ) return 5;   // Consumable item that can be used overworld
        if( !strcmp( p_str, "evolution" ) ) return 6; // Item that evolves nome Pkmn
        if( !strcmp( p_str, "hold" ) ) return 32;     // Item that only has an effect when held
        if( !strcmp( p_str, "formeChange" ) ) return 8; // Item used to change the forme of a pkmn
        if( !strcmp( p_str, "keyItems" ) ) return 9;    // Non-Consumable item with special effects
        if( !strcmp( p_str, "TM/HM" ) ) return 12;      // TM/HM/TR
        if( !strcmp( p_str, "Apricorn" ) ) return 13;   // Apricorn
        //    if( !strcmp( p_str, "Z-multi" ) ) return 14;    // Multi-use Z item (unused)
        //    if( !strcmp( p_str, "Z-held" ) ) return 15;     // Held Z item (unused)

        if( !strcmp( p_str, "berry" ) ) return 20;               // Berry
        if( !strcmp( p_str, "berry-medicine" ) ) return 18;      // Berry + Medicine
        if( !strcmp( p_str, "berry-hold" ) ) return 48;          // Berry + hold
        if( !strcmp( p_str, "berry-medicine-hold" ) ) return 50; // Berry + hold + medicine

        fprintf( stderr, "Found bad item type %s.\n", p_str );
        return 0;
    }

    u8 getMedicineEffect( char* p_str ) {
        if( !strcmp( p_str, "heal hp" ) )
            return 1; // arg0: amount (0 for percentage); arg1: percentage healed; arg3: 1 if works
                      // on fainted pkmn
        if( !strcmp( p_str, "heal hp and status" ) )
            return 2; // heals all status and heals hp extra arg0: amount (0 for percentage); arg1:
                      // percentage healed; arg2: 1 if works on fainted pkmn
        if( !strcmp( p_str, "heal hp bitter" ) )
            return 3; // heals hp but is bitter, arg0: amount (0 for percentage); arg1: percentage
                      // healed; arg3: 1 if works on fainted pkmn
        if( !strcmp( p_str, "heal status" ) )
            return 4; // arg0 (from 0): all, poison, burn, ice, sleep, paralyze,
        if( !strcmp( p_str, "heal status bitter" ) )
            return 5; // arg0 (from 0): all, poison, burn, ice, sleep, paralyze,
        if( !strcmp( p_str, "heal pp" ) )
            return 6; // arg0: amount (0 for percentage); arg1: percentage healed; arg3: number of
                      // moves
        if( !strcmp( p_str, "sacred ash" ) ) return 7;
        if( !strcmp( p_str, "ev up" ) )
            return 8; // arg0: stat (0: hp; atk, def, satk, sdef, spd); arg1: amount, arg2: limit
        if( !strcmp( p_str, "level up" ) ) return 9; // arg0: amount, arg1: limit
        if( !strcmp( p_str, "pp up" ) )
            return 10; // arg0: amount; arg1: limit; arg2: numer of moves
        if( !strcmp( p_str, "ev down berry" ) )
            return 11; // arg0: stat (0: hp; atk, def, satk, sdef, spd)
        if( !strcmp( p_str, "ability capsule" ) ) return 12;
        if( !strcmp( p_str, "iv up" ) )
            return 13; // arg0: stat (0: hp; atk, def, satk, sdef, spd); arg1: amount, arg2: target
                       // value
        if( !strcmp( p_str, "exp up" ) ) return 14;      // arg0: amount
        if( !strcmp( p_str, "nature mint" ) ) return 15; // arg0: new nature

        fprintf( stderr, "Found bad medicine effect %s.\n", p_str );
        return 0;
    }

    volatileStatus getVolatileStatus( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return NONE;
        if( !strcmp( p_str, "confusion" ) ) return CONFUSION;
        if( !strcmp( p_str, "obstruct" ) ) return OBSTRUCT;
        if( !strcmp( p_str, "partiallytrapped" ) ) return PARTIALLYTRAPPED;
        if( !strcmp( p_str, "flinch" ) ) return FLINCH;
        if( !strcmp( p_str, "octolock" ) ) return OCTOLOCK;
        if( !strcmp( p_str, "tarshot" ) ) return TARSHOT;
        if( !strcmp( p_str, "noretreat" ) ) return NORETREAT;
        if( !strcmp( p_str, "laserfocus" ) ) return LASERFOCUS;
        if( !strcmp( p_str, "spotlight" ) ) return SPOTLIGHT;
        if( !strcmp( p_str, "banefulbunker" ) ) return BANEFULBUNKER;
        if( !strcmp( p_str, "smackdown" ) ) return SMACKDOWN;
        if( !strcmp( p_str, "powder" ) ) return POWDERED;
        if( !strcmp( p_str, "spikyshield" ) ) return SPIKYSHIELD;
        if( !strcmp( p_str, "kingsshield" ) ) return KINGSSHIELD;
        if( !strcmp( p_str, "electrify" ) ) return ELECTRIFY;
        if( !strcmp( p_str, "ragepowder" ) ) return RAGEPOWDER;
        if( !strcmp( p_str, "telekinesis" ) ) return TELEKINESIS;
        if( !strcmp( p_str, "magnetrise" ) ) return MAGNETRISE;
        if( !strcmp( p_str, "aquaring" ) ) return AQUARING;
        if( !strcmp( p_str, "gastroacid" ) ) return GASTROACID;
        if( !strcmp( p_str, "powertrick" ) ) return POWERTRICK;
        if( !strcmp( p_str, "healblock" ) ) return HEALBLOCK;
        if( !strcmp( p_str, "embargo" ) ) return EMBARGO;
        if( !strcmp( p_str, "miracleeye" ) ) return MIRACLEEYE;
        if( !strcmp( p_str, "substitute" ) ) return SUBSTITUTE;
        if( !strcmp( p_str, "bide" ) ) return BIDE;
        if( !strcmp( p_str, "focusenergy" ) ) return FOCUSENERGY;
        if( !strcmp( p_str, "defensecurl" ) ) return DEFENSECURL;
        if( !strcmp( p_str, "minimize" ) ) return MINIMIZE;
        if( !strcmp( p_str, "leechseed" ) ) return LEECHSEED;
        if( !strcmp( p_str, "disable" ) ) return DISABLE;
        if( !strcmp( p_str, "foresight" ) ) return FORESIGHT;
        if( !strcmp( p_str, "snatch" ) ) return SNATCH_;
        if( !strcmp( p_str, "grudge" ) ) return GRUDGE;
        if( !strcmp( p_str, "imprison" ) ) return IMPRISON;
        if( !strcmp( p_str, "yawn" ) ) return YAWN;
        if( !strcmp( p_str, "magiccoat" ) ) return MAGICCOAT;
        if( !strcmp( p_str, "ingrain" ) ) return INGRAIN;
        if( !strcmp( p_str, "helpinghand" ) ) return HELPINGHAND;
        if( !strcmp( p_str, "taunt" ) ) return TAUNT;
        if( !strcmp( p_str, "charge" ) ) return CHARGE_;
        if( !strcmp( p_str, "followme" ) ) return FOLLOWME;
        if( !strcmp( p_str, "torment" ) ) return TORMENT;
        if( !strcmp( p_str, "stockpile" ) ) return STOCKPILE;
        if( !strcmp( p_str, "attract" ) ) return ATTRACT;
        if( !strcmp( p_str, "endure" ) ) return ENDURE;
        if( !strcmp( p_str, "protect" ) ) return PROTECT_;
        if( !strcmp( p_str, "destinybond" ) ) return DESTINYBOND;
        if( !strcmp( p_str, "curse" ) ) return CURSE;
        if( !strcmp( p_str, "nightmare" ) ) return NIGHTMARE;
        if( !strcmp( p_str, "encore" ) ) return ENCORE;

        fprintf( stderr, "unknown volatile status %s\n", p_str );
        return NONE;
    }

    moveFlags getMoveFlag( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return (moveFlags) 0;
        if( !strcmp( p_str, "authentic" ) ) return AUTHENTIC;
        if( !strcmp( p_str, "bite" ) ) return BITE;
        if( !strcmp( p_str, "bullet" ) ) return BULLET;
        if( !strcmp( p_str, "charge" ) ) return CHARGE;
        if( !strcmp( p_str, "contact" ) ) return CONTACT;
        if( !strcmp( p_str, "dance" ) ) return DANCE;
        if( !strcmp( p_str, "defrost" ) ) return DEFROST;
        if( !strcmp( p_str, "distance" ) ) return DISTANCE;
        if( !strcmp( p_str, "gravity" ) ) return GRAVITY;
        if( !strcmp( p_str, "heal" ) ) return HEAL;
        if( !strcmp( p_str, "mirror" ) ) return MIRROR;
        if( !strcmp( p_str, "mindblownrecoil" ) ) return MINDBLOWNRECOIL;
        if( !strcmp( p_str, "nonsky" ) ) return NONSKY;
        if( !strcmp( p_str, "powder" ) ) return POWDER;
        if( !strcmp( p_str, "protect" ) ) return PROTECT;
        if( !strcmp( p_str, "pulse" ) ) return PULSE;
        if( !strcmp( p_str, "protect" ) ) return PROTECT;
        if( !strcmp( p_str, "pulse" ) ) return PULSE;
        if( !strcmp( p_str, "punch" ) ) return PUNCH;
        if( !strcmp( p_str, "recharge" ) ) return RECHARGE;
        if( !strcmp( p_str, "reflectable" ) ) return REFLECTABLE;
        if( !strcmp( p_str, "snatch" ) ) return SNATCH;
        if( !strcmp( p_str, "sound" ) ) return SOUND;
        if( !strcmp( p_str, "lockedmove" ) ) return LOCKEDMOVE;
        if( !strcmp( p_str, "rage" ) ) return RAGE;
        if( !strcmp( p_str, "roost" ) ) return ROOST;
        if( !strcmp( p_str, "uproar" ) ) return UPROAR;
        if( !strcmp( p_str, "selfswitch" ) ) return SELFSWITCH;
        if( !strcmp( p_str, "defasoff" ) ) return DEFASOFF;
        if( !strcmp( p_str, "snatch" ) ) return SNATCH;
        if( !strcmp( p_str, "crashdamage" ) ) return CRASHDAMAGE;
        if( !strcmp( p_str, "ohko" ) ) return OHKO;
        if( !strcmp( p_str, "ohkoice" ) ) return moveFlags( OHKO | OHKOICE );
        if( !strcmp( p_str, "selfdestruct" ) ) return SELFDESTRUCT;
        if( !strcmp( p_str, "selfdestructhit" ) ) return SELFDESTRUCTHIT;
        if( !strcmp( p_str, "nosketch" ) ) return NOSKETCH;
        if( !strcmp( p_str, "noppboost" ) ) return NOPPBOOST;
        if( !strcmp( p_str, "sleepusable" ) ) return SLEEPUSABLE;
        if( !strcmp( p_str, "nofaint" ) ) return NOFAINT;
        if( !strcmp( p_str, "breaksprotect" ) ) return BREAKSPROTECT;
        if( !strcmp( p_str, "ignoreimmunity" ) ) return IGNOREIMMUNITY;
        if( !strcmp( p_str, "ignoreimmunitygnd" ) ) return IGNOREIMMUNITYGROUND;
        if( !strcmp( p_str, "ignoreability" ) ) return IGNOREABILITY;
        if( !strcmp( p_str, "ignoredefs" ) ) return IGNOREDEFS;
        if( !strcmp( p_str, "ignoreevasion" ) ) return IGNOREEVASION;
        if( !strcmp( p_str, "defrosttarget" ) ) return DEFROSTTARGET;
        if( !strcmp( p_str, "forceswitch" ) ) return FORCESWITCH;
        if( !strcmp( p_str, "willcrit" ) ) return WILLCRIT;
        if( !strcmp( p_str, "targetoffensives" ) ) return TARGETOFFENSIVES;
        if( !strcmp( p_str, "futuremove" ) ) return FUTUREMOVE;
        if( !strcmp( p_str, "batonpass" ) ) return BATONPASS;

        fprintf( stderr, "Found bad move flag %s.\n", p_str );
        return (moveFlags) 0;
    }

    moveFlags parseMoveFlags( char* p_buffer ) {
        char*     p   = strtok( p_buffer, "|" );
        moveFlags res = getMoveFlag( p );

        while( ( p = strtok( NULL, "|" ) ) ) { res = moveFlags( res | getMoveFlag( p ) ); }

        return res;
    }

    targets getTarget( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return NORMAL;
        if( !strcmp( p_str, "normal" ) ) return NORMAL;
        if( !strcmp( p_str, "adjacentAlly" ) ) return ADJACENT_ALLY;
        if( !strcmp( p_str, "adjacentFoe" ) ) return ADJACENT_FOE;
        if( !strcmp( p_str, "any" ) ) return ANY;
        if( !strcmp( p_str, "allies" ) ) return ALLIES;
        if( !strcmp( p_str, "allyTeam" ) ) return ALLY_TEAM;
        if( !strcmp( p_str, "scripted" ) ) return SCRIPTED; // Metal burst, counter, etc
        if( !strcmp( p_str, "adjacentAllyOrSelf" ) ) return ADJACENT_ALLY_OR_SELF;
        if( !strcmp( p_str, "self" ) ) return SELF;
        if( !strcmp( p_str, "randomNormal" ) ) return RANDOM;
        if( !strcmp( p_str, "allAdjacent" ) ) return ALL_ADJACENT;
        if( !strcmp( p_str, "allAdjacentFoes" ) ) return ALL_ADJACENT_FOES;
        if( !strcmp( p_str, "allySide" ) ) return ALLY_SIDE;
        if( !strcmp( p_str, "foeSide" ) ) return FOE_SIDE;
        if( !strcmp( p_str, "all" ) ) return ALL;

        fprintf( stderr, "unknown target %s\n", p_str );
        return (targets) 0;
    }

    u8 getFixedDamage( char* p_str ) {
        if( !strcmp( p_str, "level" ) ) return 255;
        if( !strcmp( p_str, "none" ) ) return 0;
        u8 res = 0;
        if( !sscanf( p_str, "%hhu", &res ) ) {
            fprintf( stderr, "Unknown fixed damage type %s.\n", p_str );
        }
        return res;
    }

    moveHitTypes getMoveCategory( char* p_str, moveHitTypes p_default ) {
        if( !strcmp( p_str, "none" ) ) return p_default;
        if( !strcmp( p_str, "Physical" ) ) return PHYSICAL;
        if( !strcmp( p_str, "Special" ) ) return SPECIAL;
        if( !strcmp( p_str, "Status" ) ) return STATUS;

        fprintf( stderr, "unknown move category %s\n", p_str );
        return p_default;
    }

    u8 getWeather( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;

        if( !strcmp( p_str, "RainDance" ) ) return 1;
        if( !strcmp( p_str, "hail" ) ) return 2;
        if( !strcmp( p_str, "fog" ) ) return 3;
        if( !strcmp( p_str, "Sandstorm" ) ) return 4;
        if( !strcmp( p_str, "sunnyday" ) ) return 5;
        if( !strcmp( p_str, "heavyrain" ) ) return 6;
        if( !strcmp( p_str, "heavysun" ) ) return 7;
        if( !strcmp( p_str, "strongwinds" ) ) return 8;

        fprintf( stderr, "unknown weather %s\n", p_str );
        return 0;
    }

    u8 getPseudoWeather( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "iondeluge" ) ) return 1;
        if( !strcmp( p_str, "magicroom" ) ) return 2;
        if( !strcmp( p_str, "wonderroom" ) ) return 3;
        if( !strcmp( p_str, "trickroom" ) ) return 4;
        if( !strcmp( p_str, "gravity" ) ) return 5;
        if( !strcmp( p_str, "watersport" ) ) return 6;
        if( !strcmp( p_str, "mudsport" ) ) return 7;
        if( !strcmp( p_str, "fairylock" ) ) return 8;

        fprintf( stderr, "unknown pseudo weather %s\n", p_str );
        return 0;
    }

    u8 getTerrain( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "psychicterrain" ) ) return 1;
        if( !strcmp( p_str, "electricterrain" ) ) return 2;
        if( !strcmp( p_str, "mistyterrain" ) ) return 3;
        if( !strcmp( p_str, "grassyterrain" ) ) return 4;

        fprintf( stderr, "unknown terrain %s\n", p_str );
        return 0;
    }

    u16 getSideCondition( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "craftyshield" ) ) return ( 1 << 0 );
        if( !strcmp( p_str, "stickyweb" ) ) return ( 1 << 1 );
        if( !strcmp( p_str, "matblock" ) ) return ( 1 << 2 );
        if( !strcmp( p_str, "quickguard" ) ) return ( 1 << 3 );
        if( !strcmp( p_str, "wideguard" ) ) return ( 1 << 4 );
        if( !strcmp( p_str, "stealthrock" ) ) return ( 1 << 5 );
        if( !strcmp( p_str, "toxicspikes" ) ) return ( 1 << 6 );
        if( !strcmp( p_str, "luckychant" ) ) return ( 1 << 7 );
        if( !strcmp( p_str, "tailwind" ) ) return ( 1 << 8 );
        if( !strcmp( p_str, "safeguard" ) ) return ( 1 << 9 );
        if( !strcmp( p_str, "spikes" ) ) return ( 1 << 10 );
        if( !strcmp( p_str, "reflect" ) ) return ( 1 << 11 );
        if( !strcmp( p_str, "lightscreen" ) ) return ( 1 << 12 );
        if( !strcmp( p_str, "mist" ) ) return ( 1 << 13 );
        if( !strcmp( p_str, "auroraveil" ) ) return ( 1 << 14 );

        fprintf( stderr, "unknown side condition %s\n", p_str );
        return 0;
    }

    u8 getSlotCondition( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "Wish" ) ) return 1;
        if( !strcmp( p_str, "healingwish" ) ) return 2;
        if( !strcmp( p_str, "lunardance" ) ) return 3;

        fprintf( stderr, "unknown slot condition %s\n", p_str );
        return 0;
    }

    u8 getStatus( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;
        if( !strcmp( p_str, "brn" ) ) return 1;
        if( !strcmp( p_str, "par" ) ) return 2;
        if( !strcmp( p_str, "frz" ) ) return 3;
        if( !strcmp( p_str, "slp" ) ) return 4;
        if( !strcmp( p_str, "psn" ) ) return 5;
        if( !strcmp( p_str, "tox" ) ) return 6;

        fprintf( stderr, "unknown status %s\n", p_str );
        return 0;
    }

    u8 getNumberOrNone( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;

        u8 num;
        sscanf( p_str, "%hhu", &num );
        return num;
    }

    u8 parseFraction( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;

        u8 num, denom;
        sscanf( p_str, "%hhu|%hhu", &num, &denom );
        return u8( num * 240.0 / denom );
    }

    u8 parseMultiHit( char* p_str ) {
        if( !strcmp( p_str, "none" ) ) return 0;

        u8 mn, mx;
        if( sscanf( p_str, "%hhu|%hhu", &mn, &mx ) ) return ( mn << 8 ) | mx;

        if( sscanf( p_str, "%hhu", &mn ) ) return ( mn << 8 ) | mn;

        return 0;
    }

    void parseBoost( char* p_str, boosts& p_out ) {
        char buf[ 50 ];
        s8   val;
        sscanf( p_str, "%[^:]:%hhd", buf, &val );

        if( !strcmp( buf, "atk" ) ) return p_out.setBoost( ATK, val );
        if( !strcmp( buf, "def" ) ) return p_out.setBoost( DEF, val );
        if( !strcmp( buf, "spa" ) ) return p_out.setBoost( SATK, val );
        if( !strcmp( buf, "spd" ) ) return p_out.setBoost( SDEF, val );
        if( !strcmp( buf, "spe" ) ) return p_out.setBoost( SPEED, val );
        if( !strcmp( buf, "accuracy" ) ) return p_out.setBoost( ACCURACY, val );
        if( !strcmp( buf, "evasion" ) ) return p_out.setBoost( EVASION, val );

        fprintf( stderr, "unknown boost type %s\n", buf );
    }

    boosts parseBoosts( char* p_buffer ) {
        boosts res = { 0 };
        if( !strcmp( p_buffer, "none" ) ) return res;

        char* p = strtok( p_buffer, "|" );
        if( !p ) {
            parseBoost( p_buffer, res );
        } else {
            do { parseBoost( p, res ); } while( ( p = strtok( NULL, "|" ) ) );
        }
        return res;
    }

    FILE* getFilePtr( std::string p_prefix, u16 p_index, u8 p_digits, std::string p_ext,
                      u8 p_formeId ) {
        char buffer[ 50 ];
        char buffer2[ 100 ];
        snprintf( buffer2, 20, "%%0%hhuhhu/", p_digits );
        snprintf( buffer, 40, buffer2, p_index / MAX_ITEMS_PER_DIR );

        fs::create_directories( p_prefix + buffer );
        if( !p_formeId )
            snprintf( buffer2, 80, "%s/%s%hu%s", p_prefix.c_str( ), buffer, p_index,
                      p_ext.c_str( ) );
        else
            snprintf( buffer2, 80, "%s/%s%hu_%hhu%s", p_prefix.c_str( ), buffer, p_index, p_formeId,
                      p_ext.c_str( ) );

        return fopen( buffer2, "wb" );
    }

    void printNormalized( char* p_string, FILE* p_f ) {
        while( *p_string ) {
            if( *p_string == '\xe9' || *p_string == '\xc9' ) {
                fputc( 'E', p_f );
                ++p_string;
                continue;
            }
            if( *p_string == '{' ) {
                fputc( '_', p_f );
                fputc( 'M', p_f );
                ++p_string;
                continue;
            }
            if( *p_string == '}' ) {
                fputc( '_', p_f );
                fputc( 'F', p_f );
                ++p_string;
                continue;
            }
            if( *p_string == '.' ) {
                ++p_string;
                continue;
            }
            if( *p_string == '\'' || *p_string == ' ' || *p_string == ':' || *p_string == '-'
                || *p_string == '?' ) {
                fputc( '_', p_f );
                ++p_string;
                continue;
            }
            fputc( toupper( *( p_string++ ) ), p_f );
        }
    }

    void readNames( char* p_path, std::vector<names>& p_out ) {
        FILE* f = fopen( p_path, "r" );
        char  buffer[ 500 ];
        char* t1;
        while( fgets( buffer, sizeof( buffer ), f ) ) {
            t1 = strtok( buffer, "," );
            names n;
            for( int i = 0; i < NUM_LANGUAGES; ++i ) { n.m_name[ i ] = new char[ 30 ]; }
            int cnt = 0;
            while( cnt < NUM_LANGUAGES && ( t1 = strtok( NULL, "," ) ) ) {
                strncpy( n.m_name[ cnt++ ], fixEncoding( t1 ), 29 );
            }
            p_out.push_back( n );
        }
        fprintf( stderr, "read %lu objects from %s\n", p_out.size( ), p_path );
    }

    void readNames( char* p_path, std::map<u16, names>& p_out ) {
        FILE* f = fopen( p_path, "r" );
        char  buffer[ 500 ];
        char* t1;
        while( fgets( buffer, sizeof( buffer ), f ) ) {
            t1 = strtok( buffer, "," );
            u16 id;
            sscanf( t1, "%hu", &id );
            names n;
            for( int i = 0; i < NUM_LANGUAGES; ++i ) { n.m_name[ i ] = new char[ 30 ]; }
            int cnt = 0;
            while( cnt < NUM_LANGUAGES && ( t1 = strtok( NULL, "," ) ) ) {
                strncpy( n.m_name[ cnt++ ], fixEncoding( t1 ), 29 );
            }
            p_out[ id ] = n;
        }
        fprintf( stderr, "read %lu objects from %s\n", p_out.size( ), p_path );
    }
} // namespace DATA
