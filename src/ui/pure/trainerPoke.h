#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>

#include "../../data/fs.h"
#include "../../defines.h"
#include "../../model.h"
#include "dropDown.h"
#include "fsImage.h"
#include "itemSelector.h"
#include "pokeSelector.h"
#include "switchButton.h"

namespace UI {
    /*
     * @brief: A widget to draw a single map bank
     */
    class trainerPoke {
      protected:
        bool _lock = false;

        model& _model;

        DATA::trainerPokemon _data;

        Gtk::Box                         _mainBox;
        std::shared_ptr<Gtk::Adjustment> _level;
        Gtk::SpinButton                  _levelE;

        // poke selector + forme + gender + shiny
        std::shared_ptr<pokeSelector> _pkmn;
        std::shared_ptr<switchButton> _shiny;
        // held item
        std::shared_ptr<itemSelector> _item;
        // ability + moves
        std::shared_ptr<numberedStringCacheDropDown>              _ability;
        std::vector<std::shared_ptr<numberedStringCacheDropDown>> _moves;

        std::vector<std::shared_ptr<Gtk::Adjustment>> _iv;
        std::vector<Gtk::SpinButton>                  _ivE;

        std::vector<std::shared_ptr<Gtk::Adjustment>> _ev;
        std::vector<Gtk::SpinButton>                  _evE;
        // iv/ev/nature
        std::shared_ptr<dropDown>                  _natureDropDown;
        std::vector<std::shared_ptr<switchButton>> _natureToggles;

      public:
        trainerPoke( model& p_model );

        void set( const DATA::trainerPokemon& p_poke );

        inline const DATA::trainerPokemon& get( ) const {
            return _data;
        }

        void connect( const std::function<void( )>& p_callback );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }
    };
} // namespace UI
