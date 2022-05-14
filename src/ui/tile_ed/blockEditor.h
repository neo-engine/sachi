#pragma once
#include <memory>
#include <vector>

#include <gtkmm/box.h>

#include "../../defines.h"
#include "../../model.h"
#include "../pure/switchButton.h"

namespace UI {
    class root;

    namespace TED {
        class actionBar;
        class blockSelector;
        class tileSelector;
        /*
        class paletteSelector;
        class paletteInformation;
        class tileCanvas;
        class paletteEditor;
        */
    } // namespace TED

    /*
     * @brief: Widget for the map editor that allows editing blocks and other settings of
     * a single map slice.
     */
    class blockEditor {
      public:
        enum tseDisplayMode : u8 {
            TSEMODE_EDIT_BLOCKS,
            TSEMODE_EDIT_TILES,
            TSEMODE_EDIT_PALETTES,
        };

      private:
        model& _model;
        root&  _rootWindow;

        tseDisplayMode _currentDisplayMode;

        // sub widgets
        Gtk::Box _mainBox{ Gtk::Orientation::VERTICAL };

        std::shared_ptr<switchButton>   _modeToggles;
        Gtk::Box                        _contentMainBox{ Gtk::Orientation::HORIZONTAL };
        std::shared_ptr<TED::actionBar> _actionBar;

        // column 1: block selector
        Gtk::Box                            _col1MainBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<TED::blockSelector> _blockPicker;

        // column 2: block info / tile selector
        Gtk::Box                           _col2MainBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<TED::tileSelector> _tilePicker;

        // column 3: tile editor
        Gtk::Box _col3MainBox{ Gtk::Orientation::VERTICAL };
        // std::shared_ptr<TED::tileCanvas>    _tileCanvas;

        // column 4: palette selector
        Gtk::Box _col4MainBox{ Gtk::Orientation::VERTICAL };
        // std::shared_ptr<TED::paletteSelector>    _palPicker;

        // column 5: palette editor
        Gtk::Box _col5MainBox{ Gtk::Orientation::VERTICAL };
        // std::shared_ptr<TED::paletteEditor>    _palEditor;

      public:
        blockEditor( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        void redraw( );

        /*
         * @brief: Sets the mode of the tile set editor
         */
        void setNewEditMode( tseDisplayMode p_newMode );
    };
} // namespace UI
