#pragma once

#include <typeinfo>
#include <vector>

#include "data_bitmap.h"
#include "data_locationNames.h"
#include "defines.h"

namespace DATA {
    constexpr u16 MAX_TILES_PER_TILE_SET  = 512;
    constexpr u16 MAX_BLOCKS_PER_TILE_SET = 512;
    constexpr u8  MAX_MOVEMENTS           = 64;

    constexpr u16 BLOCK_SIZE = 16;

    constexpr u16 SIZE          = 32;
    constexpr s16 dir[ 4 ][ 2 ] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    struct palette {
        u16 m_pal[ 16 ];

        void dump( ) const;
    };

    constexpr palette DEFAULT_PALETTE = { { 0 } };

    struct tile {
        u8 m_tile[ 32 ];

        void dump( ) const;
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
        computedBlockAtom m_top[ 2 ][ 2 ] = { computedBlockAtom( ) };
        u8                m_topbehave     = 0;

        computedBlockAtom m_bottom[ 2 ][ 2 ] = { computedBlockAtom( ) };
        u8                m_bottombehave     = 0;
    };

    class block {
      public:
        blockAtom m_top[ 2 ][ 2 ] = { blockAtom( ) };
        u8        m_topbehave     = 0;

        blockAtom m_bottom[ 2 ][ 2 ] = { blockAtom( ) };
        u8        m_bottombehave     = 0;

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

    struct mapSlice {
        mapBlockAtom m_blocks[ SIZE ][ SIZE ] = { { mapBlockAtom( ) } }; // [ y ][ x ]
        u8           m_map                    = 0;
        u16          m_x = 0, m_y = 0;
        u8           m_tIdx1 = 0, m_tIdx2 = 0;

        inline std::vector<std::pair<computedBlock, u8>> compute( const blockSet<2>* p_blocks,
                                                                  const tileSet<2>*  p_tiles ) {
            auto res = std::vector<std::pair<computedBlock, u8>>( );
            for( u8 y = 0; y < SIZE; ++y ) {
                for( u8 x = 0; x < SIZE; ++x ) {
                    auto bidx = m_blocks[ y ][ x ].m_blockidx;
                    res.push_back( { p_blocks->m_blocks[ bidx ].compute( p_tiles ),
                                     u8( m_blocks[ y ][ x ].m_movedata ) } );
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
    };
    enum direction : u8 { UP, RIGHT, DOWN, LEFT };

    struct movement {
        direction m_direction;
        u8        m_frame;
    };

    typedef std::pair<u8, position> warpPos;
    enum moveMode {
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
        NO_MOVEMENT            = 0,
        LOOK_UP                = 1,
        LOOK_DOWN              = 2,
        LOOK_RIGHT             = 4,
        LOOK_LEFT              = 8,
        WALK_LEFT_RIGHT        = 16,
        WALK_UP_DOWN           = 17,
        WALK_CIRCLE            = 18,
        WALK_AROUND_LEFT_RIGHT = 19,
        WALK_AROUND_UP_DOWN    = 20,
    };
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
    };
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
    };
    enum mapWeather : u8 {
        NOTHING        = 0, // Inside
        SUNNY          = 1,
        REGULAR        = 2,
        RAINY          = 3,
        SNOW           = 4,
        THUNDERSTORM   = 5,
        MIST           = 6,
        BLIZZARD       = 7,
        SANDSTORM      = 8,
        FOG            = 9,
        DENSE_MIST     = 0xa,
        CLOUDY         = 0xb, // Dark Forest clouds
        HEAVY_SUNLIGHT = 0xc,
        HEAVY_RAIN     = 0xd,
        UNDERWATER     = 0xe
    };
    enum mapType : u8 { OUTSIDE = 0, CAVE = 1, INSIDE = 2, DARK = 4, FLASHABLE = 8 };
    enum warpType : u8 {
        NO_SPECIAL,
        CAVE_ENTRY,
        DOOR,
        TELEPORT,
        EMERGE_WATER,
        LAST_VISITED,
        SLIDING_DOOR
    };

    constexpr u8 MAX_EVENTS_PER_SLICE = 64;
    struct mapData {
        mapType    m_mapType;
        mapWeather m_weather;
        u8         m_battleBG;
        u8         m_battlePlat1;
        u8         m_battlePlat2;
        u8         m_surfBattleBG;
        u8         m_surfBattlePlat1;
        u8         m_surfBattlePlat2;

        u16 m_locationIds[ 4 ][ 4 ]; // (y, x), 8x8 blocks each

        u8 m_pokemonDescrCount;
        struct wildPkmnData {
            u16          m_speciesId;
            u8           m_forme;
            wildPkmnType m_encounterType;
            u8           m_slot;
            u8           m_daytime;
            u8           m_encounterRate;
        } m_pokemon[ 30 ];
        u8 m_eventCount;
        struct event {
            u8           m_posX;
            u8           m_posY;
            u8           m_posZ;
            u16          m_activateFlag;
            u16          m_deactivateFlag;
            eventType    m_type;
            eventTrigger m_trigger;
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

                    u8 m_shiny; // BIT(6) hidden ability, BIT(7) fateful
                } m_owPkmn;
                struct {
                    u16 m_spriteId;
                    u16 m_scriptId;

                    u8 m_movementType;
                    u8 m_scriptType;
                } m_npc;
                struct {
                    warpType m_warpType;
                    u8       m_bank;
                    u8       m_mapY;
                    u8       m_mapX;

                    u8 m_posX;
                    u8 m_posY;
                    u8 m_posZ;
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
        u8 m_mapMode = 0; // 0: normal maps/data in folder, 1: scattered in subfolders, 2: combined
        u8 : 8;
        u32 : 32;

        constexpr mapBankInfo( u8 p_sizeX = 0, u8 p_sizeY = 0, u8 p_mapMode = MAPMODE_DEFAULT )
            : m_sizeX( p_sizeX ), m_sizeY( p_sizeY ), m_mapMode( p_mapMode ) {
        }
    };

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

    constexpr pixel colorForLocation( u16 p_loc ) {
        auto routecol    = pixel( 230, 200, 30, 100 );
        auto searoutecol = pixel( 7, 211, 255, 100 );
        auto citycol     = pixel( 255, 68, 119, 100 );
        auto specialcol  = pixel( 0, 255, 0, 100 );

        switch( p_loc ) {
        case L_METEOR_FALLS:
        case L_RUSTURF_TUNNEL:
        case L_DESERT_RUINS:
        case L_GRANITE_CAVE:
        case L_PETALBURG_WOODS:
        case L_MT_CHIMNEY:
        case L_JAGGED_PASS:
        case L_FIERY_PATH:
        case L_MT_PYRE:
        case L_TEAM_AQUA_HIDEOUT:
        case L_SEAFLOOR_CAVERN:
        case L_CAVE_OF_ORIGIN:
        case L_VICTORY_ROAD3:
        case L_SHOAL_CAVE:
        case L_NEW_MAUVILLE:
        case L_SEA_MAUVILLE:
        case L_ISLAND_CAVE:
        case L_ANCIENT_TOMB:
        case L_SEALED_CHAMBER:
        case L_SCORCHED_SLAB:
        case L_TEAM_MAGMA_HIDEOUT:
        case L_SKY_PILLAR:
        case L_BATTLE_FRONTIER2:
        case L_SOUTHERN_ISLAND:
        case L_SS_TIDAL:
        case L_SAFARI_ZONE2:
        case L_MIRAGE_FOREST:
        case L_MIRAGE_CAVE:
        case L_MIRAGE_ISLAND:
        case L_MIRAGE_MOUNTAIN:
        case L_TRACKLESS_FOREST:
        case L_PATHLESS_PLAIN:
        case L_NAMELESS_CAVERN:
        case L_FABLED_CAVE:
        case L_GNARLED_DEN:
        case L_CRESCENT_ISLE:
        case L_SECRET_ISLET:
        case L_SECRET_SHORE:
        case L_SECRET_MEADOW:
        case L_HIDDEN_LAKE:
        case L_POKEMON_LEAGUE2:
        case L_CRYSTAL_CAVERN: return specialcol;
        case L_LITTLEROOT_TOWN:
        case L_OLDALE_TOWN:
        case L_DEWFORD_TOWN:
        case L_LAVARIDGE_TOWN:
        case L_FALLARBOR_TOWN:
        case L_VERDANTURF_TOWN:
        case L_PACIFIDLOG_TOWN:
        case L_PETALBURG_CITY:
        case L_SLATEPORT_CITY:
        case L_MAUVILLE_CITY:
        case L_RUSTBORO_CITY:
        case L_FORTREE_CITY:
        case L_LILYCOVE_CITY:
        case L_MOSSDEEP_CITY:
        case L_SOOTOPOLIS_CITY:
        case L_EVER_GRANDE_CITY:
        case L_NEW_LILYCOVE_CITY:
        case L_BLOSSAND_TOWN:
        case L_CLIFFELTA_CITY: return citycol;
        case L_ROUTE_105:
        case L_ROUTE_106:
        case L_ROUTE_107:
        case L_ROUTE_108:
        case L_ROUTE_109:
        case L_ROUTE_122:
        case L_ROUTE_124:
        case L_ROUTE_125:
        case L_ROUTE_126:
        case L_ROUTE_127:
        case L_ROUTE_128:
        case L_ROUTE_129:
        case L_ROUTE_130:
        case L_ROUTE_131:
        case L_ROUTE_132:
        case L_ROUTE_133:
        case L_ROUTE_134:
        case L_ROUTE_137: return searoutecol;
        default: return routecol;
        }
    }
} // namespace DATA
