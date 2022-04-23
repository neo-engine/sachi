#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>

#include "../../defines.h"
#include "../../log.h"
#include "../fs.h"
#include "../maprender.h"
#include "util.h"

namespace DATA {
    const char PKMNDATA_PATH[] = "PKMNDATA/";
    const char SCRIPT_PATH[]   = "DATA/MAP_SCRIPT/";

    const char CRY_PATH[]  = "SOUND/CRIES/";
    const char SFX_PATH[]  = "SOUND/SFX/";
    const char SSEQ_PATH[] = "SOUND/BGM/SSEQ/";
    const char SBNK_PATH[] = "SOUND/BGM/SBNK/";
    const char SWAR_PATH[] = "SOUND/BGM/SWAR/";

    const char ITEM_NAME_PATH[]        = "DATA/ITEM_NAME/itemname";
    const char ITEM_DSCR_PATH[]        = "DATA/ITEM_DSCR/itemdscr";
    const char ABILITY_NAME_PATH[]     = "DATA/ABTY_NAME/abtyname";
    const char ABILITY_DSCR_PATH[]     = "DATA/ABTY_DSCR/abtydscr";
    const char MOVE_NAME_PATH[]        = "DATA/MOVE_NAME/movename";
    const char MOVE_DSCR_PATH[]        = "DATA/MOVE_DSCR/movedscr";
    const char POKEMON_NAME_PATH[]     = "DATA/PKMN_NAME/pkmnname";
    const char FORME_NAME_PATH[]       = "DATA/PKMN_NAME/pkmnfname";
    const char POKEMON_SPECIES_PATH[]  = "DATA/PKMN_SPCS/pkmnspcs";
    const char POKEMON_DEXENTRY_PATH[] = "DATA/PKMN_DXTR/pkmndxtr";

    const char BATTLE_STRINGS_PATH[]          = "DATA/TRNR_STRS/";
    const char BATTLE_TRAINER_PATH[]          = "DATA/TRNR_DATA/";
    const char BATTLE_FACILITY_STRINGS_PATH[] = "DATA/BFTR_STRS/";
    const char BATTLE_FACILITY_PKMN_PATH[]    = "DATA/BFTR_PKMN/";
    const char TCLASS_NAME_PATH[]             = "DATA/TRNR_NAME/trnrname";

    const char BGM_NAME_PATH[]      = "DATA/BGM_NAME/bgmnames";
    const char LOCATION_NAME_PATH[] = "DATA/LOC_NAME/locname";
    const char UISTRING_PATH[]      = "STRN/UIS/uis";
    const char MAPSTRING_PATH[]     = "STRN/MAP/map";
    const char PKMNPHRS_PATH[]      = "STRN/PHR/phr";
    const char BADGENAME_PATH[]     = "STRN/BDG/bdg";
    const char ACHIEVEMENT_PATH[]   = "STRN/AVM/avm";

    const char LOCDATA_PATH[]        = "DATA/location.datab";
    const char MOVE_DATA_PATH[]      = "DATA/move.datab";
    const char ITEM_DATA_PATH[]      = "DATA/item.datab";
    const char POKEMON_DATA_PATH[]   = "DATA/pkmn.datab";
    const char PKMN_LEARNSET_PATH[]  = "DATA/pkmn.learnset.datab";
    const char POKEMON_EVOS_PATH[]   = "DATA/pkmn.evolve.datab";
    const char FORME_DATA_PATH[]     = "DATA/pkmnf.datab";
    const char FORME_LEARNSET_PATH[] = "DATA/pkmnf.learnset.datab";
    const char FORME_EVOS_PATH[]     = "DATA/pkmnf.evolve.datab";

    FILE* LOCATION_DATA_FILE = nullptr;

    struct locationData {
        u16 m_bgmNameIdx = 0;
        u8  m_frameType  = 0;
        u8  m_mugType    = 0;
    };

    u16 BGMforLocation( u16 p_locationId ) {
        if( !checkOrOpen( LOCATION_DATA_FILE, LOCDATA_PATH ) ) { return 0; }

        if( std::fseek( LOCATION_DATA_FILE, p_locationId * 4, SEEK_SET ) ) { return 0; }

        auto l = locationData( );
        fread( &l, sizeof( locationData ), 1, LOCATION_DATA_FILE );

        return l.m_bgmNameIdx;
    }

    u16 frameForLocation( u16 p_locationId ) {
        if( !checkOrOpen( LOCATION_DATA_FILE, LOCDATA_PATH ) ) { return 0; }
        if( std::fseek( LOCATION_DATA_FILE, p_locationId * sizeof( locationData ), SEEK_SET ) ) {
            return 0;
        }

        auto l = locationData( );
        fread( &l, sizeof( locationData ), 1, LOCATION_DATA_FILE );

        return l.m_frameType;
    }

    u16 mugForLocation( u16 p_locationId ) {
        if( !checkOrOpen( LOCATION_DATA_FILE, LOCDATA_PATH ) ) { return 0; }
        if( std::fseek( LOCATION_DATA_FILE, p_locationId * sizeof( locationData ), SEEK_SET ) ) {
            return 0;
        }

        auto l = locationData( );
        fread( &l, sizeof( locationData ), 1, LOCATION_DATA_FILE );

        return l.m_mugType;
    }

    FILE* openScript( u16 p_scriptId ) {
        return openSplit( SCRIPT_PATH, p_scriptId, ".mapscr", 10 * 30 );
    }

    bool readPictureData( unsigned int* p_imgOut, unsigned short* p_palOut, const char* p_path,
                          const char* p_name ) {
        return readData<unsigned int, unsigned short>( p_path, p_name, 12288, p_imgOut, 256,
                                                       p_palOut );
    }

} // namespace DATA
