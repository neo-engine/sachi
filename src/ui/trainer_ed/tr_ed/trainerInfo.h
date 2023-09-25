#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/label.h>

#include "../../../model.h"
#include "../../pure/battleBG.h"

namespace UI {
    class root;
}

namespace UI::TRE {
    /*
     * @brief: Widget to display and edit general info about a trainer
     */
    class trainerInfo {
        model& _model;
        root&  _rootWindow;

        Gtk::Box _mainBox{ Gtk::Orientation::HORIZONTAL };

        // left column
        // - trainer class (editable)
        // - name (fixed based on tid)
        Gtk::Label _nameL;
        // - msg1/2/3 (fixed based on tid)
        Gtk::Label _msg1;
        Gtk::Label _msg2;
        Gtk::Label _msg3;
        // - money earned (editable)
        // - AI level (editable)
        //
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
