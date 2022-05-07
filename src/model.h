#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "data/maprender.h"

enum status { STATUS_UNTOUCHED, STATUS_NEW, STATUS_EDITED_UNSAVED, STATUS_SAVED };

struct model {
    struct fsdata {
        struct mapBankContainer {
            DATA::mapBank                                    m_bank;
            std::vector<std::vector<DATA::computedMapSlice>> m_computedBank;

            bool              m_loaded = false;
            DATA::mapBankInfo m_info   = { 0, 0, 1 };

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

        std::string m_fsrootPath;

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

        u8   m_currentDayTime  = 0;
        u8   m_blockScale      = 1;
        u8   m_blockSpacing    = 0;
        u8   m_neighborSpacing = 10;
        bool m_showAdjacent    = true;
        u16  m_blockSetWidth   = 8;
        u8   m_adjacentBlocks  = 8;

        u16                m_currentlySelectedBlockIdx;
        DATA::mapBlockAtom m_currentlySelectedBlock = DATA::mapBlockAtom( );
        // std::vector<std::vector<lookupMapSlice>>
        //    m_currentMap; // main map and parts of the adjacent maps

        std::string m_windowTitle, m_mainTitle, m_subTitle;
    };

    fsdata   m_fsdata;
    settings m_settings;

    bool loadNewFsRoot( const std::string& p_path );
};
