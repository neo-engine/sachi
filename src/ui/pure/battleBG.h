#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../defines.h"
#include "../../model.h"
#include "fsImage.h"

namespace UI {
    /*
     * @brief: A widget to draw a single map bank
     */
    class battleBG {
      protected:
        bool _lock = false;

        u8 _bgIdx = 255;
        u8 _plat1 = 255;
        u8 _plat2 = 255;
        u8 _sIdx  = 255;

        Gtk::Box                         _mainBox;
        Gtk::Label                       _nameLabel, _p3l;
        std::shared_ptr<Gtk::Adjustment> _bg, _p1, _p2, _sprite;
        Gtk::SpinButton                  _bgE, _p1E, _p2E, _spriteE;

        fsImage<imageType::IT_BG_IMAGE>          _bgI;
        fsImage<imageType::IT_SPRITE_PLATFORM>   _p1I, _p2I;
        fsImage<imageType::IT_SPRITE_ICON_64x64> _spriteI;

      public:
        battleBG( const std::string& p_name );

        void set( const std::string& p_bgPath, const std::string& p_platPath, u8 p_bg, u8 p_p1,
                  u8 p_p2 );

        void setSprite( const std::string& p_spritePath, u8 p_sId );

        void connect( const std::function<void( u8, u8, u8 )>& p_callback );

        void connect2( const std::function<void( u8, u8, u8, u8 )>& p_callback );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }
    };
} // namespace UI
