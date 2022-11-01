#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "../defines.h"
#include "maprender.h"

namespace DATA {
    constexpr u16 MAPSTRING_LEN = 800;

    struct trainerInfo {
        u16 m_trainerName; // id in trainer names string file
        // TODO
    };

    struct pkmnFormeData {
        u8  m_types[ 2 ];
        u16 m_abilities[ 4 ]; // abilities
        u8  m_bases[ 6 ];     // base values (hp, atk, def, satk, sdef, spd)
        u16 m_expYield;
        u8  m_genderRatio;  // from pkmnGenderType
        u8  m_size;         // in dm
        u16 m_weight;       // in 100g
        u8  m_colorShape;   // (color << 4) | shape
        u8  m_evYield[ 6 ]; // (hp, atk, def, satk, sdef, spd)
        u16 m_items[ 4 ];   // possible held items: 1%, 5%, 50%, 100%
        u8  m_eggGroups;    // (eg1 << 4) | eg2;
    };

    struct pkmnData {
        pkmnFormeData m_baseForme;
        u8            m_expTypeFormeCnt; // (ExpType << 5) | FormeCnt
        u8            m_eggCycles;
        u8            m_catchrate;
        u8            m_baseFriend;
    };

    typedef std::vector<std::pair<u16, u16>> pkmnLearnsetData; // (level, moveid)

    struct itemData {
        u8  m_itemType;
        u8  m_effect; // Effect index
        u16 m_param1; // Effect param 1
        u16 m_param2; // Effect param 2
        u16 m_param3; // Effect param 3
        u16 m_sellPrice;
        u16 m_buyPrice;
    };

    enum moveFlags : long long unsigned {
        /** Ignores a target's substitute. */
        AUTHENTIC = ( 1 << 0 ),
        /** Power is multiplied by 1.5 when used by a Pokemon with the Strong Jaw Ability. */
        BITE = ( 1 << 1 ),
        /** Has no effect on Pokemon with the Bulletproof Ability. */
        BULLET = ( 1 << 2 ),
        /** The user is unable to make a move between turns. */
        CHARGE = ( 1 << 3 ),
        /** Makes contact. */
        CONTACT = ( 1 << 4 ),
        /** When used by a Pokemon, other Pokemon with the Dancer Ability can attempt to execute the
           same move. */
        DANCE = ( 1 << 5 ),
        /** Thaws the user if executed successfully while the user is frozen. */
        DEFROST = ( 1 << 6 ),
        /** Can target a Pokemon positioned anywhere in a Triple Battle. */
        DISTANCE = ( 1 << 7 ),
        /** Prevented from being executed or selected during Gravity's effect. */
        GRAVITY = ( 1 << 8 ),
        /** Prevented from being executed or selected during Heal Block's effect. */
        HEAL = ( 1 << 9 ),
        /** Can be copied by Mirror Move. */
        MIRROR = ( 1 << 10 ),
        /** Recoil halves HP */
        MINDBLOWNRECOIL = ( 1 << 11 ),
        /** Prevented from being executed or selected in a Sky Battle. */
        NONSKY = ( 1 << 12 ),
        /** Has no effect on Grass-type Pokemon, Pokemon with the Overcoat Ability, and Pokemon
           holding Safety Goggles. */
        POWDER = ( 1 << 13 ),
        /** Blocked by Detect, Protect, Spiky Shield, and if not a Status move, King's Shield. */
        PROTECT = ( 1 << 14 ),
        /** Power is multiplied by 1.5 when used by a Pokemon with the Mega Launcher Ability. */
        PULSE = ( 1 << 15 ),
        /** Power is multiplied by 1.2 when used by a Pokemon with the Iron Fist Ability. */
        PUNCH = ( 1 << 16 ),
        /** If this move is successful, the user must recharge on the following turn and cannot make
           a move. */
        RECHARGE = ( 1 << 17 ),
        /** Bounced back to the original user by Magic Coat or the Magic Bounce Ability. */
        REFLECTABLE = ( 1 << 18 ),
        /** Can be stolen from the original user and instead used by another Pokemon using Snatch.
         */
        SNATCH = ( 1 << 19 ),
        /** Has no effect on Pokemon with the Soundproof Ability. */
        SOUND = ( 1 << 20 ),
        /** Forces the pokemon to use the move for another 1-2 turns. */
        LOCKEDMOVE = ( 1 << 21 ),
        /** Maintains the rage counter */
        RAGE = ( 1 << 22 ),
        /** Roost */
        ROOST = ( 1 << 23 ),
        /** Uproar */
        UPROAR = ( 1 << 24 ),
        /** Self-Switch: User switches after successful use */
        SELFSWITCH = ( 1 << 25 ),
        /** Use source defensive stats as offensive stats */
        DEFASOFF = ( 1 << 26 ),
        /** User is damaged if the attack misses */
        CRASHDAMAGE = ( 1 << 27 ),
        /** OHKO move */
        OHKO = ( 1 << 28 ),
        /** OHKO move (useless on ice-type pkmn) */
        OHKOICE = ( 1 << 28 ),
        /** PKMN self-destructs */
        SELFDESTRUCT = ( 1 << 29 ),
        /** PKMN self-destructs */
        SELFDESTRUCTHIT = ( 1 << 30 ),
        /** move cannot be sketched */
        NOSKETCH = ( 1LLU << 31 ),
        /** move pp cannot be increased */
        NOPPBOOST = ( 1LLU << 32 ),
        /** move can be used while asleep */
        SLEEPUSABLE = ( 1LLU << 33 ),
        /** target cannot faint due to move */
        NOFAINT = ( 1LLU << 34 ),
        /** breaks protect */
        BREAKSPROTECT = ( 1LLU << 35 ),
        /** ignores type immunities */
        IGNOREIMMUNITY = ( 1LLU << 36 ),
        /** ignores ground type immunity */
        IGNOREIMMUNITYGROUND = ( 1LLU << 37 ),
        /** ignores abilities */
        IGNOREABILITY = ( 1LLU << 38 ),
        /** ignores defense boosts */
        IGNOREDEFS = ( 1LLU << 39 ),
        /** ignores evasion boosts */
        IGNOREEVASION = ( 1LLU << 40 ),
        /** Defrosts the target */
        DEFROSTTARGET = ( 1LLU << 41 ),
        /** Forces the target to switch out */
        FORCESWITCH = ( 1LLU << 42 ),
        /** Will always land a critical hit */
        WILLCRIT = ( 1LLU << 43 ),
        /** Move uses atk/satk of the target */
        TARGETOFFENSIVES = ( 1LLU << 44 ),
        /** Move hits at a later time */
        FUTUREMOVE = ( 1LLU << 45 ),
        /** Beton pass */
        BATONPASS = ( 1LLU << 46 ),
    };

    enum targets : u8 {
        NORMAL                = 0,
        ADJACENT_ALLY         = 2,
        ADJACENT_FOE          = 3,
        ADJACENT_ALLY_OR_SELF = 4,
        ANY                   = 12, // single target
        SELF                  = 5,
        RANDOM                = 6, // single-target, automatic
        ALLIES                = 13,
        ALL_ADJACENT          = 7,
        ALL_ADJACENT_FOES     = 8, // spread
        ALLY_SIDE             = 9,
        FOE_SIDE              = 10,
        ALL                   = 11, // field
        SCRIPTED              = 14,
        ALLY_TEAM             = 15,
    };

    enum moveHitTypes : u8 { NOOP = 0, PHYSICAL = 1, SPECIAL = 2, STATUS = 3 };

#define HP       0
#define ATK      1
#define DEF      2
#define SATK     3
#define SDEF     4
#define SPEED    5
#define EVASION  6
#define ACCURACY 7

    struct boosts {
        u32         m_boosts;
        inline void setBoost( u8 p_stat, s8 p_val ) {
            if( p_val > 7 || p_val < -7 ) {
                fprintf( stderr, "Bad boosts value [%hhu] := %hhd\n", p_stat, p_val );
                return;
            }
            p_val += 7;

            m_boosts &= ( 0xFFFFFFFF - ( 0xF << p_stat ) );
            m_boosts |= ( p_val << p_stat );
        }
        constexpr s8 getBoost( u8 p_stat ) {
            return ( ( m_boosts >> p_stat ) & 0xF ) - 7;
        }
    };

    enum volatileStatus : u64 {
        NONE             = 0,
        CONFUSION        = ( 1 << 0 ),
        OBSTRUCT         = ( 1 << 1 ),
        PARTIALLYTRAPPED = ( 1 << 2 ),
        FLINCH           = ( 1 << 3 ),
        OCTOLOCK         = ( 1 << 4 ),
        TARSHOT          = ( 1 << 5 ),
        NORETREAT        = ( 1 << 6 ),
        LASERFOCUS       = ( 1 << 7 ),
        SPOTLIGHT        = ( 1 << 8 ),
        BANEFULBUNKER    = ( 1 << 9 ),
        SMACKDOWN        = ( 1 << 10 ),
        POWDERED         = ( 1 << 11 ),
        SPIKYSHIELD      = ( 1 << 12 ),
        KINGSSHIELD      = ( 1 << 13 ),
        ELECTRIFY        = ( 1 << 14 ),
        RAGEPOWDER       = ( 1 << 15 ),
        TELEKINESIS      = ( 1 << 16 ),
        MAGNETRISE       = ( 1 << 17 ),
        AQUARING         = ( 1 << 18 ),
        GASTROACID       = ( 1 << 19 ),
        POWERTRICK       = ( 1 << 20 ),
        HEALBLOCK        = ( 1 << 21 ),
        EMBARGO          = ( 1 << 22 ),
        MIRACLEEYE       = ( 1 << 23 ),
        SUBSTITUTE       = ( 1 << 24 ),
        BIDE             = ( 1 << 25 ),
        FOCUSENERGY      = ( 1 << 26 ),
        DEFENSECURL      = ( 1 << 27 ),
        MINIMIZE         = ( 1 << 28 ),
        LEECHSEED        = ( 1 << 29 ),
        DISABLE          = ( 1 << 30 ),
        FORESIGHT        = ( 1LLU << 31 ),
        SNATCH_          = ( 1LLU << 32 ),
        GRUDGE           = ( 1LLU << 33 ),
        IMPRISON         = ( 1LLU << 34 ),
        YAWN             = ( 1LLU << 35 ),
        MAGICCOAT        = ( 1LLU << 36 ),
        INGRAIN          = ( 1LLU << 37 ),
        HELPINGHAND      = ( 1LLU << 38 ),
        TAUNT            = ( 1LLU << 39 ),
        CHARGE_          = ( 1LLU << 40 ),
        FOLLOWME         = ( 1LLU << 41 ),
        TORMENT          = ( 1LLU << 42 ),
        ATTRACT          = ( 1LLU << 43 ),
        ENDURE           = ( 1LLU << 44 ),
        PROTECT_         = ( 1LLU << 45 ),
        DESTINYBOND      = ( 1LLU << 46 ),
        CURSE            = ( 1LLU << 47 ),
        NIGHTMARE        = ( 1LLU << 48 ),
        STOCKPILE        = ( 1LLU << 49 ),
        ENCORE           = ( 1LLU << 50 ),
    };

    struct moveData {
        u8 m_type        = 9; // ???
        u8 m_contestType = 0; // Clever, Smart, ...
        u8 m_basePower   = 0;
        u8 m_pp          = 1;

        moveHitTypes m_category          = (moveHitTypes) 0;
        moveHitTypes m_defensiveCategory = (moveHitTypes) 0; // category used for defending pkmn
        u8           m_accuracy          = 0;                // 255: always hit
        s8           m_priority          = 0;

        u32 m_sideCondition = 0; // side introduced by the move (reflect, etc)

        u8 m_weather       = 0; // weather introduced by the move
        u8 m_pseudoWeather = 0; // pseudo weather introduced by the move
        u8 m_terrain       = 0; // terrain introduced by the move
        u8 m_status        = 0;

        u8      m_slotCondition  = 0; // stuff introduced on the slot (wish, etc)
        u8      m_fixedDamage    = 0;
        targets m_target         = (targets) 0;
        targets m_pressureTarget = (targets) 0; // restrictions are computed based on different
                                                // target than resulting effect

        u8 m_heal     = 0; // as m_heal / 240
        u8 m_recoil   = 0; // as dealt damage * m_recoil / 240
        u8 m_drain    = 0; // as dealt damage * m_recoil / 240
        u8 m_multiHit = 0; // as ( min << 8 ) | max

        u8 m_critRatio       = 1;
        u8 m_secondaryChance = 0; // chance that the secondary effect triggers
        u8 m_secondaryStatus = 0;
        u8 m_unused          = 0;

        volatileStatus m_volatileStatus          = (volatileStatus) 0; // confusion, etc
        volatileStatus m_secondaryVolatileStatus = (volatileStatus) 0; // confusion, etc

        boosts m_boosts              = { 0 }; // Status ``boosts'' for the target
        boosts m_selfBoosts          = { 0 }; // Status ``boosts'' for the user (if target != user)
        boosts m_secondaryBoosts     = { 0 }; // Stat ``boosts'' for the target
        boosts m_secondarySelfBoosts = { 0 }; // Stat ``boosts'' for the user (if target != user)

        moveFlags m_flags = (moveFlags) 0;
    };

    struct names {
        char* m_name[ NUM_LANGUAGES ];
    };
    struct data {
        char m_data[ 200 ][ NUM_LANGUAGES ];
    };

    struct blockSetBankHeader {
        u8  m_blockSetCount = 0;        // number of block sets
        u8  m_dayTimeCount  = DAYTIMES; // number of extra daytime palettes per palette
        u32 m_1 : 16        = 0;
        u32 m_2 : 32        = 0;
    };

    struct fsdataInfo {
        static constexpr u8 MAX_OW_MAPS = 8;

        u16 m_maxPkmn = 0;
        u16 m_maxItem = 0;

        u16 m_maxMove   = 0;  // valid move idx 0..m_maxMove
        u8  m_languages = 2;  // num languages
        u8  m_fileSplit = 30; // max num files per dir

        u8 m_maxNavBG     = 0; // valid subscrn wallpaper 0..m_maxNavBg
        u8 m_defaultNavBG = 0;
        u8 m_owMapCount   = 1;
        u8 m_defaultOWMap = 0;

        u8 m_owMaps[ MAX_OW_MAPS ] = { 10 };

        /*
         * @brief: adds or removes p_bank from m_owMaps, according to p_owStatus.
         */
        inline bool updateOWStatus( u8 p_bank, u8 p_owStatus ) {
            for( u8 i{ 0 }; i < m_owMapCount; ++i ) {
                if( m_owMaps[ i ] == p_bank ) {
                    if( !p_owStatus ) {
                        // delete map from ow maps
                        for( u8 j{ i }; j < --m_owMapCount; ++j ) {
                            m_owMaps[ j ] = m_owMaps[ j + 1 ];
                        }
                    }
                    return true;
                }
            }
            if( m_owMapCount + 1 < MAX_OW_MAPS && p_owStatus ) {
                m_owMaps[ m_owMapCount++ ] = p_bank;
                return true;
            } else if( p_owStatus ) {
                // not enough space to store ow map
                return false;
            }
            return true;
        }
    };

    u16 frameForLocation( const char* p_path, u16 p_location );
} // namespace DATA
