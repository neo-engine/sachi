#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "../fs.h"

namespace DATA {
    bool checkOrOpen( FILE*& p_f, const char* p_path );
    bool checkOrOpen( FILE*& p_f, const char* p_path, u8& p_lastLang, u8 p_language );

    FILE* open( const char* p_path, const char* p_name, const char* p_ext = ".raw",
                const char* p_mode = "rb" );
    FILE* open( const char* p_path, u16 p_value, const char* p_ext = ".raw",
                const char* p_mode = "rb" );

    FILE* openSplit( const char* p_path, u16 p_value, const char* p_ext = ".raw",
                     u16 p_maxValue = 99 * MAX_ITEMS_PER_DIR, const char* p_mode = "rb" );

    FILE* openBank( const char* p_path, u8 p_lang, const char* p_ext, const char* p_mode = "rb" );

    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count );
    bool   readNop( FILE* p_file, u32 p_cnt );

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt,
                   unsigned short* p_data );
    template <typename T1, typename T2>
    bool readData( const char* p_path, const char* p_name, T1 p_dataCnt1, T1* p_data1,
                   T2 p_dataCnt2, T2* p_data2 ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        read( fd, p_data1, sizeof( T1 ), p_dataCnt1 );
        read( fd, p_data2, sizeof( T2 ), p_dataCnt2 );
        fclose( fd );
        return true;
    }

    template <typename T1, typename T2>
    bool readData( const char* p_path, T1 p_dataCnt1, T1* p_data1, T2 p_dataCnt2, T2* p_data2 ) {
        FILE* fd = fopen( p_path, "rb" );
        if( !fd ) return false;
        read( fd, p_data1, sizeof( T1 ), p_dataCnt1 );
        read( fd, p_data2, sizeof( T2 ), p_dataCnt2 );
        fclose( fd );
        return true;
    }

    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count );
    bool   writeNop( FILE* p_file, u32 p_cnt );

    bool readTiles( FILE* p_file, tile* p_tileSet, u16 p_startIdx = 0, u16 p_size = 512 );
    bool readPal( FILE* p_file, palette* p_palette, u8 p_count = 6 );
    bool readBlocks( FILE* p_file, block* p_tileSet, u16 p_startIdx = 0, u16 p_size = 512 );
    bool readMapData( FILE* p_file, mapData* p_result, bool p_close = true );

    bool readMapSlice( FILE* p_mapFile, mapSlice* p_result, u16 p_x = 0, u16 p_y = 0,
                       bool p_close = true );

    bool readMapSliceAndData( FILE* p_mapFile, mapSlice* p_slice, mapData* p_data, u16 p_x,
                              u16 p_y );

    bool readMapBank( FILE* p_mapFile, mapBankInfo* p_info, mapBank* p_out );

    bool readPictureData( unsigned int* p_imgOut, unsigned short* p_palOut, const char* p_path,
                          const char* p_name );
    bool readPictureData( unsigned int* p_imgOut, unsigned short* p_palOut, const char* p_path );

    bool writeTiles( FILE* p_file, const tile* p_tileSet, u16 p_startIdx = 0, u16 p_size = 512 );
    bool writePal( FILE* p_file, const palette* p_palette, u8 p_count = 6 );
    bool writeBlocks( FILE* p_file, const block* p_tileSet, u16 p_startIdx = 0, u16 p_size = 512 );

    bool writeMapData( FILE* p_file, const mapData* p_data, bool p_close = true );
    bool writeMapSlice( FILE* p_mapFile, const mapSlice* p_map, bool p_close = true );
    bool writeMapSliceAndData( FILE* p_mapFile, const mapBankInfo& p_info, const mapSlice* p_slice,
                               const mapData* p_data, u16 p_x, u16 p_y );

    bool writeMapBank( FILE* p_mapFile, const mapBankInfo* p_info, const mapBank* p_out );

    size_t         getLength( u8 p_c );
    char           getValue( char* p_text, size_t* p_readIdx );
    char*          fixEncoding( char* p_utf8 );
    u8             getType( char* p_str );
    u8             getContestType( char* p_str );
    u8             getLevelUpType( char* p_str );
    u8             getEggGroup( char* p_str );
    u8             getExpType( char* p_str );
    u8             getGender( char* p_str );
    u8             getColor( char* p_str );
    u8             getShape( char* p_str );
    u8             getItemType( char* p_str );
    u8             getMedicineEffect( char* p_str );
    volatileStatus getVolatileStatus( char* p_str );
    moveFlags      getMoveFlag( char* p_str );
    moveFlags      parseMoveFlags( char* p_buffer );
    targets        getTarget( char* p_str );
    u8             getFixedDamage( char* p_str );
    moveHitTypes   getMoveCategory( char* p_str, moveHitTypes p_default = (moveHitTypes) 0 );
    u8             getWeather( char* p_str );
    u8             getPseudoWeather( char* p_str );
    u8             getTerrain( char* p_str );
    u16            getSideCondition( char* p_str );
    u8             getSlotCondition( char* p_str );
    u8             getStatus( char* p_str );
    u8             getNumberOrNone( char* p_str );
    u8             parseFraction( char* p_str );
    u8             parseMultiHit( char* p_str );
    void           parseBoost( char* p_str, boosts& p_out );
    boosts         parseBoosts( char* p_buffer );

    FILE* getFilePtr( std::string p_prefix, u16 p_index, u8 p_digits, std::string p_ext = ".data",
                      u8 p_formeId = 0 );

    void printNormalized( char* p_string, FILE* p_f );

    void readNames( char* p_path, std::vector<names>& p_out );
    void readNames( char* p_path, std::map<u16, names>& p_out );
} // namespace DATA
