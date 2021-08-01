#pragma once

#include <functional>

#include <gtkmm/dropdown.h>
#include <gtkmm/frame.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/image.h>
#include <gtkmm/widget.h>

#include "data_maprender.h"
#include "defines.h"

namespace UI {
    class editableTiles : public Gtk::Widget {
      public:
        enum clickType {
            LEFT_DOUBLE  = GDK_BUTTON_PRIMARY,
            RIGHT_DOUBLE = GDK_BUTTON_SECONDARY,
        };

      protected:
        DATA::computedBlockAtom _tiles[ DATA::BLOCK_SIZE / DATA::TILE_SIZE ]
                                      [ DATA::BLOCK_SIZE / DATA::TILE_SIZE ]
            = { {} };
        std::shared_ptr<Gtk::GestureClick> _clickEvent;

        std::vector<std::shared_ptr<Gtk::Image>> _images;

        u16 _mapScale   = 4;
        u16 _mapSpacing = 1;
        u8  _daytime    = 0;

        DATA::palette _pals[ 5 * 16 ] = { };

      public:
        editableTiles( );

        virtual inline ~editableTiles( ) {
            for( auto& im : _images ) { im->unparent( ); }
        }

        void
        setTiles( const DATA::computedBlockAtom p_tiles[ DATA::BLOCK_SIZE / DATA::TILE_SIZE ]
                                                       [ DATA::BLOCK_SIZE / DATA::TILE_SIZE ] );

        void setTile( u8 p_x, u8 p_y, const DATA::computedBlockAtom& p_tile );

        virtual inline void
        connectClick( const std::function<void( clickType, u16, u16 )>& p_callback ) {
            _clickEvent->signal_pressed( ).connect(
                [ this, p_callback ]( int p_numPresses, double p_x, double p_y ) {
                    if( !_clickEvent->get_current_button( ) || p_numPresses <= 1 ) { return; }
                    auto blockwd = _mapScale * DATA::TILE_SIZE + _mapSpacing;
                    p_callback( (clickType) _clickEvent->get_current_button( ),
                                int( p_x / blockwd ), int( p_y / blockwd ) );
                } );
        }

        void setScale( u16 p_scale = 1 );
        void setSpacing( u16 p_blockSpacing = 0 );

        void redraw( DATA::palette p_pals[ 5 * 16 ], u8 p_daytime );

      protected:
        Gtk::SizeRequestMode get_request_mode_vfunc( ) const override;
        void measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum, int& p_natural,
                            int& p_minimumBaseline, int& p_naturalBaseline ) const override;
        void size_allocate_vfunc( int p_width, int p_height, int p_baseline ) override;
    };

    /*
     * @brief: A widget to display and edit a single block
     */
    class editableBlock {
      protected:
        Gtk::Frame    _outerFrame;
        Gtk::DropDown _majorBehave;
        Gtk::DropDown _minorBehave;

        editableTiles _tiles[ DATA::BLOCK_LAYERS ];

      public:
        editableBlock( );

        virtual inline ~editableBlock( ) {
        }

        void setBlock( const DATA::computedBlock& p_block );

        void updateTile( u8 p_layer, u8 p_x, u8 p_y, const DATA::computedBlockAtom& p_tile );

        virtual inline void connect( const std::function<void( u8 )>& p_majBehave,
                                     const std::function<void( u8 )>& p_minBehave ) {
            if( !p_majBehave || !p_minBehave ) { return; }

            _majorBehave.property_selected_item( ).signal_changed( ).connect(
                [ this, p_majBehave ]( ) {
                    if( _majorBehave.get_selected( ) == GTK_INVALID_LIST_POSITION ) { return; }
                    try {
                        p_majBehave( _majorBehave.get_selected( ) );
                    } catch( ... ) { return; }
                } );
            _minorBehave.property_selected_item( ).signal_changed( ).connect(
                [ this, p_minBehave ]( ) {
                    if( _minorBehave.get_selected( ) == GTK_INVALID_LIST_POSITION ) { return; }
                    try {
                        p_minBehave( _minorBehave.get_selected( ) );
                    } catch( ... ) { return; }
                } );
        }

        virtual inline void
        connectClick( u8                                                               p_layer,
                      const std::function<void( editableTiles::clickType, u16, u16 )>& p_tiles ) {
            if( !p_tiles ) { return; }
            _tiles[ p_layer ].connectClick( p_tiles );
        }

        void setScale( u16 p_scale = 1 );
        void setSpacing( u16 p_blockSpacing = 0 );

        inline operator Gtk::Widget&( ) {
            return _outerFrame;
        }

        void redraw( DATA::palette p_pals[ 5 * 16 ], u8 p_daytime );
    };
} // namespace UI
