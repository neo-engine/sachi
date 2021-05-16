#pragma once
#include <memory>

#include <gtkmm/image.h>
#include <gtkmm/widget.h>

#include "data_maprender.h"
#include "defines.h"

namespace UI {
    class mapSlice : public Gtk::Widget {
        std::vector<DATA::computedBlock> _blocks;
        DATA::palette                    _pals[ 16 * 5 ] = { 0 };

        u16 _blocksPerRow;
        u16 _currentScale = 1;
        u16 _height;
        u16 _blockSpacing = 0;

        std::vector<std::shared_ptr<Gtk::Image>> _images;

      public:
        inline mapSlice( ) {
        }
        virtual ~mapSlice( );

        void set( const std::vector<DATA::computedBlock>& p_blocks, DATA::palette p_pals[ 5 * 16 ],
                  u16 p_blocksPerRow = DATA::SIZE );
        void setScale( u16 p_scale = 1 );
        void setSpacing( u16 p_blockSpacing = 0 );
        void redraw( u8 p_daytime = 0 );

      protected:
        Gtk::SizeRequestMode get_request_mode_vfunc( ) const override;
        void measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum, int& p_natural,
                            int& p_minimumBaseline, int& p_naturalBaseline ) const override;
        void size_allocate_vfunc( int p_width, int p_height, int p_baseline ) override;
    };
} // namespace UI
