#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/spinbutton.h>

#include "../../../data/maprender.h"
#include "../../../model.h"
#include "../../pure/dropDown.h"
#include "../../pure/multiButton.h"
#include "../../pure/pokeSelector.h"

namespace UI {
    class root;
}

namespace UI::MED {
    /*
     * @brief: A widget to draw a single wild poke encounter data
     */
    class wildPoke {
        static inline std::string classForEncType( DATA::wildPkmnType p_type ) {
            switch( p_type ) {
            default: return "";
            case DATA::GRASS: return "encounter-grass";
            case DATA::HIGH_GRASS: return "encounter-long-grass";
            case DATA::WATER: return "encounter-water";
            case DATA::OLD_ROD: return "encounter-old-rod";
            case DATA::GOOD_ROD: return "encounter-good-rod";
            case DATA::SUPER_ROD: return "encounter-super-rod";
            case DATA::HEADBUTT: return "encounter-headbutt";
            case DATA::ROCK_SMASH: return "encounter-rock-smash";
            case DATA::SWEET_SCENT: return "encounter-sweet-scent";
            case DATA::POKE_TORE: return "encounter-radar";
            case DATA::SWARM: return "encounter-swarm";
            }
        }

      protected:
        model& _model;
        root&  _rootWindow;
        u8     _slot;

        bool _lock = false;

        u8                               _maxFormes;
        Gtk::Frame                       _outerFrame;
        Gtk::Label                       _formeLabel;
        std::shared_ptr<Gtk::Adjustment> _encRateA, _reqBadgesA;
        Gtk::SpinButton                  _encRate, _reqBadges;
        Gtk::Button                      _upB, _downB, _duplicateB, _removeB;
        std::shared_ptr<multiButton>     _encTimes;
        std::shared_ptr<pokeSelector>    _pkmnChooser;
        std::shared_ptr<dropDown>        _encType;
        Gtk::Box                         _encBox{ Gtk::Orientation::VERTICAL };
        Gtk::Separator                   _s1{ };

      public:
        wildPoke( model& p_model, root& p_root, u8 p_slot );

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
                     DATA::wildPkmnType( _encType ? _encType->currentChoice( ) : 0 ),
                     u8( _reqBadges.get_value( ) ),
                     u8( _encTimes ? _encTimes->currentChoice( ) : 0 ),
                     u8( _encRate.get_value( ) ) };
        }

        virtual inline void connect( const std::function<void( )>& p_up,
                                     const std::function<void( )>& p_down,
                                     const std::function<void( )>& p_delete ) {
            if( p_up ) {
                _upB.signal_clicked( ).connect( [ this, p_up ]( ) {
                    p_up( );
                    _upB.grab_focus( );
                } );
            }
            if( p_down ) {
                _downB.signal_clicked( ).connect( [ this, p_down ]( ) {
                    p_down( );
                    _downB.grab_focus( );
                } );
            }
            if( p_delete ) {
                _removeB.signal_clicked( ).connect( [ this, p_delete ]( ) {
                    p_delete( );
                    _removeB.grab_focus( );
                } );
            }
        }

        void redraw( );

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }
    };
} // namespace UI::MED
