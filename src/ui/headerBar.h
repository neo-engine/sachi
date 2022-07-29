#pragma once
#include <memory>

#include <giomm/menu.h>
#include <giomm/menuitem.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/popovermenu.h>

#include "../defines.h"
#include "../model.h"

namespace UI {
    class root;

    class headerBar {
        model&  _model;
        context _context = CONTEXT_NONE;

        root& _rootWindow;

        Gtk::HeaderBar _headerBar;

        std::shared_ptr<Gtk::Button>     _saveButton, _openButton;
        std::shared_ptr<Gtk::MenuButton> _openMenuButton, _saveMenuButton;
        std::shared_ptr<Gtk::MenuButton> _openMenuTSButton, _saveMenuTSButton;
        Gtk::PopoverMenu                 _openMenuPopover, _saveMenuPopover;
        Gtk::PopoverMenu                 _openMenuTSPopover, _saveMenuTSPopover;
        std::shared_ptr<Gio::Menu>       _openMenu, _saveMenu;
        std::shared_ptr<Gio::Menu>       _openMenuTS, _saveMenuTS;

        Gtk::Label _titleLabel, _subtitleLabel;

      public:
        headerBar( model& p_model, root& p_root );

        inline void switchContext( context p_context ) {
            _context = p_context;
            redraw( );
        }

        void redraw( );

        inline operator Gtk::Widget&( ) {
            return _headerBar;
        }

        /*
         * @brief: Sets the title displayed on the header bar.
         */
        void setTitle( const std::string& p_windowTitle = "", const std::string& p_mainTitle = "",
                       const std::string& p_subTitle = "", bool p_redraw = false );
    };
} // namespace UI
