#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/popover.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/togglebutton.h>

#include "../../defines.h"
#include "../../model.h"

namespace UI {
    /*
     * @brief: Widget for selecting a single option from a set of options.
     */
    class dropDown {
      protected:
        sigc::connection _conn;

        std::vector<std::string> _choices;
        u64                      _currentSelection = 0;

        Gtk::ToggleButton   _dropDown;
        Gtk::Label          _selectedText;
        Gtk::ScrolledWindow _sw;

        Gtk::Box                                  _popoverBox{ Gtk::Orientation::VERTICAL };
        std::vector<std::shared_ptr<Gtk::Button>> _popoverButtons;
        std::vector<Gtk::Image>                   _popoverBtnSelect;
        Gtk::Popover                              _popover;

        std::function<void( u64 )> _callback;

        inline void updateLabel( u64 p_idx ) {
            _selectedText.set_text( _choices[ p_idx ] );
        }

      public:
        dropDown( const std::vector<std::string>& p_choices, u64 p_defaultChoice = 0 );

        virtual ~dropDown( ) {
            _popover.unparent( );
        }

        void connect( const std::function<void( u64 )>& p_choiceChangedCallback );

        inline operator Gtk::Widget&( ) {
            return _dropDown;
        }

        inline std::shared_ptr<Gtk::Widget> pointer( ) {
            // this is as evil as it gets. I know.
            return std::shared_ptr<Gtk::Widget>( &_dropDown );
        }

        inline u64 currentChoice( ) const {
            return _currentSelection;
        }

        inline void choose( u64 p_choice ) {
            if( p_choice >= _choices.size( ) ) { return; }
            updateLabel( p_choice );
            for( auto& i : _popoverBtnSelect ) { i.hide( ); }
            _currentSelection = p_choice;
            _popoverBtnSelect[ _currentSelection ].show( );
        }

        inline void setMaxWidth( u16 p_charWidth ) {
            _selectedText.set_width_chars( p_charWidth );
        }
    };

    class pkmnDropDown : public dropDown {
        u16 _lastRefresh = 0;

      public:
        pkmnDropDown( ) : dropDown( { }, 0 ) {
        }

        /*
         * @brief: update the available choices
         */
        void refreshModel( model& p_model );
    };

    class locationDropDown : public dropDown {
        u16 _lastRefresh = 0;

      public:
        locationDropDown( ) : dropDown( { }, 0 ) {
        }

        /*
         * @brief: update the available choices
         */
        void refreshModel( model& p_model );
    };

} // namespace UI
