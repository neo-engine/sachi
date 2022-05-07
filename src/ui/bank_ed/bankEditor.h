#pragma once
#include <memory>

#include <gtkmm/label.h>
#include <gtkmm/notebook.h>

#include "bankOverview.h"
#include "bankSettings.h"
#include "mapEditor.h"

namespace UI {
    /*
     * @brief: Main widget of the bank editor.
     */
    class bankEditor {
        Gtk::Label    _loadMapLabel; // Message before map bank loaded
        Gtk::Notebook _mapNotebook;  // main container for anything map bank related

        std::shared_ptr<mapEditor>    _mapEditor;
        std::shared_ptr<bankOverview> _bankOverview;
        std::shared_ptr<bankSettings> _bankSettings;

      public:
        bankEditor( );

        inline operator Gtk::Widget&( ) {
            return _mapNotebook;
        }

        void hide( );
        void show( );
    };
} // namespace UI
