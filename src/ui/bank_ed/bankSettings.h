#pragma once
#include <memory>

#include <gtkmm/box.h>

#include "../../defines.h"
#include "../../model.h"
#include "../switchButton.h"

namespace UI {
    class root;

    /*
     * @brief: Widget for map bank settings.
     */
    class bankSettings {
        model& _model;
        root&  _rootWindow;

        Gtk::Box                      _mapSettingsBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<switchButton> _mapBankSettingsMapModeToggles;

      public:
        bankSettings( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mapSettingsBox;
        }

        inline void hide( ) {
            _mapSettingsBox.hide( );
        }

        inline void show( ) {
            _mapSettingsBox.show( );
        }

        inline bool isVisible( ) {
            return _mapSettingsBox.is_visible( );
        }

        void redraw( );
    };
} // namespace UI
