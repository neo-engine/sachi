#pragma once
#include <memory>

#include <gtkmm/box.h>

#include "../../../defines.h"
#include "../../../model.h"
#include "../../pure/battleBG.h"
#include "../../pure/dropDown.h"
#include "../../pure/multiButton.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: Widget for map settings.
     */
    class mapSettings {
        model& _model;
        root&  _rootWindow;

        Gtk::Box _mainBox{ Gtk::Orientation::VERTICAL };

        std::shared_ptr<multiButton> _mapMode;
        std::shared_ptr<dropDown>    _mapWeather;

        std::shared_ptr<battleBG> _battleBG, _surfBG;

      public:
        mapSettings( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        inline void hide( ) {
            _mainBox.hide( );
        }

        inline void show( ) {
            _mainBox.show( );
        }

        inline bool isVisible( ) {
            return _mainBox.is_visible( );
        }

        void redraw( );
    };
} // namespace UI::MED
