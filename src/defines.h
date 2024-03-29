#pragma once
#include <filesystem>
#include <string>
namespace fs = std::filesystem;
using namespace std::string_literals;

const std::string APP_NAME     = std::string( "Sachi" );
const std::string TITLE_STRING = APP_NAME + " " + std::string( VERSION );

#define conv( a )    ( (u8) ( ( a ) * 31 / 255 ) )
#define revconv( a ) ( ( ( a ) * 255 / 31 ) )

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

constexpr u8 DAYTIMES = 5;
constexpr u8 MAP_SIZE = 32;

constexpr u16 DIVE_MAP = 1000;

constexpr s64 MAX_MAPBANK_NAME = 99;
constexpr s64 MAX_TILESET_NAME = 255;
constexpr u8  MAX_MAPY         = 255;
constexpr s64 MAX_TRAINER_NAME = 2048;

const std::string MAPNAME_FORMAT  = "%hhu_%hhu.map";
const std::string BLOCKSET_FORMAT = "%hhu.bvd";

constexpr u8 MARGIN = 5;

extern char TMP_BUFFER[ 100 ];
extern char TMP_BUFFER_SHORT[ 50 ];

const std::string EXTRA_CSS
    = ".mapbank-saved { background-color: rgb( 180, 240, 180 ); color: #2e3436; }\n"
      ".mapbank-created { background-color: rgb( 180, 240, 240 ); color: #2e3436; }\n"
      ".mapbank-modified { background-color: rgb( 240, 240, 180 ); color: #2e3436; }\n"
      ".mapbank-selected { border-color: @theme_selected_bg_color; color: @theme_selected_bg_color; background-color: @theme_selected_fg_color; }\n"
      ".mapblock-selected { border-style: solid; border-width: 2px; border-color: red; }\n"
      ".movement-any { color: black; background-color: white; }\n"        /* 0x0 */
      ".movement-none { color: black; background-color: #fc797b; }\n"     /* 0x1 */
      ".movement-water { color: black; background-color: #79bdfc; }\n"    /* 0x4 */
      ".movement-walk { color: black; background-color: #bdfc79; }\n"     /* 0xc */
      ".movement-sit { color: black; background-color: #fc79bd; }\n"      /* 0xa */
      ".movement-blocked { color: black; background-color: #fcb879; }\n"  /* 1 + 4 * k, k > 0 */
      ".movement-elevated { color: black; background-color: #7bfc79; }\n" /* 0xc + 4 * k, k > 0 */
      ".movement-bridge { color: black; background-color: #fcfa79; }\n"   /* 0x3c */
      ".movement-border { color: white; background-color: purple; }\n"    /* 0x3f */
      ".movement-weird { color: white; background-color: black; }\n"      /* anything else */
      ".mark-warp { color: white; background-color: purple; }\n"
      ".mark-flypos { color: black; background-color: #7bfc79; }\n"
      ".mark-script { color: black; background-color: #fc79bd; }\n"
      ".mark-message { color: black; background-color: #fc797b; }\n"
      ".mark-sight { color: black; background-color: #79bdfc; }\n"
      ".mark-sight-red { color: black; background-color: #fcb879; }\n"
      ".mark-movement { color: black; background-color: #bdfc79; }\n"
      ".mark-berry { color: black; background-color: #f0cdc5; }\n"
      ".encounter-grass { color: black; background-color: #1bf579; }\n"
      ".encounter-long-grass { color: white; background-color: #037333; }\n"
      ".encounter-water { color: black; background-color: #4f85db; }\n"
      ".encounter-old-rod { color: black; background-color: #a587ff; }\n"
      ".encounter-good-rod { color: black; background-color: #896edb; }\n"
      ".encounter-super-rod { color: white; background-color: #5f4d99; }\n"
      ".encounter-headbutt { color: black; background-color: #72dbcf; }\n"
      ".encounter-rock-smash { color: white; background-color: #5e3b03; }\n"
      ".encounter-sweet-scent { color: black; background-color: #f2bdf1; }\n"
      ".encounter-radar { color: black; background-color: #e5fcc5; }\n"
      ".encounter-swarm { color: black; background-color: #f0cdc5; }\n"
      ".compress { padding-left: 4px; padding-right: 4px; }\n"
      ".no-padding { padding: 0px; }\n"
      "button.up, button.down { padding: 0px; }\n"
      "";

enum context : u8 {
    CONTEXT_NONE,           // nothing loaded
    CONTEXT_FSROOT_NONE,    // fsroot loaded, but nothing else
    CONTEXT_MAP_EDITOR,     // map bank loaded and map visible
    CONTEXT_TILE_EDITOR,    // tile set editor loaded
    CONTEXT_TRAINER_EDITOR, // trainer editor loaded
    CONTEXT_PKMN_DATA_EDITOR,
    CONTEXT_ITEM_DATA_EDITOR,
    CONTEXT_MOVE_DATA_EDITOR,
};
