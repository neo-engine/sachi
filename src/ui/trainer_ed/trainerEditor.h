#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>

#include "../../defines.h"
#include "../../model.h"
#include "../pure/switchButton.h"

namespace UI {
    class root;

    namespace TRE {
        class trainerInfo;
        class trainerItems;
        class trainerTeam;
    }; // namespace TRE

    /*
     * @brief: Widget that allows for editing a single trainer.
     */
    class trainerEditor {
      private:
        model& _model;
        root&  _rootWindow;

        enum difficulty : u8 {
            DIFF_EASY,
            DIFF_NORMAL,
            DIFF_HARD,
            MAX_DIFF = DIFF_HARD,
        };

        u8 _selectedDifficulty = DIFF_NORMAL;

        // sub widgets
        Gtk::Box _mainBox{ Gtk::Orientation::VERTICAL };

        Gtk::Box                      _diffEnabledBox{ Gtk::Orientation::HORIZONTAL };
        std::shared_ptr<switchButton> _diffEnabled;

        Gtk::Box _contentMainBox{ Gtk::Orientation::VERTICAL };

        std::shared_ptr<switchButton> _diffSelector;

        std::shared_ptr<TRE::trainerInfo>  _trainerInfo;
        std::shared_ptr<TRE::trainerItems> _trainerItems;
        std::shared_ptr<TRE::trainerTeam>  _trainerTeam;

      public:
        trainerEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        void redraw( );

        void enableDifficulty( bool p_enabled );

        void setDifficulty( difficulty p_newDiffculty );
    };
} // namespace UI
