#pragma once
#include <memory>
#include <string>

#include <gdkmm/pixbuf.h>

#include "../../../data/maprender.h"
#include "../../../defines.h"

namespace UI::MED {
    class block {
      public:
        static std::shared_ptr<Gdk::Pixbuf> createImage( const DATA::computedBlock& p_block,
                                                         const DATA::palette*       p_palette,
                                                         u8                         p_daytime = 0 );

        static inline std::string classForMovement( u8 p_movement ) {
            switch( p_movement ) {
            case 0x00: return "movement-any";
            case 0x01: return "movement-none";
            case 0x04: return "movement-water";
            case 0x0c: return "movement-walk";
            case 0x0a: return "movement-sit";
            case 0x3c: return "movement-bridge";
            case 0x3f: return "movement-border";
            default: break;
            }
            if( p_movement % 4 == 0 ) { return "movement-elevated"; }
            if( p_movement % 4 == 1 ) { return "movement-blocked"; }
            return "movement-weird";
        }
    };
} // namespace UI::MED
