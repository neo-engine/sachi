#pragma once
#include <memory>

#include <gtkmm/box.h>

#include "../../defines.h"
#include "../../model.h"
#include "../switchButton.h"

namespace UI {
    class root;

    /*
     * @brief: Widget for tile set settings.
     */
    class tileSetSettings {
        model& _model;
        root&  _rootWindow;

        Gtk::Box                      _tsSettingsBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<switchButton> _tsSettingsModeToggles;

      public:
        tileSetSettings( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _tsSettingsBox;
        }

        inline void hide( ) {
            _tsSettingsBox.hide( );
        }

        inline void show( ) {
            _tsSettingsBox.show( );
        }

        inline bool isVisible( ) {
            return _tsSettingsBox.is_visible( );
        }

        void redraw( );
    };
} // namespace UI
