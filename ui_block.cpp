#include <gdkmm/pixbuf.h>

#include "data_bitmap.h"
#include "ui_block.h"

namespace UI {
    int                         cnt = 100;
    std::shared_ptr<Gtk::Image> block::createImage( const DATA::computedBlock& p_block,
                                                    const DATA::palette* p_palette, u8 p_daytime ) {
        auto btm = new DATA::bitmap( DATA::BLOCK_SIZE, DATA::BLOCK_SIZE );
        DATA::renderBlock( &p_block, p_palette, btm, 0, 0, 1, p_daytime );

        //        btm->writeToFile( ( "/tmp/" + std::to_string( cnt++ ) + ".png" ).c_str( ) );

        auto pixbuf = btm->pixbuf( );

        auto res = std::make_shared<Gtk::Image>( );
        res->set( pixbuf );

        delete btm;
        return res;
    }
} // namespace UI
