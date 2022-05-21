#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/frame.h>

#include "../../../model.h"
#include "../../pure/editableColor.h"

namespace UI {
    class root;
}

namespace UI::TED {
    class paletteEditor {
        model& _model;
        root&  _rootWindow;

        Gtk::Frame                                  _frame;
        std::vector<std::shared_ptr<editableColor>> _colors;

      public:
        paletteEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _frame;
        }

        void redraw( );

        inline void show( ) {
            _frame.show( );
        }

        inline void hide( ) {
            _frame.hide( );
        }

        inline bool isVisible( ) {
            return _frame.is_visible( );
        }

        void setColor( u8 p_idx, u16 p_color );
    };
} // namespace UI::TED
