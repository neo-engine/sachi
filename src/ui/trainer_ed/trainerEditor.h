#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>

#include "../../defines.h"
#include "../../model.h"

namespace UI {
    class root;

    namespace TRE {
        // class trainerInfo;
        // class trainerItems;
        // class trainerTeamEditor;
    }; // namespace TRE

    /*
     * @brief: Widget that allows for editing a single trainer.
     */
    class trainerEditor {
      private:
        model& _model;
        root&  _rootWindow;

        // sub widgets
        Gtk::Box _mainBox{ Gtk::Orientation::VERTICAL };

        Gtk::Box _contentMainBox{ Gtk::Orientation::VERTICAL };

        // std::shared_ptr<TRE::trainerInfo>       _trainerInfo;
        // std::shared_ptr<TRE::trainerItems>      _trainerItems;
        // std::shared_ptr<TRE::trainerTeamEditor> _trainerTeams;

      public:
        trainerEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        void redraw( );
    };
} // namespace UI
