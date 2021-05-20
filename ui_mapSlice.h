#pragma once
#include <functional>
#include <memory>

#include <gtkmm/gestureclick.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/image.h>
#include <gtkmm/overlay.h>
#include <gtkmm/widget.h>

#include "data_maprender.h"
#include "defines.h"

namespace UI {
    class mapSlice : public Gtk::Widget {
      public:
        enum clickType {
            LEFT   = GDK_BUTTON_PRIMARY,
            RIGHT  = GDK_BUTTON_SECONDARY,
            MIDDLE = GDK_BUTTON_MIDDLE
        };

      private:
        std::vector<DATA::computedBlock> _blocks;
        DATA::palette                    _pals[ 16 * 5 ] = { 0 };

        u16 _blocksPerRow;
        u16 _currentScale = 1;
        u16 _height;
        u16 _blockSpacing = 0;

        u8 _currentDaytime = 0;

        std::vector<std::shared_ptr<Gtk::Overlay>> _images;

        std::shared_ptr<Gtk::GestureClick> _clickEvent;
        std::shared_ptr<Gtk::GestureDrag>  _dragEvent;

      public:
        inline mapSlice( ) {
            _clickEvent = Gtk::GestureClick::create( );
            _clickEvent->set_propagation_phase( Gtk::PropagationPhase::CAPTURE );
            _clickEvent->set_button( 0 );
            add_controller( _clickEvent );

            _dragEvent = Gtk::GestureDrag::create( );
            _dragEvent->set_propagation_phase( Gtk::PropagationPhase::CAPTURE );
            _dragEvent->set_button( 0 );
            add_controller( _dragEvent );
        }
        virtual ~mapSlice( );

        virtual void connectDrag( const std::function<void( clickType, u16, u16 )>& p_start,
                                  const std::function<void( clickType, s16, s16 )>& p_update,
                                  const std::function<void( clickType, s16, s16 )>& p_end ) {
            _dragEvent->signal_drag_begin( ).connect( [ this, p_start ]( double p_x, double p_y ) {
                if( !_dragEvent->get_current_button( ) ) { return; }
                auto blockwd = _currentScale * DATA::BLOCK_SIZE + _blockSpacing;
                p_start( (clickType) _dragEvent->get_current_button( ), int( p_x / blockwd ),
                         int( p_y / blockwd ) );
            } );
            _dragEvent->signal_drag_update( ).connect( [ this, p_update ]( double p_x,
                                                                           double p_y ) {
                if( !_dragEvent->get_current_button( ) ) { return; }
                p_update( (clickType) _dragEvent->get_current_button( ), s16( p_x ), s16( p_y ) );
            } );
            _dragEvent->signal_drag_end( ).connect( [ this, p_end ]( double p_x, double p_y ) {
                if( !_dragEvent->get_current_button( ) ) { return; }
                p_end( (clickType) _dragEvent->get_current_button( ), s16( p_x ), s16( p_y ) );
            } );
        }

        virtual void connectClick( const std::function<void( clickType, u16, u16 )>& p_callback ) {
            _clickEvent->signal_pressed( ).connect(
                [ this, p_callback ]( int, double p_x, double p_y ) {
                    if( !_clickEvent->get_current_button( ) ) { return; }
                    auto blockwd = _currentScale * DATA::BLOCK_SIZE + _blockSpacing;
                    p_callback( (clickType) _clickEvent->get_current_button( ),
                                int( p_x / blockwd ), int( p_y / blockwd ) );
                } );
        }

        /*
         * @brief: Changes the block at the specified position to the specified block.
         * Aims to be faster than redrawing the whole map.
         */
        void updateBlock( const DATA::computedBlock& p_block, u16 p_x, u16 p_y );

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
