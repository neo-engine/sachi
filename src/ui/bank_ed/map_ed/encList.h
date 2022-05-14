#pragma once

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "../../../defines.h"
#include "../../pure/fsImage.h"
#include "encData.h"

namespace UI::MED {
    /*
     * @brief: A widget to draw a list of wild pkmn widgets.
     */
    class encList {
      protected:
        DATA::wildPkmnType _encType;

        Gtk::Frame _outerFrame;
        Gtk::Box   _encBox{ Gtk::Orientation::VERTICAL };
        Gtk::Label _nameLabel;

        std::vector<encData> _encData;

        fsImage _icon;

      public:
        encList( DATA::wildPkmnType p_encType );

        virtual inline ~encList( ) {
        }

        /*
         * @brief: Clears any contained wpokeEnc entries
         */
        void clearEntries( );

        void addEntries( const std::vector<encData>& p_wpokeEncs );
        void addEntry( const encData& p_wpokeEncs );

        std::vector<DATA::mapData::wildPkmnData> exportData( );

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }

        static std::vector<encList>
        importData( const std::vector<DATA::mapData::wildPkmnData>& p_data );
    };
} // namespace UI::MED
