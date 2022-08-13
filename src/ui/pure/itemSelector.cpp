#include <gtkmm/image.h>
#include "itemSelector.h"

namespace UI {
    itemSelector::itemSelector( model& p_model )
        : _model{ p_model }, _itemIdxA{ Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 50.0, 0.0 ) },
          _itemIdx{ _itemIdxA } {

        _itemImage = std::make_shared<fsImage<imageType::IT_SPRITE_ICON_32x32>>( );
        if( _itemImage ) {
            _itemImage->set_valign( Gtk::Align::CENTER );
            _itemImage->set_margin_end( MARGIN );
            _outerBox.append( *_itemImage );
        }

        Gtk::Box selBox{ Gtk::Orientation::VERTICAL };
        _outerBox.get_style_context( )->add_class( "linked" );
        _outerBox.set_valign( Gtk::Align::CENTER );
        selBox.set_valign( Gtk::Align::CENTER );

        selBox.append( _itemIdx );
        _itemChooser = std::make_shared<stringCacheDropDown>( );
        if( _itemChooser ) { selBox.append( *_itemChooser ); }
        selBox.get_style_context( )->add_class( "linked" );

        selBox.set_hexpand( );

        _outerBox.append( selBox );
    }

    void itemSelector::setData( u16 p_data ) {
        _lock = true;

        if( _itemImage && _image != p_data ) {
            _itemImage->load( std::string( "@" ) + std::to_string( p_data ) + "@"
                              + _model.m_fsdata.itemSpritePath( ) );
            _image = p_data;
        }

        if( _itemChooser ) { _itemChooser->choose( p_data ); }
        if( !_disableIdx ) { _itemIdx.set_value( p_data ); }
        _lock = false;
    }
} // namespace UI
