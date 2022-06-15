#pragma once

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "../../../defines.h"
#include "../../../model.h"
#include "wildPoke.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: A widget to draw a list of wild pkmn widgets.
     */
    class encList {
      protected:
        model& _model;
        root&  _rootWindow;

        Gtk::Box _outerFrame;
        Gtk::Box _encBox{ Gtk::Orientation::VERTICAL };

        std::vector<std::shared_ptr<wildPoke>> _encData;

      public:
        encList( model& p_model, root& p_rootWindow );

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }

        inline void redraw( ) {
            for( auto e : _encData ) {
                if( e ) { e->redraw( ); }
            }
        }
    };
} // namespace UI::MED
