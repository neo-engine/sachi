#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>

#include "../switchButton.h"
#include "bnk_ov/actionBar.h"
#include "bnk_ov/mapBankOverview.h"

namespace UI {
    /*
     * @brief: Main widget of the bank editor.
     */
    class bankOverview {
      public:
        enum bankOverviewMode : u8 { MODE_VIEW_MAPS, MODE_VIEW_LOCATIONS };

      private:
        model& _model;
        root&  _rootWindow;

        bankOverviewMode _mapOverviewMode;

        Gtk::Box                        _mapOverviewBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<switchButton>   _bankOverviewModeToggles;
        BOV::mapBankOverview            _mapBankOverview;
        std::shared_ptr<BOV::actionBar> _actionBar;

      public:
        bankOverview( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mapOverviewBox;
        }

        void render( );

        void redraw( );

        void setNewMapOverviewMode( bankOverviewMode p_newMode );

        inline void replaceOverviewMap( const DATA::computedMapSlice& p_map, u8 p_mapY,
                                        u8 p_mapX ) {
            _mapBankOverview.replaceMap( p_map, p_mapY, p_mapX );
        }
    };
} // namespace UI
