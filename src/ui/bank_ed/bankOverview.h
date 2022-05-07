#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>

#include "../switchButton.h"
#include "bnk_ov/bankOverview.h"

namespace UI {
    /*
     * @brief: Main widget of the bank editor.
     */
    class bankOverview {
        Gtk::Box                      _mapOverviewBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<switchButton> _bankOverviewModeToggles;

        mapBankOverview _mapBankOverview;
        Gtk::SpinButton _mapBankOverviewSettings1;
        Gtk::SpinButton _mapBankOverviewSettings2;
        Gtk::SpinButton _mapBankOverviewSettings3;

      public:
        bankOverview( );

        inline operator Gtk::Widget&( ) {
            return _mapOverviewBox;
        }
    };
} // namespace UI
