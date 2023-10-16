#pragma once
#include <memory>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/overlay.h>
#include <gtkmm/spinbutton.h>

#include "../../defines.h"
#include "../../model.h"
#include "../pure/dropDown.h"
#include "../pure/fsImage.h"
#include "../pure/switchButton.h"

namespace UI {
    class root;

    /*
     * @brief: Widget for map bank settings.
     */
    class bankSettings {
        model& _model;
        root&  _rootWindow;

        bool _disableSB;

        Gtk::Box                      _mapSettingsBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<switchButton> _mapBankSettingsMapModeToggles;
        std::shared_ptr<switchButton> _mapIsOW;
        std::shared_ptr<switchButton> _mapHasDive;
        std::shared_ptr<switchButton> _showNavBorder;
        std::shared_ptr<switchButton> _showDexBorder;
        std::shared_ptr<switchButton> _showTouchArea;
        std::shared_ptr<Gtk::Button>  _recomputeMapBG, _recomputeOverlay;

        std::shared_ptr<Gtk::Button> _recomputeWildPokeBG;

        std::shared_ptr<numberedStringCacheDropDown> _bankDefaultLocation;

        Gtk::CenterBox _shbox2, _shbox3, _shbox4;

        Gtk::Frame _owMapFrame, _wpMapFrame;

        std::shared_ptr<Gtk::Adjustment> _shiftXA, _shiftYA;
        std::shared_ptr<Gtk::Adjustment> _wpshiftXA, _wpshiftYA;
        Gtk::SpinButton                  _shiftXE, _shiftYE;
        Gtk::SpinButton                  _wpshiftXE, _wpshiftYE;

        Gtk::Box                        _touchArea;
        fsImage<imageType::IT_BITMAP>   _owImage;
        fsImage<imageType::IT_BG_IMAGE> _navBorder;

        fsImage<imageType::IT_BITMAP>   _wpImage;
        fsImage<imageType::IT_BG_IMAGE> _dexBorder;

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
