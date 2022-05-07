#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>

namespace UI {
    /*
     * @brief: Widget for map bank settings.
     */
    class bankSettings {
        Gtk::Box _mapSettingsBox{ Gtk::Orientation::VERTICAL };
        std::vector<std::shared_ptr<Gtk::ToggleButton>> _mapBankSettingsMapModeToggles;

      public:
        bankSettings( );

        inline operator Gtk::Widget&( ) {
            return _mapSettingsBox;
        }
    };
} // namespace UI
