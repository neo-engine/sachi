#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/centerbox.h>

#include "../../defines.h"
#include "../../model.h"
#include "../pure/dropDown.h"
#include "../pure/switchButton.h"

namespace UI {
    class root;

    /*
     * @brief: Widget for map bank settings.
     */
    class bankSettings {
        model& _model;
        root&  _rootWindow;

        Gtk::Box                      _mapSettingsBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<switchButton> _mapBankSettingsMapModeToggles;
        std::shared_ptr<switchButton> _mapIsOW;
        std::shared_ptr<switchButton> _mapHasDive;

        std::shared_ptr<locationDropDown> _bankDefaultLocation;

        Gtk::CenterBox _shbox2, _shbox3, _shbox4;

      public:
        bankSettings( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mapSettingsBox;
        }

        inline void hide( ) {
            _mapSettingsBox.hide( );
        }

        inline void show( ) {
            _mapSettingsBox.show( );
        }

        inline bool isVisible( ) {
            return _mapSettingsBox.is_visible( );
        }

        void redraw( );
    };
} // namespace UI
