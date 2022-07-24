#pragma once
#include <memory>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../../data/maprender.h"
#include "../../../model.h"
#include "../../pure/dropDown.h"
#include "../../pure/mapPosition.h"
#include "../../pure/multiButton.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: Widget to select an event of the currently selected mapSlice and display relevant
     * information about the selected event.
     */
    class eventSelector {
        model& _model;
        root&  _rootWindow;

        bool _disableRedraw = false;
        bool _disableMI1E   = false;

        Gtk::Frame _mainFrame;
        Gtk::Box   _generalData{ Gtk::Orientation::VERTICAL };
        Gtk::Label _messageLabel1;

        std::vector<Gtk::Frame> _detailFrames;

        std::shared_ptr<Gtk::Button>     _warpJumpTo;
        std::shared_ptr<mapPosition>     _eventPosition, _warpTarget;
        std::shared_ptr<dropDown>        _eventType, _messageType1, _warpType;
        std::shared_ptr<multiButton>     _eventTrigger;
        std::shared_ptr<Gtk::Adjustment> _selectedEventA, _aFlagA, _dFlagA, _messageIdx1A;
        Gtk::SpinButton                  _selectedEventE, _aFlagE, _dFlagE, _messageIdx1E;

      public:
        eventSelector( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainFrame;
        }

        void redraw( );

        inline void show( ) {
            _mainFrame.show( );
        }

        inline void hide( ) {
            _mainFrame.hide( );
        }

        inline bool isVisible( ) {
            return _mainFrame.is_visible( );
        }
    };
} // namespace UI::MED
