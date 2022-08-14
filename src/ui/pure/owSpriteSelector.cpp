#include <gtkmm/image.h>
#include "owSpriteSelector.h"

namespace UI {
    owSpriteSelector::owSpriteSelector( model& p_model, bool p_allowFrameEdit )
        : _model{ p_model }, _owIdxA{ Gtk::Adjustment::create( 0.0, 0.0, u16( -1 ), 1.0, 50.0,
                                                               0.0 ) },
          _frameIdxA{ Gtk::Adjustment::create( 0.0, 0.0, 15.0, 1.0, 50.0, 0.0 ) },
          _owIdx{ _owIdxA }, _frameIdx{ _frameIdxA } {

        _owImage = std::make_shared<fsImage<imageType::IT_SPRITE_ANIMATED>>( );
        if( _owImage ) {
            _owImage->set_valign( Gtk::Align::CENTER );
            _owImage->set_margin_end( MARGIN );
            _outerBox.append( *_owImage );
        }

        Gtk::Box selBox{ Gtk::Orientation::HORIZONTAL };
        _outerBox.get_style_context( )->add_class( "linked" );
        _outerBox.set_valign( Gtk::Align::CENTER );
        selBox.set_valign( Gtk::Align::CENTER );

        selBox.append( _owIdx );
        selBox.append( _frameIdx );
        _frameIdx.set_visible( p_allowFrameEdit );
        selBox.get_style_context( )->add_class( "linked" );

        _owIdx.set_hexpand( );
        _frameIdx.set_hexpand( );

        selBox.set_hexpand( );

        _outerBox.append( selBox );
    }

    void owSpriteSelector::setData( u16 p_spriteIdx, u8 p_frame ) {
        _lock = true;

        if( _owImage && ( _image != p_spriteIdx || _frame != p_frame ) ) {
            if( p_spriteIdx > DATA::PKMN_SPRITE ) {
                _owImage->load( _model.m_fsdata.owSpritePath( p_spriteIdx ), p_frame );
            } else {
                _owImage->load( _model.m_fsdata.owSpritePath( p_spriteIdx | 256 ), p_frame );
            }
            _owImage->setScale( 2 );
            _image = p_spriteIdx;
            _frame = p_frame;
        }

        if( !_disableIdx ) { _owIdx.set_value( p_spriteIdx ); }
        if( !_disableFIdx ) { _frameIdx.set_value( p_frame ); }
        _lock = false;
    }
} // namespace UI
