#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>

#include "../../defines.h"
#include "../../model.h"
#include "../switchButton.h"

namespace UI {
    class root;

    namespace MED {
        class actionBar;
        class blockSelector;
        class editableMap;
    } // namespace MED

    /*
     * @brief: Widget for the map editor that allows editing blocks and other settings of
     * a single map slice.
     */
    class mapEditor {
      public:
        enum mapDisplayMode : u8 {
            MODE_EDIT_TILES,
            MODE_EDIT_MOVEMENT,
            MODE_EDIT_LOCATIONS,
            MODE_EDIT_EVENTS,
            MODE_EDIT_PKMN,
            MODE_EDIT_DATA,
        };

      private:
        model& _model;
        root&  _rootWindow;

        mapDisplayMode _currentMapDisplayMode; // current widget stateg

        // sub widgets
        Gtk::Box _mapEditorMainBox{ Gtk::Orientation::VERTICAL };

        std::shared_ptr<switchButton> _mapEditorModeToggles;

        Gtk::Box _mapContentMainBox{ Gtk::Orientation::HORIZONTAL };

        // left side
        Gtk::Box                          _mapMainBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<MED::editableMap> _edMap;
        // std::shared_ptr<MED::wpokeData> _wpoke;
        // std::shared_ptr<MED::metaData> _meta;
        std::shared_ptr<MED::actionBar> _actionBar;

        // right side
        Gtk::Box _sideBox{ Gtk::Orientation::VERTICAL };

        std::shared_ptr<MED::blockSelector> _blockPicker;
        // std::shared_ptr<MED::movementSelector> _mvmtPicker;
        // std::shared_ptr<MED::evtPicker> _evtPicker;
        // std::shared_ptr<MED::locPicker> _locPicker;

      public:
        mapEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mapEditorMainBox;
        }

        void redraw( );

        /*
         * @brief: Sets the mode of the map editor (edit blocks, movements, events, etc)
         */
        void setNewMapEditMode( mapDisplayMode p_newMode );

        /*
         * @brief: Looks up the computed image data of the block sets.
         */
        std::shared_ptr<Gdk::Pixbuf> blockSetLookup( u16 p_blockIdx );
    };
} // namespace UI
