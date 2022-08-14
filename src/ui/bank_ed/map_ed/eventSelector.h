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
#include "../../pure/itemSelector.h"
#include "../../pure/mapPosition.h"
#include "../../pure/multiButton.h"
#include "../../pure/owSpriteSelector.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: Widget to select an event of the currently selected mapSlice and display relevant
     * information about the selected event.
     */
    class eventSelector {
        model&     _model;
        mapEditor& _parent;
        root&      _rootWindow;

        bool _disableRedraw = false;
        bool _disableMI1E   = false;
        bool _disableSI1E   = false;
        bool _disableSI2E   = false;
        bool _disableWSI    = false;
        bool _disableBTI    = false;
        bool _disableAF     = false;
        bool _disableDF     = false;

        Gtk::Frame _mainFrame;
        Gtk::Box   _generalData{ Gtk::Orientation::VERTICAL },
            _warpScriptIdxBox{ Gtk::Orientation::HORIZONTAL };
        Gtk::Label _messageLabel1;

        std::vector<Gtk::Frame> _detailFrames;

        std::shared_ptr<Gtk::Button> _warpJumpTo;
        std::shared_ptr<mapPosition> _eventPosition, _warpTarget;
        std::shared_ptr<dropDown>    _eventType, _messageType1, _warpType, _itemType, _scriptType1,
            _scriptType2;
        std::shared_ptr<multiButton>     _eventTrigger;
        std::shared_ptr<Gtk::Adjustment> _selectedEventA, _aFlagA, _dFlagA, _messageIdx1A,
            _warpScriptIdxA, _scriptIdx1A, _scriptIdx2A, _flyLocationIdxA, _berryTreeIdxA;
        Gtk::SpinButton _selectedEventE, _aFlagE, _dFlagE, _messageIdx1E, _warpScriptIdxE,
            _scriptIdx1E, _scriptIdx2E, _berryTreeIdxE;
        std::shared_ptr<locationDropDown> _flyLocation;
        std::shared_ptr<itemSelector>     _item;

        std::shared_ptr<owSpriteSelector> _npcOWSprite;

      public:
        eventSelector( model& p_model, mapEditor& p_parent, root& p_root );

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
