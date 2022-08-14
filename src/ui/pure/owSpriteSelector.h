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
     * @brief: A widget to select an overworld sprite
     */
    class owSpriteSelector {
      protected:
        model& _model;

        u16 _image = u16( -1 );
        u8  _frame = u8( -1 );

        bool _lock = false;

        bool _disableIdx  = false;
        bool _disableFIdx = false;

        Gtk::Box                         _outerBox{ Gtk::Orientation::HORIZONTAL };
        std::shared_ptr<Gtk::Adjustment> _owIdxA, _frameIdxA;
        Gtk::SpinButton                  _owIdx, _frameIdx;

        std::shared_ptr<fsImage<imageType::IT_SPRITE_ANIMATED>> _owImage;

      public:
        owSpriteSelector( model& p_model, bool p_allowFrameEdit = false );

        void setData( u16 p_spriteIdx, u8 p_frame = 0 );

        inline void setData( std::pair<u16, u8> p_data ) {
            setData( p_data.first, p_data.second );
        }

        virtual inline std::pair<u16, u8> getData( ) const {
            return { u16( _owIdx.get_value_as_int( ) ), u8( _frameIdx.get_value_as_int( ) ) };
        }

        virtual inline void connect( const std::function<void( std::pair<u16, u8> )>& p_callback ) {
            if( !p_callback ) { return; }

            _owIdx.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock || _disableIdx || _owIdx.get_value_as_int( ) == _image ) { return; }
                _disableIdx = true;
                setData( getData( ) );
                p_callback( getData( ) );
                _disableIdx = false;
            } );
            _frameIdx.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock || _disableIdx || _frameIdx.get_value_as_int( ) == _frame ) { return; }
                _disableFIdx = true;
                setData( getData( ) );
                p_callback( getData( ) );
                _disableFIdx = false;
            } );
        }

        inline operator Gtk::Widget&( ) {
            return _outerBox;
        }
    };
} // namespace UI
