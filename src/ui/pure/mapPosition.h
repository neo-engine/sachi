#pragma once

#include <functional>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>

#include "../../data/maprender.h"
#include "../../defines.h"
#include "../../model.h"
#include "switchButton.h"

namespace UI {
    /*
     * @brief: A widget to display / select a map position (x:[0,31], y:[0,31], z:[0,15]),
     * optionally with an extra selector for (mapBank, mapX, mapY).
     *
     * +-----+-----------------+---+---+
     * | Map |        ##       | + | - |
     * +-----+----+---+---+----+---+---+
     * | X   | ## | + | - | ## | + | - |
     * +-----+----+---+---+----+---+---+
     * | Y   | ## | + | - | ## | + | - |
     * +-----+----+---+---+----+---+---+
     * | Z   |        ##       | + | - |
     * +-----+-----------------+---+---+
     * |+--------------+--------------+|
     * ||  Local Coord | Global Coord ||
     * |+--------------+--------------+|
     * +-------------------------------+
     */
    class mapPosition {
      protected:
        enum class displayMode { LOCAL_POSITION, GLOBAL_POSITION };

        displayMode _currentDisplayMode = displayMode::LOCAL_POSITION;
        bool        _lock               = false;

        Gtk::Box                         _mainBox{ Gtk::Orientation::VERTICAL };
        std::shared_ptr<Gtk::Adjustment> _bankA, _mapXA, _localXA, _globalXA, _mapYA, _localYA,
            _globalYA, _posZA;
        Gtk::SpinButton _bankE, _mapXE, _localXE, _globalXE, _mapYE, _localYE, _globalYE, _posZE;
        Gtk::Box _localXB{ Gtk::Orientation::HORIZONTAL }, _localYB{ Gtk::Orientation::HORIZONTAL };

        std::shared_ptr<switchButton> _modeSwitch;

        inline void recomputePosition( displayMode p_dataSource ) {
            bool ol = _lock;
            _lock   = true;
            switch( p_dataSource ) {
            case displayMode::LOCAL_POSITION: {
                _globalXE.set_value( _mapXE.get_value_as_int( ) * DATA::SIZE
                                     + _localXE.get_value_as_int( ) );
                _globalYE.set_value( _mapYE.get_value_as_int( ) * DATA::SIZE
                                     + _localYE.get_value_as_int( ) );

                if( !_mapXE.get_editable( ) ) {
                    // if map should not be editable, make sure it isn't
                    _globalXA->set_lower( _mapXE.get_value_as_int( ) * DATA::SIZE );
                    _globalYA->set_lower( _mapYE.get_value_as_int( ) * DATA::SIZE );
                    _globalXA->set_upper( ( _mapXE.get_value_as_int( ) + 1 ) * DATA::SIZE - 1 );
                    _globalYA->set_upper( ( _mapYE.get_value_as_int( ) + 1 ) * DATA::SIZE - 1 );
                }

                break;
            }
            case displayMode::GLOBAL_POSITION: {
                _mapXE.set_value( _globalXE.get_value_as_int( ) / DATA::SIZE );
                _localXE.set_value( _globalXE.get_value_as_int( ) % DATA::SIZE );
                _mapYE.set_value( _globalYE.get_value_as_int( ) / DATA::SIZE );
                _localYE.set_value( _globalYE.get_value_as_int( ) % DATA::SIZE );

                break;
            }
            }
            _lock = ol;
        }

      public:
        mapPosition( bool p_allowSliceEdit = false );

        inline void setDisplayMode( displayMode p_newMode ) {
            // recompute data to ensure that displayed data is correct
            recomputePosition( _currentDisplayMode );

            _currentDisplayMode = p_newMode;
            switch( p_newMode ) {
            case displayMode::LOCAL_POSITION: {
                if( _modeSwitch ) { _modeSwitch->choose( 0 ); }
                _globalXE.hide( );
                _globalYE.hide( );
                _localXB.show( );
                _localYB.show( );
                break;
            }
            case displayMode::GLOBAL_POSITION: {
                if( _modeSwitch ) { _modeSwitch->choose( 1 ); }
                _globalXE.show( );
                _globalYE.show( );
                _localXB.hide( );
                _localYB.hide( );
                break;
            }
            }
        }

        inline void setPosition( DATA::warpPos p_newPos ) {
            _lock = true;
            _bankE.set_value( p_newPos.first );

            _globalXA->set_lower( p_newPos.second.m_posX / DATA::SIZE * DATA::SIZE );
            _globalYA->set_lower( p_newPos.second.m_posY / DATA::SIZE * DATA::SIZE );
            _globalXA->set_upper( ( p_newPos.second.m_posX / DATA::SIZE + 1 ) * DATA::SIZE - 1 );
            _globalYA->set_upper( ( p_newPos.second.m_posY / DATA::SIZE + 1 ) * DATA::SIZE - 1 );

            _globalXE.set_value( p_newPos.second.m_posX );
            _globalYE.set_value( p_newPos.second.m_posY );
            _posZE.set_value( p_newPos.second.m_posZ );
            recomputePosition( displayMode::GLOBAL_POSITION );
            _lock = false;
        }

        inline DATA::warpPos getPosition( ) const {
            switch( _currentDisplayMode ) {
            default:
            case displayMode::LOCAL_POSITION: {
                return { (u8) _bankE.get_value_as_int( ),
                         DATA::position::fromLocal(
                             _mapXE.get_value_as_int( ), _localXE.get_value_as_int( ),
                             _mapYE.get_value_as_int( ), _localYE.get_value_as_int( ),
                             _posZE.get_value_as_int( ) ) };
            }
            case displayMode::GLOBAL_POSITION: {
                return { (u8) _bankE.get_value_as_int( ),
                         { (u16) _globalXE.get_value_as_int( ), (u16) _globalYE.get_value_as_int( ),
                           (u8) _posZE.get_value_as_int( ) } };
            }
            }
        }

        virtual inline void connect( const std::function<void( )>& p_callback ) {
            if( !p_callback ) { return; }

            _bankE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _mapXE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _localXE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _globalXE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _mapYE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _localYE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _globalYE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
            _posZE.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock ) { return; }
                p_callback( );
            } );
        }

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }
    };
} // namespace UI
