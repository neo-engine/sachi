#pragma once
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <gtkmm/stringlist.h>

#include "data/bitmap.h"
#include "data/fs.h"
#include "data/maprender.h"

enum status { STATUS_UNTOUCHED, STATUS_NEW, STATUS_EDITED_UNSAVED, STATUS_SAVED };

constexpr u16 OTLENGTH              = 8;
constexpr u16 SPECIES_NAMELENGTH    = 30;
constexpr u16 PKMN_NAMELENGTH       = 14;
constexpr u16 FORME_NAMELENGTH      = 30;
constexpr u16 ITEM_NAMELENGTH       = 20;
constexpr u16 ITEM_DSCRLENGTH       = 200;
constexpr u16 MOVE_NAMELENGTH       = 20;
constexpr u16 MOVE_DSCRLENGTH       = 200;
constexpr u16 ABILITY_NAMELENGTH    = 20;
constexpr u16 TCLASS_NAMELENGTH     = 30;
constexpr u16 ABILITY_DSCRLENGTH    = 200;
constexpr u16 LOCATION_NAMELENGTH   = 25;
constexpr u16 BGM_NAMELENGTH        = 25;
constexpr u16 DEXENTRY_NAMELENGTH   = 200;
constexpr u16 TRAINER_NAMELENGTH    = 16;
constexpr u16 TRAINER_MESSAGELENGTH = 200;
constexpr u16 TRAINER_CLASSLENGTH   = 30;

constexpr u16 UISTRING_LEN    = 250;
constexpr u16 MAPSTRING_LEN   = 800;
constexpr u16 BADGENAME_LEN   = 50;
constexpr u16 ACHIEVEMENT_LEN = 100;
constexpr u16 PKMNPHRS_LEN    = 150;

constexpr u16 LEARN_TM      = 200;
constexpr u16 LEARN_TUTOR   = 201;
constexpr u16 LEARN_EGG     = 202;
constexpr u16 LEARNSET_SIZE = 2 * 400;

struct model {
    struct fsdata {
        struct mapBankContainer {
            DATA::mapBank                                    m_bank;
            std::vector<std::vector<DATA::computedMapSlice>> m_computedBank;

            bool              m_loaded = false;
            DATA::mapBankInfo m_info{ };
            status            m_status = STATUS_UNTOUCHED;

            u8           m_mapImageRes       = 4;
            u8           m_mapImageShiftX    = 4;
            u8           m_mapImageShiftY    = 36;
            u8           m_wildPokeMapShiftX = 0;
            u8           m_wildPokeMapShiftY = 0;
            DATA::bitmap m_owMap{ 0, 0 };
            DATA::bitmap m_wpMap{ 0, 0 };

            inline status getStatus( ) const {
                return m_status;
            }
            inline u8 getSizeX( ) const {
                return m_info.m_sizeX;
            }
            inline u8 getSizeY( ) const {
                return m_info.m_sizeY;
            }
            inline bool isScattered( ) const {
                return m_info.m_mapMode == DATA::MAPMODE_SCATTERED;
            }
            inline bool isCombined( ) const {
                return m_info.m_mapMode == DATA::MAPMODE_COMBINED;
            }
            inline u8 getMapMode( ) const {
                return m_info.m_mapMode;
            }
            inline u8 getOWStatus( ) const {
                return m_info.m_isOWMap;
            }
            inline u8 getDiveStatus( ) const {
                return m_info.m_hasDiveMap;
            }
            inline u16 getDefaultLocation( ) const {
                return m_info.m_defaultLocation;
            }

            inline void setStatus( status p_status ) {
                m_status = p_status;
            }
            inline void setSizeX( u8 p_newSize ) {
                m_info.m_sizeX = p_newSize;
            }
            inline void setSizeY( u8 p_newSize ) {
                m_info.m_sizeY = p_newSize;
            }
            inline void setScattered( bool p_scattered = true ) {
                m_info.m_mapMode = p_scattered;
            }
            inline void setCombined( ) {
                m_info.m_mapMode = DATA::MAPMODE_COMBINED;
            }
            inline void setMapMode( u8 p_mode ) {
                m_info.m_mapMode = p_mode;
            }
            inline void setOWStatus( u8 p_owStatus ) {
                m_info.m_isOWMap = p_owStatus;
            }
            inline void setDiveStatus( u8 p_owStatus ) {
                m_info.m_hasDiveMap = p_owStatus;
            }
            inline void setDefaultLocation( u16 p_location ) {
                m_info.m_defaultLocation = p_location;
            }
        };

        struct blockSetInfo {
            DATA::blockSet<1> m_blockSet;
            DATA::tileSet<1>  m_tileSet;
            DATA::palette     m_pals[ 8 * DAYTIMES ];
            u8                m_stringListItem = 0;
        };

        struct trainerDataInfo {
            DATA::trainerData m_trainer[ 3 ] = { DATA::trainerData{ } };
            bool              m_active[ 3 ]  = { false };

            inline bool exists( u8 p_difficulty = 1 ) const {
                if( p_difficulty == 1 ) { return true; }
                return m_active[ p_difficulty ];
            }

            inline const auto& data( u8 p_difficulty = 1 ) const {
                return m_trainer[ p_difficulty ];
            }

            inline auto& data( u8 p_difficulty = 1 ) {
                return m_trainer[ p_difficulty ];
            }
        };

        std::string                     m_fsrootPath;
        std::map<u16, mapBankContainer> m_mapBanks;

        u8     m_tileSetMode = DATA::TILEMODE_DEFAULT;
        status m_tileStatus = STATUS_UNTOUCHED, m_trainerStatus = STATUS_UNTOUCHED;
        std::vector<Glib::ustring> m_mapBankStrList; // block set names
        std::map<u8, blockSetInfo> m_blockSets;
        std::set<u8>               m_blockSetNames;

        std::vector<trainerDataInfo> m_trainer;

        DATA::fsdataInfo m_fsInfo;

        DATA::pkmnData getPkmnData( const u16 p_pkmnId, const u8 p_forme = 0 );
        bool           getPkmnData( const u16 p_pkmnId, DATA::pkmnData* p_out );
        bool           getPkmnData( const u16 p_pkmnId, const u8 p_forme, DATA::pkmnData* p_out );

        bool getLearnset( u16 p_pkmnId, u8 p_forme, u16* p_out );
        void getLearnMoves( u16 p_pkmnId, u8 p_forme, u16 p_fromLevel, u16 p_toLevel, u16 p_num,
                            u16* p_res );
        bool canLearn( u16 p_pkmnId, u8 p_forme, u16 p_moveId, u16 p_maxLevel, u16 p_minLevel = 0 );
        bool canLearn( const u16* p_learnset, u16 p_moveId, u16 p_maxLevel, u16 p_minLevel = 0 );
        const u16* getLearnset( u16 p_pkmnId, u8 p_forme );

        inline std::string fsinfoPath( ) const {
            return m_fsrootPath + "/fsinfo";
        }

        inline std::string mapPath( ) const {
            return m_fsrootPath + "/MAPS/";
        }

        inline std::string pkmnDataPath( ) const {
            return m_fsrootPath + "/DATA/pkmn.datab";
        }

        inline std::string formeDataPath( ) const {
            return m_fsrootPath + "/DATA/pkmnf.datab";
        }
        inline std::string learnsetPath( ) const {
            return m_fsrootPath + "/DATA/pkmn.learnset.datab";
        }

        inline std::string formeLearnsetPath( ) const {
            return m_fsrootPath + "/DATA/pkmnf.learnset.datab";
        }

        inline std::string owMapPicturePath( ) const {
            return m_fsrootPath + "/PICS/NAV_APP/map";
        }

        inline std::string wpMapPicturePath( ) const {
            return m_fsrootPath + "/PICS/DEX/map";
        }

        inline std::string navBorderPath( ) const {
            return m_fsrootPath + "/PICS/Border.raw";
        }

        inline std::string dexBorderPath( ) const {
            return m_fsrootPath + "/PICS/DEX/dextop2.raw";
        }

        inline std::string mapStringPath( ) const {
            return m_fsrootPath + "/STRN/MAP/map";
        }

        inline std::string uiStringPath( ) const {
            return m_fsrootPath + "/STRN/UIS/uis";
        }

        inline std::string trainerClassPath( ) const {
            return m_fsrootPath + "/DATA/TRNR_NAME/trnrname";
        }

        inline std::string trainerNamePath( ) const {
            return m_fsrootPath + "/STRN/TRN/name";
        }

        inline std::string trainerMessagePath( u8 p_message ) const {
            return m_fsrootPath + "/STRN/TRN/msg" + std::to_string( p_message + 1 );
        }

        inline std::string mapLocationPath( ) const {
            return m_fsrootPath + "/DATA/MAP_LOCATION/";
        }

        inline std::string tilesetPath( ) const {
            return m_fsrootPath + "/MAPS/TILESETS/";
        }

        inline std::string blocksetPath( ) const {
            return m_fsrootPath + "/MAPS/BLOCKESTS/";
        }

        inline std::string palettePath( ) const {
            return m_fsrootPath + "/MAPS/PALETTES/";
        }

        inline std::string mapDataPath( ) const {
            return m_fsrootPath + "/DATA/MAP_DATA/";
        }

        inline std::string battleBGPath( ) const {
            return m_fsrootPath + "/PICS/BATTLE_BACK/";
        }

        inline std::string battlePlatPath( ) const {
            return m_fsrootPath + "/PICS/SPRITES/PLAT/plat";
        }

        inline std::string itemSpritePath( ) const {
            return m_fsrootPath + "/PICS/SPRITES/item.icon.rawb";
        }

        inline std::string trainerSpritePath( u8 p_trainerIdx ) const {
            return m_fsrootPath + "/PICS/SPRITES/TRAINER/"
                   + std::to_string( p_trainerIdx / m_fsInfo.m_fileSplit ) + "/";
        }

        inline std::string trainerDataPath( u8 p_difficulty = 1 ) const {
            return m_fsrootPath + "/DATA/TRNR_DATA/" + std::to_string( p_difficulty ) + "/";
        }

        inline size_t trainerCount( ) const {
            return m_trainer.size( );
        }

        inline void createTrainer( u16 p_trainerId ) {
            if( p_trainerId >= trainerCount( ) ) { m_trainer.resize( p_trainerId + 1 ); }
        }

        inline std::string owSpritePath( u16 p_spriteIdx, u8 p_forme = 0, bool p_shiny = false,
                                         bool p_female = false ) const {
            if( p_spriteIdx > DATA::PKMN_SPRITE ) {
                u16  species = p_spriteIdx - DATA::PKMN_SPRITE;
                u8   forme   = p_forme;
                bool shiny   = p_shiny;
                bool female  = p_female;

                char buf[ 100 ];
                if( !forme ) {
                    snprintf( buf, 99, "%d/%hu%s%s", species / m_fsInfo.m_fileSplit, species,
                              female ? "f" : "", shiny ? "s" : "" );
                } else {
                    snprintf( buf, 99, "%d/%hu_%hhu%s%s", species / m_fsInfo.m_fileSplit, species,
                              forme, female ? "f" : "", shiny ? "s" : "" );
                }
                return m_fsrootPath + "/PICS/SPRITES/NPCP/" + std::string( buf ) + ".rsd";
            } else if( p_spriteIdx < 250 ) {
                // player sprite
                return m_fsrootPath + "/PICS/SPRITES/OW/" + std::to_string( p_spriteIdx ) + ".rsd";
            }
            if( p_spriteIdx == 250 ) {
                // load player appearance
                p_spriteIdx = 0;
            } else if( p_spriteIdx == 251 ) {
                // load rival's appearance
                p_spriteIdx = 1;
            } else {
                p_spriteIdx &= 255;
            }

            return std::string( "@" ) + std::to_string( p_spriteIdx ) + "@" + m_fsrootPath
                   + "/PICS/SPRITES/npc.rsdb";
        }

        inline std::string pkmnSpritePath( bool p_female = false, bool p_shiny = false,
                                           std::string p_type = "frnt" ) const {
            auto res = m_fsrootPath + "/PICS/SPRITES/" + p_type;
            if( p_female ) { res += "f"; }
            if( p_shiny ) { res += "s"; }

            return res + ".pkmn.sprb";
        }

        inline std::string pkmnFormePath( u16 p_speciesId, u8 p_forme, bool p_female = false,
                                          bool        p_shiny = false,
                                          std::string p_type  = "frnt" ) const {
            auto res = m_fsrootPath + "/PICS/SPRITES/" + p_type + "/";
            res += std::to_string( p_speciesId / m_fsInfo.m_fileSplit ) + "/";
            if( p_forme ) {
                res += std::to_string( p_speciesId ) + "_" + std::to_string( p_forme );
            }

            if( p_female ) { res += "f"; }
            if( p_shiny ) { res += "s"; }

            return res + ".raw";
        }

        inline std::string pkmnNamePath( ) const {
            return m_fsrootPath + "/DATA/PKMN_NAME/pkmnname";
        }

        inline std::string locationNamePath( ) const {
            return m_fsrootPath + "/DATA/LOC_NAME/locname";
        }

        inline std::string locationDataPath( ) const {
            return m_fsrootPath + "/DATA/location.datab";
        }

        inline std::string itemNamePath( ) const {
            return m_fsrootPath + "/DATA/ITEM_NAME/itemname";
        }

        inline std::string itemDscrPath( ) const {
            return m_fsrootPath + "/DATA/ITEM_DSCR/itemdscr";
        }

        inline std::string moveNamePath( ) const {
            return m_fsrootPath + "/DATA/MOVE_NAME/movename";
        }

        inline std::string abilityNamePath( ) const {
            return m_fsrootPath + "/DATA/ABTY_NAME/abtyname";
        }
    };
    struct settings {
        int m_selectedBank = -1;
        s16 m_selectedMapX = -1, m_selectedMapY = -1;

        bool m_focusMode = false;

        u8   m_currentDayTime  = 0;
        u8   m_blockScale      = 1;
        u8   m_blockSpacing    = 0;
        u8   m_neighborSpacing = 10;
        bool m_showAdjacent    = true;
        u16  m_blockSetWidth   = 8;
        u8   m_adjacentBlocks  = 8;

        u8   m_bankOverviewScale   = 3;
        u8   m_bankOverviewSpacing = 2;
        u8   m_bankOverviewDayTime = 0;
        bool m_overviewNeedsRedraw = false;

        u8 m_selectedEvent = 0;

        DATA::mapBlockAtom m_currentlySelectedBlock = DATA::mapBlockAtom( );

        std::string m_windowTitle, m_mainTitle, m_subTitle;

        u8   m_tseBS1 = 0, m_tseBS2 = 1;
        u16  m_tseSelectedBlock   = 0;
        u16  m_tseSelectedTile    = 0;
        u16  m_tseSelectedTileIdx = 0;
        u8   m_tseSelectedPalette = 0;
        u8   m_tseSelectedPalIdx  = 0;
        bool m_tseFlipX           = false;
        bool m_tseFlipY           = false;
        u8   m_tseScale           = 2;
        u8   m_tseSpacing         = 0;
        u8   m_tseDayTime         = 0;
        u16  m_tseBlockSetWidth   = 8;
        u16  m_tseTileSetWidth    = 16;
        bool m_tseTileOverlay     = true;

        u16 m_treId  = 0;
        u8  m_treDif = 1;

        u16 m_pkmnDataEId = 1;
        u16 m_itemDataEId = 1;
        u16 m_moveDataEId = 1;
    };

    struct stringCache {
        u16                      m_lastRefresh = 0;
        bool                     m_valid       = false;
        std::vector<std::string> m_strings;
    };

    fsdata   m_fsdata;
    settings m_settings;
    bool     m_good = false;

    bool m_needsRefresh = true;

    stringCache m_pkmnNameCache;
    stringCache m_locationNameCache;
    stringCache m_itemNameCache;
    stringCache m_moveNameCache;
    stringCache m_abilityNameCache;
    stringCache m_trainerClassCache;
    stringCache m_trainerNameCache;
    stringCache m_trainerMessageCache[ 3 ];

    /*
     * @brief: sets the ow status of the current map and correspondingly adds/removes
     * the bank from the owbank list
     * @returns: true on success.
     */
    bool setCurrentBankOWStatus( u8 p_owStatus );

    inline void invalidateCaches( ) {
        m_needsRefresh                     = true;
        m_pkmnNameCache.m_valid            = false;
        m_locationNameCache.m_valid        = false;
        m_itemNameCache.m_valid            = false;
        m_moveNameCache.m_valid            = false;
        m_abilityNameCache.m_valid         = false;
        m_trainerNameCache.m_valid         = false;
        m_trainerMessageCache[ 0 ].m_valid = false;
        m_trainerMessageCache[ 1 ].m_valid = false;
        m_trainerMessageCache[ 2 ].m_valid = false;
        m_trainerClassCache.m_valid        = false;
    }

    inline u16 maxItem( ) const {
        return m_fsdata.m_fsInfo.m_maxItem;
    }

    inline u16 maxPkmn( ) const {
        return m_fsdata.m_fsInfo.m_maxPkmn;
    }

    inline bool needsRefresh( ) const {
        return m_needsRefresh;
    }

    inline void refresh( ) {
        if( !m_needsRefresh ) { return; }

        pkmnNames( );
        locationNames( );
        itemNames( );
        moveNames( );
        abilityNames( );
        trainerNames( );
        for( u8 i = 0; i < 3; ++i ) { trainerMessage( i ); }
        trainerClasses( );

        m_needsRefresh = false;
    }

    /*
     * @brief: reads the names of all pkmn from the fsroot and stores them in a string
     * cache, which is returned.
     */
    const stringCache& pkmnNames( );

    /*
     * @brief: reads the names of all locations from the fsroot and stores them in a string
     * cache, which is returned.
     */
    const stringCache& locationNames( );

    /*
     * @brief: reads the names of all items from the fsroot and stores them in a string
     * cache, which is returned.
     */
    const stringCache& itemNames( );

    const stringCache& moveNames( );

    const stringCache& abilityNames( );

    const stringCache& trainerNames( );

    const stringCache& trainerMessage( u8 p_messageId );

    const stringCache& trainerClasses( );

    std::string getMapString( u16 p_stringId, u8 p_language = 0 );

    /*
     * @returns: true on error.
     */
    bool readOrCreateFsInfo( );

    /*
     * @returns: true on error.
     */
    bool writeFsInfo( );

    /*
     * @returns: true on error.
     */
    bool writeFsRoot( );

    static model readFromPath( const std::string& p_path );

    void addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, u8 p_mapMode = DATA::MAPMODE_COMBINED,
                        status p_status = STATUS_UNTOUCHED );

    inline auto selectedBankIsDive( ) const {
        return m_settings.m_selectedBank > DIVE_MAP
               && m_settings.m_selectedBank <= DIVE_MAP + MAX_MAPBANK_NAME;
    }

    inline auto selectedBank( ) const {
        return m_settings.m_selectedBank;
    }

    inline void selectBank( int p_newSelection ) {
        m_settings.m_selectedBank = p_newSelection;
    }
    inline auto selectedTrainerId( ) const {
        return m_settings.m_treId;
    }

    inline void selectTrainer( int p_newSelection ) {
        m_settings.m_treId = p_newSelection;
    }

    inline auto selectedTrainerDifficulty( ) const {
        return m_settings.m_treDif;
    }

    inline void selectTrainerDifficulty( u8 p_newSelection ) {
        m_settings.m_treDif = p_newSelection;
    }

    inline const auto& selectedTrainerInfo( ) const {
        if( selectedTrainerId( ) < m_fsdata.m_trainer.size( ) ) {
            return m_fsdata.m_trainer[ selectedTrainerId( ) ];
        } else {
            static const auto _default = fsdata::trainerDataInfo{ };
            return _default;
        }
    }

    inline auto& selectedTrainerInfo( ) {
        if( selectedTrainerId( ) >= m_fsdata.m_trainer.size( ) ) {
            m_fsdata.createTrainer( selectedTrainerId( ) );
        }
        return m_fsdata.m_trainer[ selectedTrainerId( ) ];
    }

    inline const auto& selectedTrainer( ) const {
        return selectedTrainerInfo( ).data( selectedTrainerDifficulty( ) );
    }

    inline auto& selectedTrainer( ) {
        return selectedTrainerInfo( ).data( selectedTrainerDifficulty( ) );
    }

    void recomputeDNS( u8 p_tsIdx, bool p_override = true );

    void setTileSet( u8 p_tsIdx, u8 p_newTileSet );

    /*
     * @brief: Creates a new block/tile/palette set with the specified index.
     */
    void createBlockSet( u8 p_tsIdx );

    /*
     * @brief: Merges the two currently visible block sets into a single block set used
     * for rendering maps.
     */
    void buildBlockSet( DATA::blockSet<2>* p_out, s8 p_ts1 = -1, s8 p_ts2 = -1 );

    /*
     * @brief: Merges the two currently visible tile sets into a single tile set used
     * for rendering maps.
     */
    void buildTileSet( DATA::tileSet<2>* p_out, s8 p_ts1 = -1, s8 p_ts2 = -1 );

    /*
     * @brief: Merges the two currently visible palette sets into a single palette used
     * for rendering maps.
     */
    void buildPalette( DATA::palette p_out[ 5 * 16 ], s8 p_ts1 = -1, s8 p_ts2 = -1 );

    /*
     * @brief: Reads all tile / block / palette sets from disk; first looks for a
     * combined tile set bank ("MAP_PATH/tileset.tsb"), then checks the directories
     * "TILESET_PATH", "BLOCKSET_PATH" and "PALETTE_PATH".
     */
    bool readTileSets( );

    /*
     * @brief: Writes all tile / block / palette sets to disk.
     */
    bool writeTileSets( );

    bool readTrainers( );

    bool writeTrainers( );

    /*
     * @brief: Reads blocks for tile set p_bsId from file p_path.
     */
    bool readBlockSet( u16 p_bsId, std::string p_path );

    /*
     * @brief: Writes blocks of tile set p_bsId to file p_path.
     */
    bool writeBlockSet( u16 p_bsId, std::string p_path );

    /*
     * @brief: Reads tiles for tile set p_bsId from file p_path.
     */
    bool readTileSet( u16 p_bsId, std::string p_path );

    /*
     * @brief: Writes tiles of tile set p_bsId to file p_path.
     */
    bool writeTileSet( u16 p_bsId, std::string p_path );

    /*
     * @brief: Writes the specified map slice to the FS.
     * @param p_path: If specified, the map is stored at the specified path, otherwise
     * it will be stored at the default location (FSROOT/MAPS/p_bank[/p_mapY]/p_mapY_p_mapX.map)
     * in the current working directory.
     */
    bool writeMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path = "",
                        bool p_writeMapData = true );

    /*
     * @brief: Writes all map slices of the specified map bank to the FS.
     */
    bool writeMapBank( u16 p_bank );

    /*
     * @brief: Reads the specified map slice from the FS.
     * @param p_path: If specified, the map is read from the specified path, otherwise
     * it will be read from the default location (FSROOT/MAPS/p_bank[/p_mapY]/p_mapY_p_mapX.map)
     * in the current working directory.
     */
    bool readMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path = "",
                       bool p_readMapData = true );

    /*
     * @brief: Read a (large) map from the specified file, slice it and insert it at the
     * specified position (might cover multiple slices)
     */
    bool readLargeMap( u16 p_bank, u8 p_mapX, u8 p_mapY, u8 p_insertX, u8 p_insertY,
                       std::string p_path );

    /*
     * @brief: Reads all map slices of the specified map bank from the FS.
     * @param p_forceReread: If true, ignores and overwrites any previously read data.
     */
    bool checkOrLoadBank( int p_bank, bool p_forceRead );

    inline auto selectedMapEvent( ) const {
        return m_settings.m_selectedEvent;
    }

    inline void selectEvent( u8 p_event ) {
        m_settings.m_selectedEvent = p_event;
    }

    inline auto selectedMapX( ) const {
        return m_settings.m_selectedMapX;
    }

    inline auto selectedMapY( ) const {
        return m_settings.m_selectedMapY;
    }

    void selectMap( s16 p_mapX, s16 p_mapY );

    inline auto selectedSizeX( ) const {
        return m_fsdata.m_mapBanks.at( selectedBank( ) ).getSizeX( );
    }

    inline auto selectedSizeY( ) const {
        return m_fsdata.m_mapBanks.at( selectedBank( ) ).getSizeY( );
    }

    inline auto sizeX( u16 p_bank ) const {
        return m_fsdata.m_mapBanks.at( p_bank ).getSizeX( );
    }

    inline auto sizeY( u16 p_bank ) const {
        return m_fsdata.m_mapBanks.at( p_bank ).getSizeY( );
    }

    inline auto bankStatus( u16 p_bank ) const {
        return m_fsdata.m_mapBanks.at( p_bank ).getStatus( );
    }

    inline void selectedSetSizeX( u8 p_sx ) {
        m_fsdata.m_mapBanks[ selectedBank( ) ].setSizeX( p_sx );
    }

    inline void selectedSetSizeY( u8 p_sy ) {
        m_fsdata.m_mapBanks[ selectedBank( ) ].setSizeY( p_sy );
    }

    inline bool existsBank( u16 p_bank ) {
        return m_fsdata.m_mapBanks.count( p_bank );
    }

    inline auto& bank( u16 p_bank ) {
        return m_fsdata.m_mapBanks[ p_bank ];
    }

    inline auto& bank( ) {
        return bank( selectedBank( ) );
    }

    inline const auto& bank( u16 p_bank ) const {
        return m_fsdata.m_mapBanks.at( p_bank );
    }

    inline const auto& bank( ) const {
        return bank( selectedBank( ) );
    }

    inline auto& computedSlice( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        return bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX ];
    }

    inline auto& computedSlice( ) {
        return computedSlice( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    inline const auto& computedSlice( u16 p_bank, u8 p_mapY, u8 p_mapX ) const {
        return bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX ];
    }

    inline const auto& computedSlice( ) const {
        return computedSlice( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    inline auto& slice( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        return bank( p_bank ).m_bank.m_slices[ p_mapY ][ p_mapX ];
    }

    inline auto& slice( ) {
        return slice( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    inline const auto& slice( u16 p_bank, u8 p_mapY, u8 p_mapX ) const {
        return bank( p_bank ).m_bank.m_slices[ p_mapY ][ p_mapX ];
    }

    inline const auto& slice( ) const {
        return slice( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    inline auto& mapData( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        return bank( p_bank ).m_bank.m_mapData[ p_mapY ][ p_mapX ];
    }

    inline auto& mapData( ) {
        return mapData( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    inline const auto& mapData( u16 p_bank, u8 p_mapY, u8 p_mapX ) const {
        return bank( p_bank ).m_bank.m_mapData[ p_mapY ][ p_mapX ];
    }

    inline const auto& mapData( ) const {
        return mapData( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    inline auto& mapEvent( ) {
        return mapData( ).m_events[ selectedMapEvent( ) ];
    }

    inline const auto& mapEvent( ) const {
        return mapData( ).m_events[ selectedMapEvent( ) ];
    }

    inline DATA::warpPos mapEventPosition( ) const {
        auto evt = mapEvent( );
        return { (u8) selectedBank( ),
                 DATA::position::fromLocal( selectedMapX( ), evt.m_posX, selectedMapY( ),
                                            evt.m_posY, evt.m_posZ ) };
    }

    inline void copySelectedMapTo( u16 p_mapY, u16 p_mapX ) {
        markSelectedBankChanged( );
        slice( selectedBank( ), p_mapY, p_mapX )         = slice( );
        computedSlice( selectedBank( ), p_mapY, p_mapX ) = computedSlice( );
        mapData( selectedBank( ), p_mapY, p_mapX )       = mapData( );
    }

    inline void copySelectedMapMetaTo( u16 p_mapY, u16 p_mapX ) {
        markSelectedBankChanged( );
        auto tmp{ mapData( selectedBank( ), p_mapY, p_mapX ) };
        mapData( selectedBank( ), p_mapY, p_mapX ) = mapData( );

        for( u8 i{ 0 }; i < DATA::MAX_EVENTS_PER_SLICE; ++i ) {
            mapData( selectedBank( ), p_mapY, p_mapX ).m_events[ i ] = tmp.m_events[ i ];
        }
    }

    /*
     * @brief: Sets the status of the edit tile set widget to the specified status,
     * resulting in the TS being highlighted in the sidebar.
     */
    inline void markTileSetsChanged( status p_newStatus = STATUS_EDITED_UNSAVED ) {
        m_fsdata.m_tileStatus = p_newStatus;
    }

    inline void markTrainersChanged( status p_newStatus = STATUS_EDITED_UNSAVED ) {
        m_fsdata.m_trainerStatus = p_newStatus;
    }

    /*
     * @brief: Sets the status of the specified map bank to the specified status,
     * resulting in the specified map bank being highlighted in the sidebar.
     */
    inline void markBankChanged( u16 p_bank, status p_newStatus = STATUS_EDITED_UNSAVED ) {
        bank( p_bank ).m_status = p_newStatus;
    }

    inline void markSelectedBankChanged( status p_newStatus = STATUS_EDITED_UNSAVED ) {
        bank( ).m_status = p_newStatus;
    }

    inline status tileStatus( ) const {
        return m_fsdata.m_tileStatus;
    }

    inline status trainerStatus( ) const {
        return m_fsdata.m_trainerStatus;
    }

    /*
     * @brief: returns the encounter data of the currently selected slice.
     */
    const DATA::mapData::wildPkmnData& encounterData( u8 p_encSlot ) const;

    DATA::mapData::wildPkmnData& encounterData( u8 p_encSlot );

    inline void updateSelectedBlock( DATA::mapBlockAtom p_block ) {
        m_settings.m_currentlySelectedBlock = p_block;
    }

    DATA::pixel colorForLocation( u16 p_loc );

    void recomputeDexWPPic( );

    void recomputeBankPic( );

    void recomputeBankLocationOverlay( );
};
