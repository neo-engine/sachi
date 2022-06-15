#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>

#include "../../../data/maprender.h"
#include "../../root.h"
#include "encList.h"

namespace UI::MED {
    encList::encList( model& p_model, root& p_root ) : _model{ p_model }, _rootWindow( p_root ) {
        _outerFrame.set_vexpand( );

        Gtk::ScrolledWindow sw{ };
        _outerFrame.append( sw );
        sw.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );

        sw.set_child( _encBox );

        for( u8 slot{ 0 }; slot < DATA::MAX_PKMN_PER_SLICE; ++slot ) {
            auto wd = std::make_shared<wildPoke>( _model, _rootWindow, slot );
            if( !wd ) { continue; }
            _encData.push_back( wd );
            _encBox.append( *wd );

            wd->connect(
                [ this, slot ]( ) {
                    if( slot ) {
                        std::swap( _model.mapData( ).m_pokemon[ slot - 1 ],
                                   _model.mapData( ).m_pokemon[ slot ] );
                        _model.markSelectedBankChanged( );
                        _rootWindow.redraw( );
                    }
                },
                [ this, slot ]( ) {
                    if( slot + 1 < DATA::MAX_PKMN_PER_SLICE ) {
                        std::swap( _model.mapData( ).m_pokemon[ slot + 1 ],
                                   _model.mapData( ).m_pokemon[ slot ] );
                        _model.markSelectedBankChanged( );
                        _rootWindow.redraw( );
                    }
                },
                [ this, slot ]( ) {
                    _model.mapData( ).m_pokemon[ slot ] = DATA::mapData::wildPkmnData{ };
                    _model.markSelectedBankChanged( );
                    _rootWindow.redraw( );
                } );
        }
    }
} // namespace UI::MED
