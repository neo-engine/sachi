#pragma once
#include <memory>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../../model.h"
#include "../../pure/battleBG.h"
#include "../../pure/dropDown.h"

namespace UI {
    class root;
}

namespace UI::TRE {
    /*
     * @brief: Widget to display and edit general info about a trainer
     */
    class trainerInfo {
        bool _lock = false;

        model& _model;
        root&  _rootWindow;

        Gtk::Box _mainBox{ Gtk::Orientation::HORIZONTAL };

        // left column
        Gtk::Label _nameL;
        // - trainer class (editable)
        std::shared_ptr<stringCacheDropDown> _tclassChooser;
        // - money earned (editable)
        std::shared_ptr<Gtk::Adjustment> _money;
        Gtk::SpinButton                  _moneyE;
        // - AI level (editable)
        std::shared_ptr<Gtk::Adjustment> _ai;
        Gtk::SpinButton                  _aiE;
        // - msg1/2/3 (fixed based on tid)
        Gtk::Label _msg1;
        Gtk::Label _msg2;
        Gtk::Label _msg3;

        // right column
        // - sprite (editable)
        // - Battle BG/plat1/plat2 (editable)
        std::shared_ptr<battleBG> _battleBG;

      public:
        trainerInfo( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        void redraw( );

        inline void show( ) {
            _mainBox.show( );
        }

        inline void hide( ) {
            _mainBox.hide( );
        }

        inline bool isVisible( ) {
            return _mainBox.is_visible( );
        }
    };
} // namespace UI::TRE
