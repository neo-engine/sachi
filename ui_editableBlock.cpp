#include <cstring>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/stringlist.h>

#include "ui_editableBlock.h"

namespace UI {
    std::vector<Glib::ustring> MAJOR_BEHAVES = { "00 None",
                                                 "01",
                                                 "02 Grass / Anim",
                                                 "03 Long Grass / Anim",
                                                 "04",
                                                 "05",
                                                 "06 Grass / No Anim",
                                                 "07",
                                                 "08 Grass / No Anim",
                                                 "09 (Long Grass Border)",
                                                 "0A No Bike",
                                                 "0B",
                                                 "0C",
                                                 "0D",
                                                 "0E",
                                                 "0F",
                                                 "10 (Water / Reflection)",
                                                 "11",
                                                 "12 Rock Climb",
                                                 "13 Waterfall",
                                                 "14",
                                                 "15 (Water)",
                                                 "16 (Water / Reflection)",
                                                 "17",
                                                 "18",
                                                 "19",
                                                 "1A",
                                                 "1B",
                                                 "1C",
                                                 "1D",
                                                 "1E",
                                                 "1F",
                                                 "20 Slide Player",
                                                 "21 (Footprints)",
                                                 "22",
                                                 "23",
                                                 "24 Ashen Grass / Anim",
                                                 "25",
                                                 "26 (Spolis Gym Ice Tile)",
                                                 "27 (Spolis Gym Crk Tile)",
                                                 "28 (Hot Spring)",
                                                 "29 Lavaridge Gym Warp Up",
                                                 "2A",
                                                 "2B",
                                                 "2C",
                                                 "2D",
                                                 "2E",
                                                 "2F",
                                                 "30 Blocked Right",
                                                 "31 Blocked Left",
                                                 "32 Blocked Up",
                                                 "33 Blocked Down",
                                                 "34",
                                                 "35",
                                                 "36 Blocked Down, Left",
                                                 "37 Blocked Down, Right",
                                                 "38 Jump Right",
                                                 "39 Jump Left",
                                                 "3A Jump Up",
                                                 "3B Jump Down",
                                                 "3C",
                                                 "3D",
                                                 "3E",
                                                 "3F",
                                                 "40 Move Right",
                                                 "41 Move Left",
                                                 "42 Move Up",
                                                 "43 Move Down",
                                                 "44 Slide Right",
                                                 "45 Slide Left",
                                                 "46 Slide Up",
                                                 "47 Slide Down",
                                                 "48 Slide Player",
                                                 "49",
                                                 "4A",
                                                 "4B",
                                                 "4C",
                                                 "4D",
                                                 "4E",
                                                 "4F",
                                                 "50 Run Right",
                                                 "51 Run Left",
                                                 "52 Run Up",
                                                 "53 Run Down",
                                                 "54",
                                                 "55",
                                                 "56",
                                                 "57",
                                                 "58",
                                                 "59",
                                                 "5A",
                                                 "5B",
                                                 "5C",
                                                 "5D",
                                                 "5E",
                                                 "5F",
                                                 "60 Warp Cave + Wk Dwn",
                                                 "61 Warp No Special",
                                                 "62 Warp When Right",
                                                 "63 Warp When Left",
                                                 "64 Warp When Up",
                                                 "65 Warp When Down",
                                                 "66 Warp Fall Through",
                                                 "67 (Warp Teleport Down)",
                                                 "68 Warp No Special",
                                                 "69 Warp Enter Door",
                                                 "6A (Warp PC Stairs Up)",
                                                 "6B (Warp PC Stairs Down)",
                                                 "6C Warp Emerge Water",
                                                 "6D Warp When Down",
                                                 "6E Warp then Walk Up",
                                                 "6F",
                                                 "70 (Warp Teleport Up)",
                                                 "71",
                                                 "72",
                                                 "73",
                                                 "74 (Pcflog Ver Log Up)",
                                                 "75 (Pcflog Ver Log Dwn)",
                                                 "76 (Pcflog Hor Log Lft)",
                                                 "77 (Pcflog Hor Log Rgt)",
                                                 "78 (Fortree Bridge)",
                                                 "79",
                                                 "7A",
                                                 "7B",
                                                 "7C",
                                                 "7D",
                                                 "7E",
                                                 "7F",
                                                 "80 Load Script 1B Behind",
                                                 "81",
                                                 "82",
                                                 "83 Access Storage System",
                                                 "84 (Access L. Btl Stats)",
                                                 "85 Access Map",
                                                 "86 Access TV",
                                                 "87 (PKBLOCK Feeder)",
                                                 "88",
                                                 "89 (Access Slot Machine)",
                                                 "8A (Access Roulette)",
                                                 "8B",
                                                 "8C (Lkd Door TrkHouse)",
                                                 "8D",
                                                 "8E",
                                                 "8F",
                                                 "90",
                                                 "91",
                                                 "92",
                                                 "93",
                                                 "94",
                                                 "95",
                                                 "96",
                                                 "97",
                                                 "98",
                                                 "99",
                                                 "9A",
                                                 "9B",
                                                 "9C",
                                                 "9D",
                                                 "9E",
                                                 "9F",
                                                 "A0 Only Walk",
                                                 "A1",
                                                 "A2",
                                                 "A3",
                                                 "A4",
                                                 "A5",
                                                 "A6",
                                                 "A7",
                                                 "A8",
                                                 "A9",
                                                 "AA",
                                                 "AB",
                                                 "AC",
                                                 "AD",
                                                 "AE",
                                                 "AF",
                                                 "B0",
                                                 "B1",
                                                 "B2",
                                                 "B3",
                                                 "B4",
                                                 "B5",
                                                 "B6",
                                                 "B7",
                                                 "B8",
                                                 "B9",
                                                 "BA",
                                                 "BB",
                                                 "BC",
                                                 "BD",
                                                 "BE",
                                                 "BF",
                                                 "C0 Block Up, Down",
                                                 "C1 Block Left, Right",
                                                 "C2",
                                                 "C3",
                                                 "C4",
                                                 "C5",
                                                 "C6",
                                                 "C7",
                                                 "C8",
                                                 "C9",
                                                 "CA",
                                                 "CB",
                                                 "CC",
                                                 "CD",
                                                 "CE",
                                                 "CF",
                                                 "D0 Fast Bike / Slide Down",
                                                 "D1",
                                                 "D2 Breakable Tile",
                                                 "D3 Bike Up/Down",
                                                 "D4 Bike Left/Right",
                                                 "D5 Bike Up/Down",
                                                 "D6 Bike Left/Right",
                                                 "D7",
                                                 "D8",
                                                 "D9",
                                                 "DA",
                                                 "DB",
                                                 "DC",
                                                 "DD",
                                                 "DE",
                                                 "DF",
                                                 "E0 Picture Books / M_S( 133 )",
                                                 "E1 Picture Books / M_S( 134 )",
                                                 "E2 PC Magazines / M_S( 30 )",
                                                 "E3 (Slateport Empty Vase)",
                                                 "E4 Empty Trash / M_S( 404 )",
                                                 "E5 Mart Shelves / M_S( 127 )",
                                                 "E6 Blue Prints / M_S( 396 )",
                                                 "E7 (PC Data Screen 1)",
                                                 "E8 (PC Data Screen 2)",
                                                 "E9",
                                                 "EA",
                                                 "EB",
                                                 "EC",
                                                 "ED",
                                                 "EE",
                                                 "EF",
                                                 "F0",
                                                 "F1",
                                                 "F2",
                                                 "F3",
                                                 "F4",
                                                 "F5",
                                                 "F6",
                                                 "F7",
                                                 "F8",
                                                 "F9",
                                                 "FA",
                                                 "FB",
                                                 "FC",
                                                 "FD",
                                                 "FE",
                                                 "FF" };

    std::vector<Glib::ustring> MINOR_BEHAVES = { "00 None",
                                                 "01",
                                                 "02",
                                                 "03",
                                                 "04",
                                                 "05",
                                                 "06",
                                                 "07",
                                                 "08",
                                                 "09",
                                                 "0A",
                                                 "0B",
                                                 "0C",
                                                 "0D",
                                                 "0E",
                                                 "0F",
                                                 "10 Hidden by Player",
                                                 "11",
                                                 "12",
                                                 "13",
                                                 "14",
                                                 "15",
                                                 "16",
                                                 "17",
                                                 "18",
                                                 "19",
                                                 "1A",
                                                 "1B",
                                                 "1C",
                                                 "1D",
                                                 "1E",
                                                 "1F",
                                                 "20",
                                                 "21",
                                                 "22",
                                                 "23",
                                                 "24",
                                                 "25",
                                                 "26",
                                                 "27",
                                                 "28",
                                                 "29",
                                                 "2A",
                                                 "2B",
                                                 "2C",
                                                 "2D",
                                                 "2E",
                                                 "2F",
                                                 "30",
                                                 "31",
                                                 "32",
                                                 "33",
                                                 "34",
                                                 "35",
                                                 "36",
                                                 "37",
                                                 "38",
                                                 "39",
                                                 "3A",
                                                 "3B",
                                                 "3C",
                                                 "3D",
                                                 "3E",
                                                 "3F",
                                                 "40",
                                                 "41",
                                                 "42",
                                                 "43",
                                                 "44",
                                                 "45",
                                                 "46",
                                                 "47",
                                                 "48",
                                                 "49",
                                                 "4A",
                                                 "4B",
                                                 "4C",
                                                 "4D",
                                                 "4E",
                                                 "4F",
                                                 "50",
                                                 "51",
                                                 "52",
                                                 "53",
                                                 "54",
                                                 "55",
                                                 "56",
                                                 "57",
                                                 "58",
                                                 "59",
                                                 "5A",
                                                 "5B",
                                                 "5C",
                                                 "5D",
                                                 "5E",
                                                 "5F",
                                                 "60",
                                                 "61",
                                                 "62",
                                                 "63",
                                                 "64",
                                                 "65",
                                                 "66",
                                                 "67",
                                                 "68",
                                                 "69",
                                                 "6A",
                                                 "6B",
                                                 "6C",
                                                 "6D",
                                                 "6E",
                                                 "6F",
                                                 "70",
                                                 "71",
                                                 "72",
                                                 "73",
                                                 "74",
                                                 "75",
                                                 "76",
                                                 "77",
                                                 "78",
                                                 "79",
                                                 "7A",
                                                 "7B",
                                                 "7C",
                                                 "7D",
                                                 "7E",
                                                 "7F",
                                                 "80",
                                                 "81",
                                                 "82",
                                                 "83",
                                                 "84",
                                                 "85",
                                                 "86",
                                                 "87",
                                                 "88",
                                                 "89",
                                                 "8A",
                                                 "8B",
                                                 "8C",
                                                 "8D",
                                                 "8E",
                                                 "8F",
                                                 "90",
                                                 "91",
                                                 "92",
                                                 "93",
                                                 "94",
                                                 "95",
                                                 "96",
                                                 "97",
                                                 "98",
                                                 "99",
                                                 "9A",
                                                 "9B",
                                                 "9C",
                                                 "9D",
                                                 "9E",
                                                 "9F",
                                                 "A0",
                                                 "A1",
                                                 "A2",
                                                 "A3",
                                                 "A4",
                                                 "A5",
                                                 "A6",
                                                 "A7",
                                                 "A8",
                                                 "A9",
                                                 "AA",
                                                 "AB",
                                                 "AC",
                                                 "AD",
                                                 "AE",
                                                 "AF",
                                                 "B0",
                                                 "B1",
                                                 "B2",
                                                 "B3",
                                                 "B4",
                                                 "B5",
                                                 "B6",
                                                 "B7",
                                                 "B8",
                                                 "B9",
                                                 "BA",
                                                 "BB",
                                                 "BC",
                                                 "BD",
                                                 "BE",
                                                 "BF",
                                                 "C0",
                                                 "C1",
                                                 "C2",
                                                 "C3",
                                                 "C4",
                                                 "C5",
                                                 "C6",
                                                 "C7",
                                                 "C8",
                                                 "C9",
                                                 "CA",
                                                 "CB",
                                                 "CC",
                                                 "CD",
                                                 "CE",
                                                 "CF",
                                                 "D0",
                                                 "D1",
                                                 "D2",
                                                 "D3",
                                                 "D4",
                                                 "D5",
                                                 "D6",
                                                 "D7",
                                                 "D8",
                                                 "D9",
                                                 "DA",
                                                 "DB",
                                                 "DC",
                                                 "DD",
                                                 "DE",
                                                 "DF",
                                                 "E0",
                                                 "E1",
                                                 "E2",
                                                 "E3",
                                                 "E4",
                                                 "E5",
                                                 "E6",
                                                 "E7",
                                                 "E8",
                                                 "E9",
                                                 "EA",
                                                 "EB",
                                                 "EC",
                                                 "ED",
                                                 "EE",
                                                 "EF",
                                                 "F0",
                                                 "F1",
                                                 "F2",
                                                 "F3",
                                                 "F4",
                                                 "F5",
                                                 "F6",
                                                 "F7",
                                                 "F8",
                                                 "F9",
                                                 "FA",
                                                 "FB",
                                                 "FC",
                                                 "FD",
                                                 "FE",
                                                 "FF" };

    const DATA::palette DUMMY_PAL[ 5 * 16 ] = { };

    std::shared_ptr<Gtk::Image> createImage( const DATA::computedBlockAtom& p_tile,
                                             const DATA::palette p_pals[ 5 * 16 ] = DUMMY_PAL,
                                             u8                  p_daytime        = 0 ) {
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

    editableTiles::editableTiles( ) {
        for( u8 y = 0; y < DATA::BLOCK_SIZE / DATA::TILE_SIZE; ++y ) {
            for( u8 x = 0; x < DATA::BLOCK_SIZE / DATA::TILE_SIZE; ++x ) {
                auto im = createImage( _tiles[ y ][ x ] );
                im->set_size_request( DATA::TILE_SIZE, DATA::TILE_SIZE );
                im->set_parent( *this );
                _images.push_back( im );
            }
        }
    }

    void editableTiles::setTiles(
        const DATA::computedBlockAtom p_tiles[ DATA::BLOCK_SIZE / DATA::TILE_SIZE ]
                                             [ DATA::BLOCK_SIZE / DATA::TILE_SIZE ] ) {
        for( u8 x = 0; x < DATA::BLOCK_SIZE / DATA::TILE_SIZE; ++x ) {
            for( u8 y = 0; y < DATA::BLOCK_SIZE / DATA::TILE_SIZE; ++y ) {
                setTile( x, y, p_tiles[ y ][ x ] );
            }
        }
    }

    void editableTiles::setTile( u8 p_x, u8 p_y, const DATA::computedBlockAtom& p_tile ) {
        _tiles[ p_y ][ p_x ] = p_tile;

        size_t imidx = p_y * DATA::BLOCK_SIZE / DATA::TILE_SIZE + p_x;
        if( imidx < _images.size( ) ) {
            auto& oim = _images[ imidx ];
            oim->unparent( );
            oim = createImage( p_tile, _pals, _daytime );
            oim->set_size_request( DATA::TILE_SIZE, DATA::TILE_SIZE );
            oim->set_parent( *this );
        }
    }

    void editableTiles::setScale( u16 p_scale ) {
        if( p_scale ) { _mapScale = p_scale; }
    }

    void editableTiles::setSpacing( u16 p_blockSpacing ) {
        _mapSpacing = p_blockSpacing;
    }

    void editableTiles::redraw( DATA::palette p_pals[ 5 * 16 ], u8 p_daytime ) {
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
        _daytime = p_daytime;
        setTiles( _tiles );
    }

    Gtk::SizeRequestMode editableTiles::get_request_mode_vfunc( ) const {
        return Gtk::SizeRequestMode::CONSTANT_SIZE;
    }

    void editableTiles::measure_vfunc( Gtk::Orientation, int, int& p_minimum, int& p_natural,
                                       int& p_minimumBaseline, int& p_naturalBaseline ) const {
        p_minimumBaseline = -1;
        p_naturalBaseline = -1;

        auto sz   = DATA::BLOCK_SIZE / DATA::TILE_SIZE;
        p_minimum = sz * _mapScale * DATA::TILE_SIZE + ( sz - 1 ) * _mapSpacing;
        p_natural = sz * _mapScale * DATA::TILE_SIZE + ( sz - 1 ) * _mapSpacing;
    }

    void editableTiles::size_allocate_vfunc( int p_width, int p_height, int p_baseline ) {
        if( _images.empty( ) ) { return; }

        (void) p_width;
        (void) p_height;

        // make sure bordering slices stay glued together, even if we get surplus space
        for( size_t y = 0; y < DATA::BLOCK_SIZE / DATA::TILE_SIZE; ++y ) {
            for( size_t x = 0; x < DATA::BLOCK_SIZE / DATA::TILE_SIZE; ++x ) {
                if( y * DATA::BLOCK_SIZE / DATA::TILE_SIZE + x >= _images.size( ) ) { continue; }

                auto& im = _images[ y * DATA::BLOCK_SIZE / DATA::TILE_SIZE + x ];
                if( im == nullptr ) [[unlikely]] { continue; }

                // make dummy calls to measure to suppress warnings (yes we do know how big
                // every block should be.)
                int ignore;
                im->measure( Gtk::Orientation::HORIZONTAL, -1, ignore, ignore, ignore, ignore );

                Gtk::Allocation allo;

                u16 sx = x * _mapScale * DATA::TILE_SIZE;
                u16 sy = y * _mapScale * DATA::TILE_SIZE;
                sx += x * _mapSpacing;
                sy += y * _mapSpacing;

                allo.set_x( sx );
                allo.set_y( sy );
                auto width  = _mapScale * DATA::TILE_SIZE;
                auto height = _mapScale * DATA::TILE_SIZE;
                allo.set_width( width );
                allo.set_height( height );
                im->size_allocate( allo, p_baseline );
            }
        }
    }

    tileInfo::tileInfo( ) {
        std::vector<Glib::ustring> pals = {
            "Pal 0 (TS1/0)",  "Pal 1 (TS1/1)",  "Pal 2 (TS1/2)",     "Pal 3 (TS1/3)",
            "Pal 4 (TS1/4)",  "Pal 5 (TS1/5)",  "Pal 6 (TS2/0)",     "Pal 7 (TS2/1)",
            "Pal 8 (TS2/2)",  "Pal 9 (TS2/3)",  "Pal 10 (TS2/4)",    "Pal 11 (TS2/5)",
            "Pal 12 (TS2/6)", "Pal 13 (TS2/7)", "(Pal 14) (unused)", "(Pal 15) (unused)",
        };
        auto sl = Gtk::StringList::create( pals );

        _palette.set_model( sl );

        _palette.property_selected_item( ).signal_changed( ).connect( [ this ]( ) {
            if( _noTrigger || _palette.get_selected( ) == GTK_INVALID_LIST_POSITION ) { return; }

            _tile.m_palno = _palette.get_selected( );
            redraw( _pals, _daytime );
        } );

        auto mainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );

        _outerFrame.set_child( mainBox );
        _outerFrame.set_label_align( Gtk::Align::CENTER );

        auto lbox  = Gtk::Box( Gtk::Orientation::VERTICAL );
        auto rbox  = Gtk::Box( Gtk::Orientation::VERTICAL );
        auto rhbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );

        mainBox.set_margin( MARGIN );
        mainBox.set_spacing( MARGIN );
        mainBox.set_valign( Gtk::Align::CENTER );
        mainBox.set_hexpand( true );

        lbox.set_spacing( MARGIN );

        _tileImage = createImage( _tile );
        lbox.append( *_tileImage );
        _tileImage->set_size_request( _mapScale * DATA::TILE_SIZE, _mapScale * DATA::TILE_SIZE );

        rbox.set_valign( Gtk::Align::CENTER );
        rbox.set_hexpand( true );

        mainBox.append( lbox );
        mainBox.append( rbox );

        rbox.append( _tileName );
        rbox.append( _palette );
        lbox.append( rhbox );

        rhbox.append( _flipX );
        _flipX.set_label( "X Flip" );
        _flipX.property_active( ).signal_changed( ).connect( [ this ]( ) {
            if( _noTrigger ) { return; }
            _tile.m_vflip = _flipX.get_active( );
            redraw( _pals, _daytime );
        } );

        rhbox.append( _flipY );
        _flipY.set_label( "Y Flip" );
        _flipY.property_active( ).signal_changed( ).connect( [ this ]( ) {
            if( _noTrigger ) { return; }
            _tile.m_hflip = _flipY.get_active( );
            redraw( _pals, _daytime );
        } );

        _tileName.set_margin_bottom( MARGIN );

        char buffer[ 50 ];
        snprintf( buffer, 49, "Tile 0x%03hX / %04hu", _currentTileIdx, _currentTileIdx );
        _tileName.set_text( std::string( buffer ) );
    }

    void tileInfo::setTile( const DATA::computedBlockAtom& p_tile, u16 p_tileIdx ) {
        _noTrigger      = true;
        _tile           = p_tile;
        _currentTileIdx = p_tileIdx;

        _flipX.set_active( p_tile.m_vflip );
        _flipY.set_active( p_tile.m_hflip );
        _palette.set_selected( p_tile.m_palno );

        _noTrigger = false;
    }

    void tileInfo::redraw( DATA::palette p_pals[ 5 * 16 ], u8 p_daytime ) {
        std::memcpy( _pals, p_pals, sizeof( _pals ) );
        _daytime   = p_daytime;
        _tileImage = createImage( _tile, p_pals, p_daytime );
    }

    void tileInfo::setScale( u16 p_scale ) {
        _mapScale = p_scale;
        _tileImage->set_size_request( _mapScale * DATA::TILE_SIZE, _mapScale * DATA::TILE_SIZE );
    }

    editableBlock::editableBlock( ) {
        auto sl1 = Gtk::StringList::create( MAJOR_BEHAVES );
        auto sl2 = Gtk::StringList::create( MINOR_BEHAVES );

        _majorBehave.set_model( sl1 );
        _minorBehave.set_model( sl2 );

        auto mainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );

        _outerFrame.set_child( mainBox );
        _outerFrame.set_label_align( Gtk::Align::CENTER );

        auto lbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto rbox = Gtk::Box( Gtk::Orientation::VERTICAL );

        mainBox.set_margin( MARGIN );
        mainBox.set_spacing( MARGIN );
        mainBox.set_valign( Gtk::Align::CENTER );
        mainBox.set_hexpand( true );

        lbox.set_spacing( MARGIN );
        rbox.get_style_context( )->add_class( "linked" );
        rbox.set_valign( Gtk::Align::CENTER );
        rbox.set_hexpand( true );

        mainBox.append( lbox );
        mainBox.append( rbox );

        for( u8 i = 0; i < DATA::BLOCK_LAYERS; ++i ) {
            auto f = Gtk::Frame( );
            if( i == 0 ) {
                f.set_label( "Top" );
            } else if( i == DATA::BLOCK_LAYERS - 1 ) {
                f.set_label( "Bottom" );
            } else {
                f.set_label( "Middle" );
            }

            f.set_child( _tiles[ i ] );
            _tiles[ i ].set_margin( MARGIN );
            _tiles[ i ].set_margin_top( 0 );
            _tiles[ i ].set_halign( Gtk::Align::CENTER );
            _tiles[ i ].set_valign( Gtk::Align::CENTER );

            f.set_label_align( Gtk::Align::CENTER );
            lbox.append( f );
        }

        rbox.append( _blockName );
        rbox.append( _majorBehave );
        rbox.append( _minorBehave );

        _blockName.set_margin_bottom( MARGIN );

        char buffer[ 50 ];
        snprintf( buffer, 49, "Block 0x%03hX / %04hu", _currentBlockIdx, _currentBlockIdx );
        _blockName.set_text( std::string( buffer ) );
    }

    void editableBlock::setBlock( const DATA::computedBlock& p_block, u16 p_blockIdx ) {
        _noTrigger = true;
        _tiles[ 0 ].setTiles( p_block.m_top );
        _tiles[ 1 ].setTiles( p_block.m_bottom );

        _majorBehave.set_selected( p_block.m_bottombehave );
        _minorBehave.set_selected( p_block.m_topbehave );
        _noTrigger = false;

        _currentBlockIdx = p_blockIdx;

        char buffer[ 50 ];
        snprintf( buffer, 49, "Block 0x%03hX / %04hu", p_blockIdx, p_blockIdx );
        _blockName.set_text( std::string( buffer ) );
    }

    void editableBlock::updateTile( u8 p_layer, u8 p_x, u8 p_y,
                                    const DATA::computedBlockAtom& p_tile ) {
        _tiles[ p_layer ].setTile( p_x, p_y, p_tile );
    }

    void editableBlock::setScale( u16 p_scale ) {
        for( u8 i = 0; i < DATA::BLOCK_LAYERS; ++i ) { _tiles[ i ].setScale( p_scale ); }
    }
    void editableBlock::setSpacing( u16 p_blockSpacing ) {
        for( u8 i = 0; i < DATA::BLOCK_LAYERS; ++i ) { _tiles[ i ].setSpacing( p_blockSpacing ); }
    }

    void editableBlock::redraw( DATA::palette p_pals[ 5 * 16 ], u8 p_daytime ) {
        for( u8 i = 0; i < DATA::BLOCK_LAYERS; ++i ) { _tiles[ i ].redraw( p_pals, p_daytime ); }
    }

} // namespace UI
