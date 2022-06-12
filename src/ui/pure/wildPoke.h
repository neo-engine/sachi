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
#include "multiButton.h"
#include "pokeSelector.h"

namespace UI {
    /*
     * @brief: A widget to draw a single wild poke encounter data
     */
    class wildPoke {
      protected:
        model& _model;

        bool _lock = false;

        u8   _maxFormes;
        bool _allowDuplication;

        Gtk::Frame                       _outerFrame;
        Gtk::Label                       _formeLabel;
        std::shared_ptr<Gtk::Adjustment> _encRateA, _reqBadgesA;
        Gtk::SpinButton                  _encRate, _reqBadges;
        Gtk::Button                      _upB, _downB, _duplicateB, _removeB;
        std::shared_ptr<multiButton>     _encTimes;
        std::shared_ptr<pokeSelector>    _pkmnChooser;

      public:
        wildPoke( model& p_model );

        /*
         * @brief: reload list of pkmn names from fs and correspondinly update the widget
         */
        inline void refreshModel( ) {
            if( _pkmnChooser ) { _pkmnChooser->refreshModel( ); }
        }

        void setData( const DATA::mapData::wildPkmnData& p_data );

        virtual inline DATA::mapData::wildPkmnData getData( ) const {
            auto pkmnVal
                = _pkmnChooser ? _pkmnChooser->getData( ) : std::make_pair<u16, u8>( 0, 0 );

            return { pkmnVal.first,
                     pkmnVal.second,
                     DATA::wildPkmnType::GRASS,
                     u8( _reqBadges.get_value( ) ),
                     u8( _encTimes ? _encTimes->currentChoice( ) : 0 ),
                     u8( _encRate.get_value( ) ) };
        }

        virtual inline void
        connect( const std::function<void( DATA::mapData::wildPkmnData )>& p_callback ) {
            if( !p_callback ) { return; }

            if( _pkmnChooser ) {
                _pkmnChooser->connect( [ this, p_callback ]( pokeSelector::pkmnDscr ) {
                    if( _lock ) { return; }
                    p_callback( getData( ) );
                } );
            }

            if( _encTimes ) {
                _encTimes->connect( [ this, p_callback ]( u32 ) {
                    if( _lock ) { return; }
                    p_callback( getData( ) );
                } );
            }

            _encRateA->signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( getData( ) );
            } );
            _reqBadgesA->signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( getData( ) );
            } );

            // TODO
        }

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }
    };
} // namespace UI
