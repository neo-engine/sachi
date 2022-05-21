#include <cstring>

#include "../../root.h"
#include "paletteEditor.h"

namespace UI::TED {
    paletteEditor::paletteEditor( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        Gtk::Box mbox{ Gtk::Orientation::VERTICAL };
        mbox.set_margin( MARGIN );
        mbox.set_spacing( MARGIN );
        mbox.set_vexpand( );
        mbox.set_valign( Gtk::Align::CENTER );

        _frame.set_child( mbox );

        for( u8 y{ 0 }; y < 4; ++y ) {
            Gtk::Box box{ Gtk::Orientation::HORIZONTAL };
            box.set_spacing( MARGIN );
            box.set_halign( Gtk::Align::CENTER );
            for( u8 x{ 0 }; x < 4; ++x ) {
                auto c{ 4 * y + x };

                char buffer[ 50 ];
                snprintf( buffer, 49, "%02hu/%X", c, c );

                auto ec = std::make_shared<editableColor>( buffer );
                if( !ec ) { continue; }

                ec->connect( [ this, c ]( u16 p_newColor ) { setColor( c, p_newColor ); } );

                _colors.push_back( ec );
                box.append( *ec );
            }
            mbox.append( box );
        }
    }

    void paletteEditor::redraw( ) {
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _model.buildPalette( pals, _model.m_settings.m_tseBS1, _model.m_settings.m_tseBS2 );

        for( u8 c{ 0 }; c < 16; ++c ) {
            if( c >= _colors.size( ) || !_colors[ c ] ) { break; }

            _colors[ c ]->set(
                pals[ 16 * _model.m_settings.m_tseDayTime + _model.m_settings.m_tseSelectedPalette ]
                    .m_pal[ c ] );
        }
    }

    void paletteEditor::setColor( u8 p_idx, u16 p_color ) {
        u8 pal, ts;
        if( 6 <= _model.m_settings.m_tseSelectedPalette
            && _model.m_settings.m_tseSelectedPalette < 14 ) { // TS2
            pal = _model.m_settings.m_tseSelectedPalette - 6;
            ts  = _model.m_settings.m_tseBS2;
        } else { // TS1
            if( _model.m_settings.m_tseSelectedPalette >= 14 ) {
                pal = _model.m_settings.m_tseSelectedPalette - 14 + 6;
            } else {
                pal = _model.m_settings.m_tseSelectedPalette;
            }
            ts = _model.m_settings.m_tseBS1;
        }
        _model.m_fsdata.m_blockSets[ ts ]
            .m_pals[ 8 * _model.m_settings.m_tseDayTime + pal ]
            .m_pal[ p_idx ]
            = p_color;

        _model.markTileSetsChanged( );
        _rootWindow.redraw( );
    }
} // namespace UI::TED
