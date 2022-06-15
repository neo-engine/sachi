#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include "../../data/maprender.h"
#include "../../model.h"
#include "dropDown.h"
#include "fsImage.h"
#include "multiButton.h"

namespace UI {
    /*
     * @brief: A widget to select a pkmn species
     */
    class pokeSelector {
      public:
        using pkmnDscr = std::pair<u16, u8>;

      protected:
        model& _model;

        pkmnDscr _image = { u16( -1 ), u8( -1 ) };

        bool _lock = false;

        u8 _maxFormes;

        Gtk::Box                         _outerBox{ Gtk::Orientation::HORIZONTAL };
        std::shared_ptr<Gtk::Adjustment> _pkmnIdxA, _pkmnFormeA;
        Gtk::SpinButton                  _pkmnIdx, _pkmnForme;
        std::shared_ptr<pkmnDropDown>    _pkmnChooser;

        std::shared_ptr<fsImage<imageType::IT_SPRITE_PKMN>> _pkmnImage;

      public:
        pokeSelector( model& p_model );

        /*
         * @brief: reload list of pkmn names from fs and correspondinly update the widget
         */
        inline void refreshModel( ) {
            if( _pkmnChooser ) { _pkmnChooser->refreshModel( _model ); }
            if( _pkmnIdxA ) { _pkmnIdxA->set_upper( _model.maxPkmn( ) ); }
        }

        void setData( pkmnDscr p_data );

        virtual inline pkmnDscr getData( ) const {
            return { u16( _pkmnIdx.get_value( ) ), u8( _pkmnForme.get_value( ) ) };
        }

        virtual inline void connect( const std::function<void( pkmnDscr )>& p_callback ) {
            if( !p_callback ) { return; }

            _pkmnIdxA->signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                auto [ i, unused ] = getData( );
                setData( { i, 0 } );
                p_callback( getData( ) );
            } );
            _pkmnFormeA->signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                setData( getData( ) );
                p_callback( getData( ) );
            } );
            if( _pkmnChooser ) {
                _pkmnChooser->connect( [ this, p_callback ]( u64 p_newChoice ) {
                    setData( { p_newChoice, 0 } );
                    p_callback( getData( ) );
                } );
            }
        }

        inline operator Gtk::Widget&( ) {
            return _outerBox;
        }
    };
} // namespace UI
