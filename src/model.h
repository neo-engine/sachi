#pragma once
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <gtkmm/stringlist.h>

#include "data/maprender.h"

enum status { STATUS_UNTOUCHED, STATUS_NEW, STATUS_EDITED_UNSAVED, STATUS_SAVED };

struct model {
    struct fsdata {
        struct mapBankContainer {
            DATA::mapBank                                    m_bank;
            std::vector<std::vector<DATA::computedMapSlice>> m_computedBank;

            bool              m_loaded = false;
            DATA::mapBankInfo m_info   = { 0, 0, 1 };
            status            m_status;

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
        };

        struct blockSetInfo {
            DATA::blockSet<1> m_blockSet;
            DATA::tileSet<1>  m_tileSet;
            DATA::palette     m_pals[ 8 * DAYTIMES ];
            u8                m_stringListItem = 0;
        };

        std::string                     m_fsrootPath;
        std::map<u16, mapBankContainer> m_mapBanks;

        u8                                              m_tileSetMode = DATA::TILEMODE_DEFAULT;
        status                                          m_tileStatus;
        std::vector<Glib::ustring>                      m_mapBankStrList; // block set names
        std::vector<std::pair<DATA::computedBlock, u8>> m_currentBlocksets[ 2 ];
        std::map<u8, blockSetInfo>                      m_blockSets;
        std::set<u8>                                    m_blockSetNames;

        inline std::string mapPath( ) const {
            return m_fsrootPath + "/MAPS/";
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

        DATA::mapBlockAtom m_currentlySelectedBlock = DATA::mapBlockAtom( );

        std::string m_windowTitle, m_mainTitle, m_subTitle;
    };

    fsdata   m_fsdata;
    settings m_settings;
    bool     m_good = false;

    bool writeFsRoot( );

    static model readFromPath( const std::string& p_path );

    void addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, u8 p_mapMode = DATA::MAPMODE_COMBINED,
                        status p_status = STATUS_UNTOUCHED );

    inline auto selectedBank( ) const {
        return m_settings.m_selectedBank;
    }

    inline void selectBank( int p_newSelection ) {
        m_settings.m_selectedBank = p_newSelection;
    }

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
     * @brief: Reads all map slices of the specified map bank from the FS.
     * @param p_forceReread: If true, ignores and overwrites any previously read data.
     */
    bool checkOrLoadBank( int p_bank, bool p_forceRead );

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

    inline auto& slice( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        return bank( p_bank ).m_bank.m_slices[ p_mapY ][ p_mapX ];
    }

    inline auto& slice( ) {
        return slice( selectedBank( ), selectedMapY( ), selectedMapX( ) );
    }

    /*
     * @brief: Sets the status of the edit tile set widget to the specified status,
     * resulting in the TS being highlighted in the sidebar.
     */
    inline void markTileSetsChanged( status p_newStatus = STATUS_EDITED_UNSAVED ) {
        m_fsdata.m_tileStatus = p_newStatus;
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

    inline void updateSelectedBlock( DATA::mapBlockAtom p_block ) {
        m_settings.m_currentlySelectedBlock = p_block;
    }
};
