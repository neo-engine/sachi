#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>

#include "../../defines.h"
#include "../../model.h"
#include "fsImage.h"

namespace UI {
    /*
     * @brief: Widget for selecting a single option from a set of options.
     */
    class pokeSwitchButton {
      public:
        using pkmnDscr = std::pair<u16, u8>;

      protected:
        model& _model;

        u8       _currentSelection = 0;
        Gtk::Box _buttonBox{ Gtk::Orientation::HORIZONTAL };

        std::vector<std::shared_ptr<Gtk::ToggleButton>>                  _toggles;
        std::vector<std::shared_ptr<fsImage<imageType::IT_SPRITE_PKMN>>> _pkmnImages;

      public:
        pokeSwitchButton( model& p_model, const std::vector<pkmnDscr>& p_choices,
                          u8 p_defaultChoice = 0 );

        void updateChoices( const std::vector<pkmnDscr>& p_choices );

        void connect( const std::function<void( u8 )>& p_choiceChangedCallback );

        inline operator Gtk::Widget&( ) {
            return _buttonBox;
        }

        inline u8 currentChoice( ) const {
            return _currentSelection;
        }

        inline void choose( u8 p_choice ) {
            if( p_choice > _toggles.size( ) || !_toggles[ p_choice ] ) { return; }

            for( u8 i{ 0 }; i < _toggles.size( ); ++i ) {
                _toggles[ i ]->set_active( i == p_choice );
            }
        }
    };
} // namespace UI
