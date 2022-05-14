#pragma once

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../../defines.h"
#include "../../pure/fsImage.h"

namespace UI::MED {
    /*
     * @brief: A widget to draw a single wild pkmn (including enc data)
     */
    class encData {
      protected:
        wildPkmnData _encData;

        Gtk::Frame _outerFrame;
        Gtk::Box   _copyDelBox{ Gtk::Orientation::VERTICAL }, // box for copy/delete enc data entry
            _pkmnBox{ Gtk::Orientation::HORIZONTAL },         // box for pkmn icon/name/id/forme
            _encRateBox{ Gtk::Orientation::VERTICAL };        // box for encounter rate/req badg
        Gtk::Label                       _nameLabel;
        std::shared_ptr<Gtk::Adjustment> _pkmnIdAdj, _pkmnFormeAdj;
        std::shared_ptr<Gtk::Adjustment> _encRateAdj, _reqBadgeAdj;
        Gtk::SpinButton                  _pkmnIdEntry, _pkmnFormeEntry;
        Gtk::SpinButton                  _encRateEntry, _reqBadgeEntry;

        Gtk::Button _cloneButton{ "Clone" }, _deleteButton{ "Delete" };

        fsImage _pkmnIcon;

      public:
        encData( const wildPkmnData& p_data );

        virtual inline ~encData( ) {
        }

        inline wildPkmnData getData( ) const {
            return _encData;
        }

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }
    };

    class addEncData : public encData {
        std::shared_ptr<Gtk::Adjustment> _nameAdj;
        Gtk::SpinButton                  _nameEntry;

      public:
        addEncData( );

        inline ~addEncData( ) override {
        }
    };
} // namespace UI::MED
