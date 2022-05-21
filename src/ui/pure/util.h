#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

#include <gdkmm/pixbuf.h>
#include <gtkmm/button.h>

#include "../../data/maprender.h"
#include "../../defines.h"

namespace UI {
    /*
     * @brief: Creates a new Gtk::Button with an icon and a string and the given
     * handler for the clicked signal.
     */
    std::shared_ptr<Gtk::Button> createButton(
        const std::string& p_iconName, const std::string& p_labelText,
        std::function<void( )> p_callBack = []( ) {} );

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

    class tile {
      public:
        static constexpr DATA::palette DUMMY_PAL[ 5 * 16 ] = { };

        static std::shared_ptr<Gdk::Pixbuf> createImage( u16 p_color );

        static std::shared_ptr<Gdk::Pixbuf> createImage( const DATA::tile&    p_tile,
                                                         const DATA::palette& p_palette,
                                                         bool p_flipX, bool p_flipY );

        static std::shared_ptr<Gtk::Image>
        createImage( const DATA::computedBlockAtom& p_tile,
                     const DATA::palette p_pals[ 5 * 16 ] = tile::DUMMY_PAL, u8 p_daytime = 0 );
    };
} // namespace UI
