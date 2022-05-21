#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "../../data/bitmap.h"
#include "util.h"

namespace UI {
    std::shared_ptr<Gtk::Button> createButton( const std::string&     p_iconName,
                                               const std::string&     p_labelText,
                                               std::function<void( )> p_callback ) {
        auto hbox = Gtk::Box( Gtk::Orientation::HORIZONTAL, 5 );
        if( p_iconName != "" ) {
            auto icon = Gtk::Image( );
            hbox.append( icon );
            icon.set_from_icon_name( p_iconName );
        }
        if( p_labelText != "" ) {
            auto label = Gtk::Label( p_labelText );
            label.set_expand( true );
            label.set_use_underline( );
            hbox.append( label );
        }

        auto resultButton = std::make_shared<Gtk::Button>( );
        resultButton->set_child( hbox );
        resultButton->signal_clicked( ).connect(
            [ = ]( ) {
                p_callback( );
                resultButton->grab_focus( );
            },
            false );
        return resultButton;
    }

    std::shared_ptr<Gdk::Pixbuf> block::createImage( const DATA::computedBlock& p_block,
                                                     const DATA::palette*       p_palette,
                                                     u8                         p_daytime ) {
        auto btm = new DATA::bitmap( DATA::BLOCK_SIZE, DATA::BLOCK_SIZE );
        DATA::renderBlock( &p_block, p_palette, btm, 0, 0, 1, p_daytime );
        auto pixbuf = btm->pixbuf( );
        delete btm;
        return pixbuf;
    }

    std::shared_ptr<Gdk::Pixbuf> tile::createImage( const DATA::tile&    p_tile,
                                                    const DATA::palette& p_palette, bool p_flipX,
                                                    bool p_flipY ) {
        auto btm = new DATA::bitmap( DATA::TILE_SIZE, DATA::TILE_SIZE );
        DATA::renderTile( &p_tile, &p_palette, p_flipX, p_flipY, btm, 0, 0, 1 );
        auto pixbuf = btm->pixbuf( );
        delete btm;
        return pixbuf;
    }

    std::shared_ptr<Gtk::Image> tile::createImage( const DATA::computedBlockAtom& p_tile,
                                                   const DATA::palette            p_pals[ 5 * 16 ],
                                                   u8                             p_daytime ) {
        auto btm = new DATA::bitmap( DATA::TILE_SIZE, DATA::TILE_SIZE );

        DATA::renderTile( &p_tile.m_tile, &p_pals[ 16 * p_daytime + p_tile.m_palno ],
                          p_tile.m_vflip, p_tile.m_hflip, btm );

        //    btm->writeToFile( ( "/tmp/" + std::to_string( cnt++ ) + ".png" ).c_str( ) );

        auto pixbuf = btm->pixbuf( );
        auto res    = std::make_shared<Gtk::Image>( );
        res->set( pixbuf );

        delete btm;
        return res;
    }

    std::shared_ptr<Gdk::Pixbuf> tile::createImage( u16 p_color ) {
        auto btm         = new DATA::bitmap( 1, 1 );
        ( *btm )( 0, 0 ) = DATA::pixel( red( p_color ), green( p_color ), blue( p_color ) );
        //    btm->writeToFile( ( "/tmp/" + std::to_string( cnt++ ) + ".png" ).c_str( ) );

        auto pixbuf = btm->pixbuf( );
        delete btm;
        return pixbuf;
    }

} // namespace UI
