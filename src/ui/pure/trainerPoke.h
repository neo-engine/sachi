#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../data/fs.h"
#include "../../defines.h"
#include "../../model.h"
#include "fsImage.h"

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
        Gtk::Label                       _nameLabel;
        std::shared_ptr<Gtk::Adjustment> _level;
        Gtk::SpinButton                  _levelE;

        // poke selector + forme + gender + shiny
        // held item
        // ability + moves
        // iv/ev/nature/level

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
