#pragma once
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

#define conv( a )    ( ( u8 )( (a) *31 / 255 ) )
#define revconv( a ) ( ( (a) *255 / 31 ) )

#define green( a ) ( revconv( ( ( a ) >> 5 ) & 31 ) )
#define blue( a )  ( revconv( ( ( a ) >> 10 ) & 31 ) )
#define red( a )   ( revconv( ( ( a ) >> 0 ) & 31 ) )

typedef uint8_t           u8;
typedef uint16_t          u16;
typedef uint32_t          u32;
typedef uint64_t          u64;
typedef unsigned __int128 u128;
typedef int8_t            s8;
typedef int16_t           s16;
typedef int32_t           s32;
typedef int64_t           s64;
typedef __int128          s128;

extern std::string FSROOT_PATH;
extern std::string MAP_PATH;
extern std::string TILESET_PATH;
extern std::string BLOCKSET_PATH;
extern std::string PALETTE_PATH;
extern std::string MAPDATA_PATH;

constexpr u8 MAP_SIZE = 32;

constexpr s64 MAX_MAPBANK_NAME = 99;
constexpr u8  MAX_MAPY         = 255;

const std::string MAPNAME_FORMAT  = "%hhu_%hhu.map";
const std::string BLOCKSET_FORMAT = "%hhu.bvd";

constexpr u8 MARGIN = 5;

const std::string EXTRA_CSS
    = ".mapbank-saved { background-color: rgb( 180, 240, 180 ); color: #2e3436; }\n"
      ".mapbank-created { background-color: rgb( 180, 240, 240 ); color: #2e3436; }\n"
      ".mapbank-modified { background-color: rgb( 240, 240, 180 ); color: #2e3436; }\n"
      ".mapbank-selected { border-color: @theme_selected_bg_color; color: @theme_selected_bg_color; background-color: @theme_selected_fg_color; }";
