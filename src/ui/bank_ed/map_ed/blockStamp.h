#pragma once
/*
#include <map>
#include <memory>
#include <set>
#include <tuple>

#include <giomm/menu.h>
#include <giomm/menuitem.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/iconview.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/notebook.h>
#include <gtkmm/popovermenu.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/window.h>
*/

#include "../../data/maprender.h"
#include "../editableBlock.h"
#include "mapBankOverview.h"
#include "mapSlice.h"

namespace UI::MED {
    /*
     * @brief: A Dialog window displaying block stamp data
     */
    class blockStamp {
        std::vector<DATA::mapBlockAtom> _blockStampData;
        std::shared_ptr<Gtk::Dialog>    _blockStampDialog;

        lookupMapSlice _blockStampMap;
        u16            _blockStampWidth;
        bool           _blockStampDialogInvalid = true;

      public:
        blockStamp( );

        std::shared_ptr<Gtk::Dialog> getDialog( ) {
            return _blockStampDialog;
        }

        void reset( );

        void create( );
    };
} // namespace UI::MED
