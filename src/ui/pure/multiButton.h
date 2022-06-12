#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>

#include "../../defines.h"

namespace UI {
    /*
     * @brief: Widget for selecting multiple options from a set of (at most 32) options.
     */
    class multiButton {
        u32                           _currentSelection = 0;
        std::function<u32( u32, u8 )> _transition;

        Gtk::Box _buttonBox;

        std::vector<std::shared_ptr<Gtk::ToggleButton>> _toggles;

      public:
        multiButton( const std::vector<std::string>&      p_choices,
                     const std::function<u32( u32, u8 )>& p_transition, u32 p_defaultChoice = 0,
                     Gtk::Orientation p_orientation = Gtk::Orientation::HORIZONTAL );

        void connect( const std::function<void( u32 )>& p_choiceChangedCallback );

        inline operator Gtk::Widget&( ) {
            return _buttonBox;
        }

        /*
         * @returns: A bitflag corresponding to the currently selected items.
         */
        inline u32 currentChoice( ) const {
            return _currentSelection;
        }

        inline void choose( u32 p_choice ) {
            _currentSelection = p_choice;

            for( u8 i{ 0 }; i < _toggles.size( ); ++i ) {
                _toggles[ i ]->set_active( !!( p_choice & ( 1 << i ) ) );
            }
        }
    };
} // namespace UI
