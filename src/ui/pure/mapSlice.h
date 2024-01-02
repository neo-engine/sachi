#pragma once
#include <functional>
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/overlay.h>
#include <gtkmm/widget.h>

#include "../../data/maprender.h"
#include "../../defines.h"
#include "dropDown.h"
#include "util.h"

namespace UI {
    class mapSlice : public Gtk::Widget {
      public:
        enum clickType {
            LEFT   = GDK_BUTTON_PRIMARY,
            RIGHT  = GDK_BUTTON_SECONDARY,
            MIDDLE = GDK_BUTTON_MIDDLE
        };

        enum mark {
            MARK_WARP = 0,
            MARK_FLYPOS,
            MARK_SCRIPT,
            MARK_MESSAGE,
            MARK_SIGHT,
            MARK_MOVEMENT,
            MARK_BERRY,
            MARK_SIGHT_RED, // used for unfinished trainers
        };

      private:
        u16 _currentScale = 1;
        u16 _blockSpacing = 0;

        s16 _currentSelectionIndex = -1;

        double _overlayOpacity      = .3;
        double _marksOpacity        = .3;
        double _extraOverlayOpacity = 1.;
        bool   _showOverlay         = false;
        bool   _showMarks           = false;

        std::vector<std::shared_ptr<Gtk::Overlay>> _images;
        std::vector<std::shared_ptr<Gdk::Pixbuf>>  _imageData;
        std::vector<std::shared_ptr<Gtk::Label>>   _overlayMovement;
        std::vector<std::shared_ptr<Gtk::Label>>   _overlayMarks;
        std::vector<std::vector<mark>>             _marks;

        std::shared_ptr<Gtk::GestureClick> _clickEvent;
        std::shared_ptr<Gtk::GestureDrag>  _dragEvent;

        Gtk::Box _selectionBox;

      protected:
        virtual void                         redrawBlock( u16 p_blockIdx );
        virtual std::shared_ptr<Gdk::Pixbuf> computeImageData( u16 p_blockIdx )    = 0;
        virtual u8                           computeMovementData( u16 p_blockIdx ) = 0;
        virtual void updateBlockMovement( u8 p_oldValue, u8 p_movement, u16 p_x, u16 p_y );

        virtual inline bool colorMovement( ) const {
            return true;
        }

        std::shared_ptr<Gtk::Label> computeMarkLabel( u16 p_pos );

        virtual void computeMarkLabel( u16 p_pos, std::shared_ptr<Gtk::Label> p_label );

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

            _selectionBox = Gtk::Box( );
            _selectionBox.get_style_context( )->add_class( "mapblock-selected" );
            _selectionBox.set_size_request( _currentScale * DATA::BLOCK_SIZE - 4,
                                            _currentScale * DATA::BLOCK_SIZE - 4 );
            _currentSelectionIndex = -1;
        }
        virtual ~mapSlice( );

        virtual void addMark( s16 p_blockIdx, mark p_mark );

        virtual inline void addMark( u16 p_x, u16 p_y, mark p_mark ) {
            addMark( p_y * getWidth( ) + p_x, p_mark );
        }

        virtual void removeMark( s16 p_blockIdx, mark p_mark );

        virtual inline void removeMark( u16 p_x, u16 p_y, mark p_mark ) {
            removeMark( p_y * getWidth( ) + p_x, p_mark );
        }

        virtual void clearMarks( );

        virtual u16 getWidth( ) const  = 0;
        virtual u16 getHeight( ) const = 0;

        virtual void draw( );

        virtual void        selectBlock( s16 p_blockIdx );
        virtual inline void selectBlock( u16 p_blockX, u16 p_blockY ) {
            selectBlock( p_blockY * getWidth( ) + p_blockX );
        }

        virtual inline std::shared_ptr<Gdk::Pixbuf> getImageData( u16 p_blockIdx ) {
            if( p_blockIdx >= _imageData.size( ) ) { return nullptr; }
            return _imageData[ p_blockIdx ];
        }
        virtual inline std::shared_ptr<Gdk::Pixbuf> getImageData( u16 p_blockX, u16 p_blockY ) {
            return getImageData( p_blockY * getWidth( ) + p_blockX );
        }

        virtual inline void connectDrag( const std::function<void( clickType, u16, u16 )>& p_start,
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

        virtual inline void
        connectClick( const std::function<void( clickType, u16, u16 )>& p_callback ) {
            _clickEvent->signal_pressed( ).connect(
                [ this, p_callback ]( int, double p_x, double p_y ) {
                    if( !_clickEvent->get_current_button( ) ) { return; }
                    auto blockwd = _currentScale * DATA::BLOCK_SIZE + _blockSpacing;
                    p_callback( (clickType) _clickEvent->get_current_button( ),
                                int( p_x / blockwd ), int( p_y / blockwd ) );
                } );
        }

        virtual void setScale( u16 p_scale = 1 );
        virtual void setSpacing( u16 p_blockSpacing = 0 );
        virtual void setOverlayHidden( bool p_hidden = true );
        virtual void setMarksHidden( bool p_hidden = true );

        virtual void setOverlayOpacity( double p_newValue = .4 );
        virtual void setMarksOpacity( double p_newValue = .4 );

      protected:
        Gtk::SizeRequestMode get_request_mode_vfunc( ) const override;
        void measure_vfunc( Gtk::Orientation p_orientation, int, int& p_minimum, int& p_natural,
                            int& p_minimumBaseline, int& p_naturalBaseline ) const override;
        void size_allocate_vfunc( int p_width, int p_height, int p_baseline ) override;
    };

    /*
     * @brief: A concrete mapSlice widget that gets its image data via a look-up function
     * (with the intent that said lookup function does something smarter than just
     * rendering the block).
     */
    class lookupMapSlice : public mapSlice {
      private:
        std::vector<DATA::mapBlockAtom>                                   _blocks;
        std::function<std::shared_ptr<Gdk::Pixbuf>( DATA::mapBlockAtom )> _lookupFunction;

        u16 _blocksPerRow;
        u16 _height;

        inline u16 getWidth( ) const override {
            return _blocksPerRow;
        }
        inline u16 getHeight( ) const override {
            return _height;
        }

        inline std::shared_ptr<Gdk::Pixbuf> computeImageData( u16 p_blockIdx ) override {
            return _lookupFunction( _blocks[ p_blockIdx ] );
        }
        inline u8 computeMovementData( u16 p_blockIdx ) override {
            return u8( _blocks[ p_blockIdx ].m_movedata );
        }

      public:
        inline lookupMapSlice( ) : mapSlice( ) {
        }
        inline virtual ~lookupMapSlice( ) {
            mapSlice::~mapSlice( );
        }

        /*
         * @brief: Changes the block at the specified position to the specified block.
         * Aims to be faster than redrawing the whole map.
         */
        void updateBlock( const DATA::mapBlockAtom& p_block, u16 p_x, u16 p_y );
        void updateBlockMovement( u8 p_movement, u16 p_x, u16 p_y );

        void set( const std::vector<DATA::mapBlockAtom>& p_blocks,
                  const std::function<std::shared_ptr<Gdk::Pixbuf>( DATA::mapBlockAtom )>&
                      p_lookupFunction,
                  u16 p_blocksPerRow = DATA::SIZE );
    };

    /*
     * @brief: A concrete mapSlice widget that renders its blocks from specified
     * computedBlocks. Not very fast for large maps.
     */
    class computedMapSlice : public mapSlice {
      private:
        std::vector<std::pair<DATA::computedBlock, u8>> _blocks;
        DATA::palette                                   _pals[ 16 * 5 ] = { 0 };

        u16 _blocksPerRow;
        u16 _height;
        u8  _currentDaytime = 0;

        inline u16 getWidth( ) const override {
            return _blocksPerRow;
        }
        inline u16 getHeight( ) const override {
            return _height;
        }

        inline std::shared_ptr<Gdk::Pixbuf> computeImageData( u16 p_blockIdx ) override {
            return block::createImage( _blocks[ p_blockIdx ].first, _pals, _currentDaytime );
        }

        inline u8 computeMovementData( u16 p_blockIdx ) override {
            return _blocks[ p_blockIdx ].second;
        }

      public:
        inline computedMapSlice( ) : mapSlice( ) {
        }
        inline virtual ~computedMapSlice( ) {
            mapSlice::~mapSlice( );
        }

        inline void setDaytime( u8 p_currentDaytime ) {
            _currentDaytime = p_currentDaytime;
        }

        inline const DATA::computedBlock& getBlockData( u16 p_blockIdx ) const {
            return _blocks[ p_blockIdx ].first;
        }
        inline const DATA::computedBlock& getBlockData( u16 p_blockX, u16 p_blockY ) const {
            return getBlockData( p_blockY * getWidth( ) + p_blockX );
        }

        void updateBlock( const DATA::computedBlock& p_block, u16 p_x, u16 p_y );
        void updateBlockMovement( u8 p_movement, u16 p_x, u16 p_y );

        void set( const std::vector<std::pair<DATA::computedBlock, u8>>& p_blocks,
                  DATA::palette p_pals[ 5 * 16 ], u16 p_blocksPerRow = DATA::SIZE );
    };

    /*
     * @brief: A concrete mapSlice widget that renders a tile set
     */
    class tileSetMapSlice : public mapSlice {
      private:
        DATA::tileSet<1> _tiles;
        DATA::palette    _pals[ 16 * 5 ] = { 0 };

        u16 _tilesPerRow;
        u16 _height;
        u8  _currentDaytime = 0;
        u8  _selectedPal    = 0;

        inline u16 getWidth( ) const override {
            return _tilesPerRow;
        }
        inline u16 getHeight( ) const override {
            return _height;
        }

        inline std::shared_ptr<Gdk::Pixbuf> computeImageData( u16 p_blockIdx ) override {
            return tile::createImage( _tiles.m_tiles[ p_blockIdx ],
                                      _pals[ 16 * _currentDaytime + _selectedPal ], false, false );
        }

        inline u8 computeMovementData( u16 ) override {
            return 0;
        }

      public:
        inline void setOverlayHidden( bool = true ) override {
            return;
        }

        inline tileSetMapSlice( ) : mapSlice( ) {
        }

        inline virtual ~tileSetMapSlice( ) {
            mapSlice::~mapSlice( );
        }

        inline void setDaytime( u8 p_currentDaytime ) {
            _currentDaytime = p_currentDaytime;
        }

        inline void setPal( u8 p_pal ) {
            _selectedPal = p_pal;
        }

        inline void updateTile( const DATA::tile& p_tile, u16 p_x, u16 p_y ) {
            auto pos              = p_x + p_y * _tilesPerRow;
            _tiles.m_tiles[ pos ] = p_tile;
            redrawBlock( pos );
        }

        inline const DATA::tile& getTileData( u16 p_tileIdx ) const {
            return _tiles.m_tiles[ p_tileIdx ];
        }

        inline const DATA::tile& getTileData( u16 p_tileX, u16 p_tileY ) const {
            return getTileData( p_tileY * getWidth( ) + p_tileX );
        }

        void set( const DATA::tileSet<1>& p_tiles, DATA::palette p_pals[ 5 * 16 ],
                  u16 p_tilesPerRow = DATA::SIZE );
    };

    /*
     * @brief: A concrete mapSlice widget that renders a single tile (with every pixel
     * seperate)
     */
    class tileSlice : public mapSlice {
      public:
        static constexpr u8 TILE_SIZE = 8;

      private:
        DATA::tile    _tile;
        DATA::palette _pals[ 16 * 5 ] = { 0 };

        u8 _currentDaytime = 0;
        u8 _selectedPal    = 0;

        inline u16 getWidth( ) const override {
            return TILE_SIZE;
        }
        inline u16 getHeight( ) const override {
            return TILE_SIZE;
        }

        inline std::shared_ptr<Gdk::Pixbuf> computeImageData( u16 p_blockIdx ) override {
            return tile::createImage( _pals[ 16 * _currentDaytime + _selectedPal ]
                                          .m_pal[ computeMovementData( p_blockIdx ) ] );
        }

        inline u8 computeMovementData( u16 p_blockidx ) override {
            auto x{ p_blockidx % getWidth( ) };
            auto y{ p_blockidx / getWidth( ) };

            return _tile.at( x, y );
        }

        inline bool colorMovement( ) const override {
            return false;
        }

      public:
        inline tileSlice( ) : mapSlice( ) {
        }

        inline virtual ~tileSlice( ) {
            mapSlice::~mapSlice( );
        }

        inline void setDaytime( u8 p_currentDaytime ) {
            _currentDaytime = p_currentDaytime;
        }

        inline void setPal( u8 p_pal ) {
            _selectedPal = p_pal;
        }

        inline void updatePixel( u8 p_color, u16 p_x, u16 p_y ) {
            p_color &= 0xF;
            _tile.set( p_x, p_y, p_color );
            redrawBlock( p_y * getWidth( ) + p_x );
        }

        void set( const DATA::tile& p_tile, DATA::palette p_pals[ 5 * 16 ] );
    };

    /*
     * @brief: A concrete mapSlice widget that renders a vector of colors.
     */
    class colorSlice : public mapSlice {
      private:
        std::vector<u16> _data;

        u16 _colorsPerRow;
        u16 _height;

        inline u16 getWidth( ) const override {
            return _colorsPerRow;
        }
        inline u16 getHeight( ) const override {
            return _height;
        }

        inline std::shared_ptr<Gdk::Pixbuf> computeImageData( u16 p_blockIdx ) override {
            return tile::createImage( _data[ p_blockIdx ] );
        }

        inline u8 computeMovementData( u16 p_blockidx ) override {
            // return _data[ p_blockidx ];
            return p_blockidx;
        }

        inline bool colorMovement( ) const override {
            return false;
        }

      public:
        inline colorSlice( ) : mapSlice( ) {
        }

        inline virtual ~colorSlice( ) {
            mapSlice::~mapSlice( );
        }

        void set( const std::vector<u16>& p_colors, u16 p_colorsPerRow = 0 );
    };

} // namespace UI
