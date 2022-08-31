#pragma once

#include <typeinfo>
#include <vector>

#include "../defines.h"
#include "bitmap.h"

namespace DATA {
    constexpr u16 MAX_TILES_PER_TILE_SET  = 512;
    constexpr u16 MAX_BLOCKS_PER_TILE_SET = 512;
    constexpr u8  MAX_MOVEMENTS           = 64;

    constexpr u16 BLOCK_SIZE   = 16;
    constexpr u16 BLOCK_LAYERS = 2;
    constexpr u16 TILE_SIZE    = BLOCK_SIZE / 2;

    constexpr u16 MAX_FLAG = 255;
    constexpr u16 MAX_VARS = 255;

    constexpr u16 PKMN_SPRITE = 1000;

    constexpr u16 SIZE          = 32;
    constexpr s16 dir[ 4 ][ 2 ] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    struct palette {
        u16 m_pal[ 16 ];

        void dump( ) const;
    };

    constexpr palette DEFAULT_PALETTE = { { 0 } };

    struct tile {
        u8 m_tile[ TILE_SIZE * TILE_SIZE / 2 ];

        void dump( ) const;

        inline u8 at( u8 p_x, u8 p_y ) const {
            u8 cl = m_tile[ TILE_SIZE * p_y / 2 + p_x / 2 ];

            if( p_x & 1 ) {
                cl >>= 4;
            } else {
                cl &= 0xF;
            }
            return cl;
        }

        inline void set( u8 p_x, u8 p_y, u8 p_color ) {
            u8 cl = m_tile[ TILE_SIZE * p_y / 2 + p_x / 2 ];

            if( p_x & 1 ) {
                p_color = ( p_color & 0xF ) << 4;
                cl      = ( cl & 0x0F ) | p_color;
            } else {
                p_color = p_color & 0xF;
                cl      = ( cl & 0xF0 ) | p_color;
            }

            m_tile[ TILE_SIZE * p_y / 2 + p_x / 2 ] = cl;
        }
    };
    template <u16 t_size = 2>
    struct tileSet {
        tile m_tiles[ t_size * MAX_TILES_PER_TILE_SET ];
    };

    constexpr tile DEFAULT_TILE = { { 0 } };

    struct computedBlockAtom {
        tile m_tile;
        u8   m_vflip : 1;
        u8   m_hflip : 1;
        u8   m_palno : 4;
        u8 : 2;
    };

    struct blockAtom {
        u16 m_tileidx : 10 = 0;
        u8  m_vflip : 1    = 0;
        u8  m_hflip : 1    = 0;
        u8  m_palno : 4    = 0;

        inline computedBlockAtom compute( const tileSet<2>* p_tiles ) const {
            computedBlockAtom res = { p_tiles->m_tiles[ m_tileidx ], m_vflip, m_hflip, m_palno };
            return res;
        }
    };

    class computedBlock {
      public:
        computedBlockAtom m_top[ BLOCK_SIZE / TILE_SIZE ][ BLOCK_SIZE / TILE_SIZE ]
            = { computedBlockAtom( ) };
        u8 m_topbehave = 0;

        computedBlockAtom m_bottom[ BLOCK_SIZE / TILE_SIZE ][ BLOCK_SIZE / TILE_SIZE ]
            = { computedBlockAtom( ) };
        u8 m_bottombehave = 0;
    };

    class block {
      public:
        blockAtom m_top[ BLOCK_SIZE / TILE_SIZE ][ BLOCK_SIZE / TILE_SIZE ] = { blockAtom( ) };
        u8        m_topbehave                                               = 0;

        blockAtom m_bottom[ BLOCK_SIZE / TILE_SIZE ][ BLOCK_SIZE / TILE_SIZE ] = { blockAtom( ) };
        u8        m_bottombehave                                               = 0;

        inline computedBlock compute( const tileSet<2>* p_tiles ) const {
            auto res           = computedBlock( );
            res.m_topbehave    = m_topbehave;
            res.m_bottombehave = m_bottombehave;

            for( u8 i = 0; i < 2; ++i ) {
                for( u8 j = 0; j < 2; ++j ) {
                    res.m_top[ i ][ j ]    = m_top[ i ][ j ].compute( p_tiles );
                    res.m_bottom[ i ][ j ] = m_bottom[ i ][ j ].compute( p_tiles );
                }
            }
            return res;
        }
    };

    constexpr block DEFAULT_BLOCK = block( );

    template <u16 t_size = 2>
    struct blockSet {
        block m_blocks[ t_size * MAX_BLOCKS_PER_TILE_SET ];
    };

    struct mapBlockAtom {
        u16 m_blockidx : 10 = 0;
        u8  m_movedata : 6  = 1;

        static inline std::vector<std::pair<computedBlock, u8>>
        computeBlockSet( const blockSet<1>* p_blocks, const tileSet<2>* p_tiles ) {
            auto res = std::vector<std::pair<computedBlock, u8>>( );
            for( auto b = 0; b < MAX_BLOCKS_PER_TILE_SET; ++b ) {
                res.push_back( { p_blocks->m_blocks[ b ].compute( p_tiles ), 0 } );
            }
            return res;
        }

        static constexpr u8 MOVEMENT_ORDER[ MAX_MOVEMENTS ] = {
            0x0,  0x1,  0x4,  0xc,  0xa,  0x3c, 0x8,  0x10, 0x14, 0x18, 0x1c, 0x20, 0x24,
            0x28, 0x2c, 0x30, 0x34, 0x38, 0x5,  0x9,  0xd,  0x11, 0x15, 0x19, 0x1d, 0x21,
            0x25, 0x29, 0x2d, 0x31, 0x35, 0x39, 0x3d, 0x3f, 0x2,  0x6,  0xe,  0x12, 0x16,
            0x1a, 0x1e, 0x22, 0x26, 0x2a, 0x2e, 0x32, 0x36, 0x3a, 0x3e, 0x3,  0x7,  0xb,
            0xf,  0x13, 0x17, 0x1b, 0x1f, 0x23, 0x27, 0x2b, 0x2f, 0x33, 0x37, 0x3b,
        };

        static inline std::vector<std::pair<computedBlock, u8>> computeMovementSet( ) {
            auto res = std::vector<std::pair<computedBlock, u8>>( );

            for( auto b = 0; b < MAX_MOVEMENTS; ++b ) {
                res.push_back( { computedBlock( ), MOVEMENT_ORDER[ b ] } );
            }
            return res;
        }
    };

    struct mapSliceData {
        u8 m_sizeX = SIZE; // ignored / assumed to be SIZE
        u32 : 24;
        u8 m_sizeY = SIZE; // ignored / assumed to be SIZE
        u32 : 24;

        u8 m_tIdx1 = 0;
        u32 : 24;
        u8 m_tIdx2 = 1;
        u32 : 24;

        u8 m_borderSizeX = 0; // ignored / assumed to be 0
        u8 m_borderSizeY = 0; // ignored / assumed to be 0
        u32 : 16;

        mapBlockAtom m_blocks[ SIZE ][ SIZE ] = { { mapBlockAtom( ) } }; // [ y ][ x ]
    };

    struct largeMapSliceHeader {
        u8 m_sizeX = 0;
        u32 : 24;
        u8 m_sizeY = 0;
        u32 : 24;

        u8 m_tIdx1 = 0;
        u32 : 24;
        u8 m_tIdx2 = 1;
        u32 : 24;

        u8 m_borderSizeX = 0;
        u8 m_borderSizeY = 0;
        u32 : 16;
    };

    struct mapSlice {
        mapSliceData m_data;
        u16          m_x = 0, m_y = 0;

        inline std::vector<std::pair<computedBlock, u8>> compute( const blockSet<2>* p_blocks,
                                                                  const tileSet<2>*  p_tiles ) {
            auto res = std::vector<std::pair<computedBlock, u8>>( );
            for( u8 y = 0; y < SIZE; ++y ) {
                for( u8 x = 0; x < SIZE; ++x ) {
                    auto bidx = m_data.m_blocks[ y ][ x ].m_blockidx;
                    res.push_back( { p_blocks->m_blocks[ bidx ].compute( p_tiles ),
                                     u8( m_data.m_blocks[ y ][ x ].m_movedata ) } );
                }
            }
            return res;
        }
    };

    struct computedMapSlice {
        std::vector<std::pair<computedBlock, u8>> m_computedBlocks;
        DATA::palette                             m_pals[ 5 * 16 ];

        inline computedMapSlice( ) {
            m_computedBlocks
                = std::vector<std::pair<computedBlock, u8>>( SIZE * SIZE, { computedBlock( ), 1 } );
            memset( m_pals, 0, sizeof( m_pals ) );
        }
    };

    struct position {
        u16 m_posX; // Global
        u16 m_posY; // Global
        u8  m_posZ;

        constexpr auto operator<=>( const position& ) const = default;

        static inline position fromLocal( u8 p_mapX, u8 p_localX, u8 p_mapY, u8 p_localY, u8 p_z ) {
            return { (u16) ( p_mapX * SIZE + p_localX ), (u16) ( p_mapY * SIZE + p_localY ),
                     (u8) p_z };
        }

        constexpr u8 mapX( ) const {
            return m_posX / SIZE;
        }
        constexpr u8 mapY( ) const {
            return m_posY / SIZE;
        }

        constexpr u8 localX( ) const {
            return m_posX % SIZE;
        }
        constexpr u8 localY( ) const {
            return m_posY % SIZE;
        }
    };
    enum direction : u8 { UP, RIGHT, DOWN, LEFT };

    struct movement {
        direction m_direction;
        u8        m_frame;
    };

    typedef std::pair<u8, position> warpPos;
    struct flyPos {
        u8  m_owBank; // position on ow map where this fly pos should appear
        u8  m_targetBank;
        u16 m_targetZ : 4;
        u16 m_owMapX : 6;
        u16 m_owMapY : 6;

        u16 m_targetX;
        u16 m_targetY;

        /*
         * @brief: returns the ow map this flypos shoul appear on
         */
        constexpr auto owMap( ) const {
            return m_owBank;
        }

        /*
         * @brief: returns the target position this fly pos should warp to.
         */
        constexpr warpPos target( ) const {
            return { m_targetBank, { m_targetX, m_targetY, u8( m_targetZ ) } };
        }

        constexpr auto operator<=>( const flyPos& ) const = default;
    };

    enum moveMode : u8 {
        // Player modes
        WALK       = 0,
        BIKE       = ( 1 << 2 ),
        ACRO_BIKE  = ( 1 << 2 ) | ( 1 << 0 ),
        MACH_BIKE  = ( 1 << 2 ) | ( 1 << 1 ),
        BIKE_JUMP  = ( 1 << 2 ) | ( 1 << 0 ) | ( 1 << 1 ),
        SIT        = ( 1 << 3 ),
        DIVE       = ( 1 << 4 ),
        ROCK_CLIMB = ( 1 << 5 ),
        STRENGTH   = ( 1 << 6 ),
        SURF       = ( 1 << 7 ),
        // NPC modes
        NO_MOVEMENT             = 0,
        LOOK_UP                 = 1,
        LOOK_DOWN               = 2,
        LOOK_RIGHT              = 4,
        LOOK_LEFT               = 8,
        WALK_LEFT_RIGHT         = 16,
        WALK_UP_DOWN            = 17,
        WALK_CIRCLE             = 18,
        WALK_AROUND_LEFT_RIGHT  = 19,
        WALK_AROUND_UP_DOWN     = 20,
        WALK_CONT_LEFT_RIGHT    = 21,
        WALK_CONT_UP_DOWN       = 22,
        WALK_AROUND_SQUARE      = 23, // randomly walks around in a 3x3 square
        WALK_CONT_FOLLOW_OBJECT = 24, // walks along the edge of an object, touching w/ right hand

    };
    const std::vector<std::string> MOVE_MODE_NAMES{ "Look Up",
                                                    "Look Down",
                                                    "Look Right",
                                                    "Look Left",
                                                    "Walk Left-Right",
                                                    "Walk Up-Down",
                                                    "Walk Circle",
                                                    "Walk Around Left-Right",
                                                    "Walk Around Up-Down",
                                                    "Walk Non-Stop Left-Right",
                                                    "Walk Non-Stop Up-Down",
                                                    "Walk Around Square",
                                                    "Walk Follow Edge" };

    constexpr u8 getFrame( direction p_direction ) {
        switch( p_direction ) {
        case UP: return 3;
        case RIGHT: return 9;
        case DOWN: return 0;
        case LEFT: return 6;
        }
        return 0;
    }

    constexpr u8 getOWPKMNFrame( direction p_direction ) {
        switch( p_direction ) {
        case UP: return 2;
        case RIGHT: return 6;
        case DOWN: return 0;
        case LEFT: return 4;
        }
        return 0;
    }

    inline std::function<u8( direction )> frameFuncionForIdx( u16 p_owSprite ) {
        if( p_owSprite > PKMN_SPRITE ) {
            return getOWPKMNFrame;
        } else {
            return getFrame;
        }
    }

    inline u8 moveModeToFrame( moveMode p_moveMode, std::function<u8( direction )> p_dirToFrame ) {
        if( p_moveMode & LOOK_DOWN ) { return p_dirToFrame( DOWN ); }
        if( p_moveMode & LOOK_UP ) { return p_dirToFrame( UP ); }
        if( p_moveMode & LOOK_LEFT ) { return p_dirToFrame( LEFT ); }
        if( p_moveMode & LOOK_RIGHT ) { return p_dirToFrame( RIGHT ); }
        if( p_moveMode == WALK_LEFT_RIGHT || p_moveMode == WALK_AROUND_LEFT_RIGHT ) {
            return p_dirToFrame( LEFT );
        }
        if( p_moveMode == WALK_UP_DOWN || p_moveMode == WALK_AROUND_UP_DOWN
            || p_moveMode == WALK_CIRCLE ) {
            return p_dirToFrame( DOWN );
        }

        return p_dirToFrame( DOWN );
    }

    enum eventType : u8 {
        EVENT_NONE        = 0,
        EVENT_MESSAGE     = 1,
        EVENT_ITEM        = 2,
        EVENT_TRAINER     = 3,
        EVENT_OW_PKMN     = 4,
        EVENT_NPC         = 5,
        EVENT_WARP        = 6,
        EVENT_GENERIC     = 7,
        EVENT_HMOBJECT    = 8, // cut, rock smash, strength
        EVENT_BERRYTREE   = 9,
        EVENT_NPC_MESSAGE = 10,
        EVENT_FLY_POS     = 11,
    }; // namespace DATA
    const std::vector<std::string> EVENT_TYPE_NAMES{
        "No Event", "Message", "Item",      "Trainer",    "OW PKMN",     "NPC",
        "Warp",     "Generic", "HM Object", "Berry Tree", "NPC Message", "Fly Target" };

    enum eventTrigger : u8 {
        TRIGGER_NONE           = 0,
        TRIGGER_STEP_ON        = ( 1 << 0 ),
        TRIGGER_INTERACT       = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 ),
        TRIGGER_INTERACT_DOWN  = ( 1 << 1 ),
        TRIGGER_INTERACT_LEFT  = ( 1 << 2 ),
        TRIGGER_INTERACT_UP    = ( 1 << 3 ),
        TRIGGER_INTERACT_RIGHT = ( 1 << 4 ),
        TRIGGER_ON_MAP_ENTER   = ( 1 << 5 ),
    };
    const std::vector<std::string> EVENT_TRIGGER_NAMES{
        "On Step-on",        "On Interaction Down",  "On Interaction Left",
        "On Interaction Up", "On Interaction Right", "On Map Enter" };

    enum wildPkmnType : u8 {
        GRASS,
        HIGH_GRASS,
        WATER,
        OLD_ROD,
        GOOD_ROD,
        SUPER_ROD,
        HEADBUTT,
        ROCK_SMASH,
        SWEET_SCENT,
        POKE_TORE,
        SWARM,

        _TP_FIRST = GRASS,
        _TP_LAST  = SWARM
    };

    constexpr std::string wildPkmnTypeName( wildPkmnType p_type ) {
        switch( p_type ) {
        default: return "";
        case GRASS: return "Grass";
        case HIGH_GRASS: return "Long Grass";
        case WATER: return "Surf";
        case OLD_ROD: return "Old Rod";
        case GOOD_ROD: return "Good Rod";
        case SUPER_ROD: return "Super Rod";
        case HEADBUTT: return "Headbutt";
        case ROCK_SMASH: return "Rock Smash";
        case SWEET_SCENT: return "Sweet Scent";
        case POKE_TORE: return "PokéRadar";
        case SWARM: return "Swarm";
        }
    }

    enum mapWeather : u8 {
        NOTHING         = 0, // Inside
        SUNNY           = 1,
        REGULAR         = 2,
        RAINY           = 3,
        SNOW            = 4,
        THUNDERSTORM    = 5,
        MIST            = 6,
        BLIZZARD        = 7,
        SANDSTORM       = 8,
        FOG             = 9,
        DENSE_MIST      = 0xa,
        CLOUDY          = 0xb,
        HEAVY_SUNLIGHT  = 0xc,
        HEAVY_RAIN      = 0xd,
        UNDERWATER      = 0xe,
        DARK_FLASHABLE  = 0xf,
        DARK_PERMANENT  = 0x10,
        DARK_FLASH_USED = 0x11,
        FOREST_CLOUDS   = 0x12,
        ASH_RAIN        = 0x13, // route 113
        DARK_FLASH_1    = 0x14, // dewford gym defeated 1-2 trainers
        DARK_FLASH_2    = 0x15, // dewford gym defeated 3-4 trainers
    };
    enum mapType : u8 { OUTSIDE = 0, CAVE = 1, INSIDE = 2, DARK = 4, FLASHABLE = 8 };
    enum warpType : u8 {
        NO_SPECIAL,
        CAVE_ENTRY,
        DOOR,
        TELEPORT,
        EMERGE_WATER,
        LAST_VISITED,
        SLIDING_DOOR,
        FLY,
        SCRIPT
    };
    const std::vector<std::string> WARP_TYPE_NAMES{
        "Default",      "Cave Entry",   "Door",  "Teleport", "(Emerge Water)",
        "Last Visited", "Sliding Door", "(Fly)", "Script" };

    enum style : u8 {
        MSG_NORMAL          = 0,
        MSG_INFO            = 1,
        MSG_NOCLOSE         = 2, // Msgbox stays open, future calls to print append text
        MSG_INFO_NOCLOSE    = 3,
        MSG_MART_ITEM       = 4, // Message used for items / pokemart
        MSG_ITEM            = 5, // Message used when player obtains an item
        MSG_SIGN            = 6,
        MSG_NORMAL_CONT     = 7,
        MSG_INFO_CONT       = 8,
        MSG_BRAILLE         = 9,
        MSG_SHOW_MONEY_FLAG = 128, // show the current money
    };
    const std::vector<std::string> MESSAGE_TYPE_NAMES{
        "Normal",        "Information", "No-redraw",         "No-redraw Info",  "(Mart Item)",
        "(Obtain Item)", "Sign",        "Wait for Interact", "Interact (Info)", "Braille",
    };

    const std::vector<std::string> ITEM_TYPE_NAMES{ "Hidden", "Normal", "HM/TM/TR" };
    const std::vector<std::string> SCRIPT_TYPE_NAMES{ "Unrestricted", "Player is Brendan",
                                                      "Player is May" };

    constexpr u8 MAX_EVENTS_PER_SLICE = 64;
    constexpr u8 MAX_PKMN_PER_SLICE   = 30;
    struct mapData {
        u8 m_mapType;
        u8 m_weather;
        u8 m_battleBG;
        u8 m_battlePlat1;

        u8 m_battlePlat2;
        u8 m_surfBattleBG;
        u8 m_surfBattlePlat1;
        u8 m_surfBattlePlat2;

        u32 : 32;

        u16 m_locationIds[ 4 ][ 4 ]; // (y, x), 8x8 blocks each

        struct wildPkmnData {
            u16          m_speciesId;
            u8           m_forme;
            wildPkmnType m_encounterType;

            u8 m_slot; // num req badges for pkmn to show up
            u8 m_daytime;
            u8 m_encounterRate;
        } m_pokemon[ MAX_PKMN_PER_SLICE ];
        struct event {
            u8  m_posX;
            u8  m_posY;
            u8  m_posZ;
            u16 m_activateFlag;
            u16 m_deactivateFlag;
            u8  m_type;

            u8 m_trigger;
            union data {
                struct {
                    u8  m_msgType;
                    u16 m_msgId;
                } m_message;
                struct {
                    u8  m_itemType;
                    u16 m_itemId;
                } m_item;
                struct {
                    u16 m_spriteId;
                    u16 m_trainerId;

                    u8 m_movementType;
                    u8 m_sight;
                } m_trainer;
                struct {
                    u16 m_speciesId;
                    u8  m_level;
                    u8  m_forme; // BIT(6) female; BIT(7) genderless

                    u8 m_shiny; // BIT(6) hidden ability, BIT(7)    fateful
                } m_owPkmn;
                struct {
                    u16 m_spriteId;
                    u16 m_scriptId;

                    u8 m_movementType;
                    u8 m_scriptType;
                } m_npc;
                struct {
                    u8 m_warpType;
                    u8 m_bank;
                    u8 m_mapX;
                    u8 m_mapY;

                    u8 m_posX;
                    u8 m_posY;
                    u8 m_posZ; // script id if warp type is script
                } m_warp;
                struct {
                    u16 m_scriptId;
                    u8  m_scriptType;
                } m_generic;
                struct {
                    u8 m_hmType;
                } m_hmObject;
                struct {
                    u8 m_treeIdx; // internal id of this berry tree
                } m_berryTree;
                struct {
                    u16 m_location; // location idx
                } m_flyPos;
            } m_data;
        } m_events[ MAX_EVENTS_PER_SLICE ];
    };

    struct mapBank {
        std::vector<std::vector<mapSlice>> m_slices;  // [y][x]
        std::vector<std::vector<mapData>>  m_mapData; // [y][x]
    };

    constexpr u8 MAPMODE_DEFAULT   = 0;
    constexpr u8 MAPMODE_SCATTERED = 1;
    constexpr u8 MAPMODE_COMBINED  = 2;
    struct mapBankInfo {
        u8 m_sizeX   = 0;
        u8 m_sizeY   = 0;
        u8 m_mapMode = 2; // 0: normal maps/data in folder, 1: scattered in subfolders, 2: combined
        u8 m_isOWMap : 1 = false; // stores whether the map bank has location data and hence
                                  // should appear in the fsinfo owmap list
        u8  m_hasDiveMap : 1      = false; // stores whether there is a seperate map for hm dive
        u8  m_owMapResolution : 6 = 4;
        u16 m_defaultLocation     = 3002;
        u8  m_owMapShiftX         = 0;
        u8  m_owMapShiftY         = 0;

        constexpr mapBankInfo( u8 p_sizeX = 0, u8 p_sizeY = 0, u8 p_mapMode = MAPMODE_DEFAULT,
                               bool p_isOWMap = false )
            : m_sizeX{ p_sizeX }, m_sizeY{ p_sizeY }, m_mapMode{ p_mapMode }, m_isOWMap{
                                                                                  p_isOWMap } {
        }
    };

    constexpr u8 MAP_LOCATION_RES = 8;

    constexpr u8 MAX_MAP_X = 30;
    constexpr u8 MAX_MAP_Y = 20;

    constexpr u8 TILEMODE_DEFAULT  = 0;
    constexpr u8 TILEMODE_COMBINED = 1;

    void renderTile( const tile* p_tile, const palette* p_pal, bool p_flipX, bool p_flipY,
                     bitmap* p_out, u32 p_x = 0, u32 p_y = 0, u16 p_scale = 1 );
    void renderBlock( const block* p_block, const tileSet<>* p_tiles,
                      const palette p_pals[ 16 * 5 ], bitmap* p_out, u32 p_x = 0, u32 p_y = 0,
                      u16 p_scale = 1, u8 p_time = 0 );

    void renderBlock( const computedBlock* p_block, const palette p_pals[ 16 * 5 ], bitmap* p_out,
                      u32 p_x = 0, u32 p_y = 0, u16 p_scale = 1, u8 p_time = 0 );

    /*
     * @brief: renders the given mapSlice and outputs starting at the specified upper left
     * corner of the given bitmap.
     */
    void renderMapSlice( const mapSlice* p_mapSlice, const blockSet<>* p_blockSet,
                         const tileSet<>* p_tileSet, const palette p_pals[ 16 * 5 ], bitmap* p_out,
                         u32 p_x = 0, u32 p_y = 0, u16 p_scale = 1, u8 p_time = 0 );

    void renderMapSlice( const computedMapSlice* p_mapSlice, bitmap* p_out, u32 p_x = 0,
                         u32 p_y = 0, u16 p_scale = 1, u8 p_time = 0 );

    void tintRectangle( bitmap& p_out, u32 p_tx, u32 p_ty, u32 p_bx, u32 p_by, pixel p_tint,
                        u32 p_borderStrength = 0, pixel p_border = pixel( 0, 0, 0 ),
                        u8 p_borderSides = 0 << 0 | 0 << 1 | 0 << 2 | 0 << 3 );

} // namespace DATA
