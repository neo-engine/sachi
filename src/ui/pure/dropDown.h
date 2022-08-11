#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/popover.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/spinbutton.h>
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

        virtual void connect( const std::function<void( u64 )>& p_choiceChangedCallback );

        virtual inline operator Gtk::Widget&( ) {
            return _dropDown;
        }

        inline u64 currentChoice( ) const {
            return _currentSelection;
        }

        virtual inline void choose( u64 p_choice ) {
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
        u16  _lastRefresh = 0;
        bool _disableE    = false;

        std::shared_ptr<Gtk::Adjustment> _locationA;
        Gtk::SpinButton                  _locationE;
        Gtk::Box                         _mainBox;

      public:
        locationDropDown( Gtk::Orientation p_orientation = Gtk::Orientation::VERTICAL );

        inline operator Gtk::Widget&( ) override {
            return _mainBox;
        }

        void connect( const std::function<void( u64 )>& p_choiceChangedCallback ) override;

        inline void choose( u64 p_choice ) override {
            if( p_choice >= _choices.size( ) ) { return; }
            updateLabel( p_choice );
            for( auto& i : _popoverBtnSelect ) { i.hide( ); }
            _currentSelection = p_choice;
            _popoverBtnSelect[ _currentSelection ].show( );

            if( !_disableE ) { _locationE.set_value( p_choice ); }
        }

        /*
         * @brief: update the available choices
         */
        void refreshModel( model& p_model );
    };

} // namespace UI
