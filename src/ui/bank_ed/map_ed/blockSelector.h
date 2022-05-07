#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/frame.h>

#include "../../data/maprender.h"
#include "../editableBlock.h"
#include "mapBankOverview.h"
#include "mapSlice.h"

namespace UI::MED {
    /*
     * @brief: Widget to select a block from a blockset
     */
    class blockSelector {
        Gtk::Frame       _blockSetFrame;
        Gtk::Box         _mapEditorBlockSetBox{ Gtk::Orientation::VERTICAL };
        Gtk::DropDown    _mapEditorBS1CB, _mapEditorBS2CB; // select BS1/BS2
        computedMapSlice _ts1widget, _ts2widget;

        std::shared_ptr<Gtk::StringList>                _mapBankStrList; // block set names
        std::vector<std::pair<DATA::computedBlock, u8>> _currentBlockset1;
        std::vector<std::pair<DATA::computedBlock, u8>> _currentBlockset2;
        std::map<u8, blockSetInfo>                      _blockSets;
        std::set<u8>                                    _blockSetNames;

      public:
        blockSelector( );

        inline operator Gtk::Widget&( ) {
            return _blockSetFrame;
        }
    };
} // namespace UI::MED
