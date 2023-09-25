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
     * @brief: A widget to select an item
     */
    class itemSelector {
      protected:
        model& _model;

        u16 _image = u16( -1 );

        bool _lock = false;

        bool _disableIdx = false;

        Gtk::Box                             _outerBox{ Gtk::Orientation::HORIZONTAL };
        std::shared_ptr<Gtk::Adjustment>     _itemIdxA;
        Gtk::SpinButton                      _itemIdx;
        std::shared_ptr<stringCacheDropDown> _itemChooser;

        std::shared_ptr<fsImage<imageType::IT_SPRITE_ICON_32x32>> _itemImage;

      public:
        itemSelector( model&           p_model,
                      Gtk::Orientation p_orientation = Gtk::Orientation::HORIZONTAL );

        /*
         * @brief: reload list of item names from fs and correspondinly update the widget
         */
        inline void refreshModel( ) {
            if( _itemChooser ) { _itemChooser->refreshModel( _model.itemNames( ) ); }
            if( _itemIdxA ) { _itemIdxA->set_upper( _model.maxItem( ) ); }
        }

        void setData( u16 p_data );

        virtual inline u16 getData( ) const {
            return u16( _itemIdx.get_value( ) );
        }

        virtual inline void connect( const std::function<void( u16 )>& p_callback ) {
            if( !p_callback ) { return; }

            _itemIdx.signal_value_changed( ).connect( [ this, p_callback ]( ) {
                if( _lock || _disableIdx || getData( ) == _image ) { return; }
                _disableIdx = true;
                setData( getData( ) );
                p_callback( getData( ) );
                _disableIdx = false;
            } );
            if( _itemChooser ) {
                _itemChooser->connect( [ this, p_callback ]( u64 p_newChoice ) {
                    setData( p_newChoice );
                    p_callback( getData( ) );
                } );
            }
        }

        inline operator Gtk::Widget&( ) {
            return _outerBox;
        }
    };
} // namespace UI
