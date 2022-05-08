#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>

#include "../defines.h"

namespace UI {
    /*
     * @brief: Widget for selecting a single option from a set of options.
     */
    class switchButton {
        u8       _currentSelection = 0;
        Gtk::Box _buttonBox{ Gtk::Orientation::HORIZONTAL };
        std::vector<std::shared_ptr<Gtk::ToggleButton>>
            _modeToggles; // buttons to toggle current display mode

      public:
        switchButton( const std::vector<std::string>& p_choices, u8 p_defaultChoice = 0 );

        void connect( const std::function<void( u8 )>& p_choiceChangedCallback );

        inline operator Gtk::Widget&( ) {
            return _buttonBox;
        }

        inline u8 currentChoice( ) const {
            return _currentSelection;
        }
    };
} // namespace UI
