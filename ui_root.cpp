#include <cstring>
#include <filesystem>

#include <gtkmm/cssprovider.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/multiselection.h>
#include <gtkmm/notebook.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/signallistitemfactory.h>

#include "data_fs.h"
#include "defines.h"
#include "ui_block.h"
#include "ui_root.h"

namespace UI {
    const std::string TITLE_STRING = std::string( "Sachi " VERSION );

    auto root::createButton( const std::string& p_iconName, const std::string& p_labelText,
                             std::function<void( )> p_callback ) {
        auto icon = Gtk::Image( );
        icon.set_from_icon_name( p_iconName );
        auto label = Gtk::Label( p_labelText );
        label.set_expand( true );
        label.set_use_underline( );

        auto hbox = Gtk::Box( Gtk::Orientation::HORIZONTAL, 5 );
        hbox.append( icon );
        hbox.append( label );

        // auto resultButton = Gtk::make_managed<Gtk::Button>( );
        auto resultButton = std::make_shared<Gtk::Button>( );
        resultButton->set_child( hbox );
        resultButton->signal_clicked( ).connect( p_callback, false );
        return resultButton;
    }

    root::root( ) {
        set_title( TITLE_STRING );
        set_default_size( 800, 600 );

        auto provider = Gtk::CssProvider::create( );
        provider->load_from_data( EXTRA_CSS );
        Gtk::StyleContext::add_provider_for_display( Gdk::Display::get_default( ), provider,
                                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );

        auto headerBar = Gtk::HeaderBar( );
        set_titlebar( headerBar );

        _openButton = createButton( "document-open", "_Load FSROOT",
                                    [ & ]( ) { this->onFsRootOpenClick( ); } );
        _saveButton = createButton( "document-save", "_Save Changes",
                                    [ & ]( ) { this->onFsRootSaveClick( ); } );

        headerBar.pack_start( *_openButton );
        headerBar.pack_start( *_saveButton );

        _saveButton->hide( );

        _mainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        set_child( _mainBox );

        auto lFrame = Gtk::Frame( );
        lFrame.set_margin( MARGIN );
        _mainBox.append( lFrame );

        auto lScrolledWindow = Gtk::ScrolledWindow( );
        lFrame.set_child( lScrolledWindow );
        // _mainBox.set_shrink_start_child( false );
        lScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );

        _mapBankBox = Gtk::Box( Gtk::Orientation::VERTICAL, MARGIN );
        _mapBankBox.set_margin( MARGIN );
        lScrolledWindow.set_child( _mapBankBox );
        _mapBankBox.set_vexpand( true );

        _addMapBank = std::make_shared<addMapBank>( );
        _mapBankBox.append( *_addMapBank );

        if( _addMapBank ) {
            _addMapBank->connect(
                [ this ]( u16 p_bk, u8 p_y, u8 p_x ) { createMapBank( p_bk, p_y, p_x ); } );
        }

        _mapBanks.clear( );
        _selectedBank = -1;

        _loadMapLabel = Gtk::Label( );
        _loadMapLabel.set_markup( "<span size=\"x-large\">Add or load a map bank!</span>" );
        _loadMapLabel.set_expand( );
        _mainBox.append( _loadMapLabel );

        _mapNotebook.set_margin( MARGIN );
        _mapNotebook.set_expand( );
        _mapNotebook.hide( );
        auto mapEditorPaned = Gtk::Paned( Gtk::Orientation::HORIZONTAL );
        mapEditorPaned.set_margin( MARGIN );

        _mapNotebook.append_page( mapEditorPaned, "Map _Editor", true );
        _mapNotebook.append_page( _mapOverviewBox, "Bank _Overview", true );
        _mainBox.append( _mapNotebook );
        _mainBox.hide( );

        mapEditorPaned.set_start_child( _mapEditorMapBox );
        mapEditorPaned.set_shrink_start_child( false );
        _mapEditorMapBox.set_expand( );
        mapEditorPaned.set_end_child( _mapEditorBlockSetBox );

        auto bsselbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        bsselbox.set_margin( MARGIN );
        bsselbox.set_halign( Gtk::Align::CENTER );

        auto bsself = Gtk::Label( "Blocksets" );
        bsself.set_margin( MARGIN );
        _mapEditorBlockSetBox.append( bsself );

        auto emptystr   = std::vector<Glib::ustring>( );
        _mapBankStrList = Gtk::StringList::create( emptystr );

        _mapEditorBS1CB.set_model( _mapBankStrList );
        _mapEditorBS2CB.set_model( _mapBankStrList );
        bsselbox.append( _mapEditorBS1CB );
        bsselbox.append( _mapEditorBS2CB );
        bsselbox.get_style_context( )->add_class( "linked" );
        _mapEditorBlockSetBox.append( bsselbox );

        auto rScrolledWindow = Gtk::ScrolledWindow( );

        _currentMap    = Gio::ListStore<Gtk::Image>::create( );
        auto selection = Gtk::MultiSelection::create( );
        selection->set_model( _currentMap );
        _mapFactory = Gtk::SignalListItemFactory::create( );

        _mapFactory->signal_setup( ).connect( []( const std::shared_ptr<Gtk::ListItem>& p_item ) {
            fprintf( stderr, "signal_create\n" );
        } );
        _mapFactory->signal_bind( ).connect( []( const std::shared_ptr<Gtk::ListItem>& p_item ) {
            fprintf( stderr, "signal_bind\n" );
        } );

        _mapView = Gtk::GridView( selection, _mapFactory );
        _mapView.set_max_columns( MAP_SIZE );
        _mapView.set_min_columns( MAP_SIZE );
        _mapView.set_enable_rubberband( );
        rScrolledWindow.set_child( _mapView );
        _mapView.set_expand( );
        _mapEditorMapBox.append( rScrolledWindow );

        // テスト用

        //        loadNewFsRoot( "/home/philip/Repos/FSROOT" );
    }

    root::~root( ) {
    }

    void root::onFsRootOpenClick( ) {
        auto dialog = new Gtk::FileChooserDialog( "Select or create an FSROOT folder",
                                                  Gtk::FileChooser::Action::SELECT_FOLDER, true );
        dialog->set_transient_for( *this );
        dialog->set_modal( true );
        dialog->set_default_size( 800, 600 );
        dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
            this->onFolderDialogResponse( p_responseId, dialog );
        } );

        // Add response buttons to the dialog:
        dialog->add_button( "_Cancel", Gtk::ResponseType::CANCEL );
        dialog->add_button( "_Select", Gtk::ResponseType::OK );

        // Show the dialog and wait for a user response:
        dialog->show( );
    }

    void root::onFsRootSaveClick( ) {
        fprintf( stderr, "Here 2" );
    }

    void root::onFolderDialogResponse( int p_responseId, Gtk::FileChooserDialog* p_dialog ) {
        if( p_dialog == nullptr ) {
            fprintf( stderr, "[ERROR] root::onFolderDialogResponse: p_dialog is nullptr." );
            return;
        }

        // Handle the response:
        switch( p_responseId ) {
        case Gtk::ResponseType::OK: {
            loadNewFsRoot( p_dialog->get_file( )->get_path( ) );
            break;
        }
        default:
        case Gtk::ResponseType::CANCEL: break;
        }
        delete p_dialog;
    }

    bool root::checkOrCreatePath( const std::string& p_path ) {
        std::error_code ec;
        auto            p = fs::path( p_path, fs::path::generic_format );
        if( ec ) {
            fprintf( stderr, "[ERROR] Could not check if path exists: %s\n",
                     ec.message( ).c_str( ) );
            return false;
        }
        if( !fs::exists( p, ec ) ) {
            fs::create_directories( p, ec );
            fprintf( stderr, "[LOG] Creating directory %s.\n", p_path.c_str( ) );
            if( ec ) {
                fprintf( stderr, "[ERROR] Creating directory failed: %s\n",
                         ec.message( ).c_str( ) );
                return false;
            }
        }
        return true;
    }

    root::mapBankData root::exploreMapBank( const fs::path& p_path ) {
        std::error_code ec;
        u8              sx = 0, sy = 0;
        bool            scattered = false;
        for( auto& p : fs::directory_iterator( p_path ) ) {
            if( !p.is_directory( ec ) || ec ) { continue; }
            // check if the directory has a number as name
            std::string name         = p.path( ).filename( );
            int         nameAsNumber = -1;
            try {
                nameAsNumber = std::stoi( name );
            } catch( ... ) { continue; }

            if( nameAsNumber >= 0 && nameAsNumber <= int( MAX_MAPY ) ) {
                scattered = true;
                sy        = std::max( u8( nameAsNumber ), sy );
            }
        }
        if( scattered ) {
            // Check all subdirs [0, sy] for max horizontal dim
            for( u16 i = 0; i < sy; ++i ) {
                try {
                    auto tmpath = p_path / std::to_string( i );
                    for( auto& p : fs::directory_iterator( tmpath ) ) {
                        if( !p.is_regular_file( ec ) || ec ) { continue; }
                        // check if the file has a name that corresponds to a map
                        u8 mx, my;
                        if( sscanf( p.path( ).filename( ).c_str( ), MAPNAME_FORMAT.c_str( ), &my,
                                    &mx )
                            == 2 ) {
                            sx = std::max( sx, mx );
                            sy = std::max( sy, my );
                        }
                    }
                } catch( ... ) { continue; }
            }

        } else {
            for( auto& p : fs::directory_iterator( p_path ) ) {
                if( !p.is_regular_file( ec ) || ec ) { continue; }
                // check if the file has a name that corresponds to a map
                u8 mx, my;
                if( sscanf( p.path( ).filename( ).c_str( ), MAPNAME_FORMAT.c_str( ), &my, &mx )
                    == 2 ) {
                    sx = std::max( sx, mx );
                    sy = std::max( sy, my );
                }
            }
        }

        return { sx, sy, scattered };
    }

    void root::loadNewFsRoot( const std::string& p_path ) {
        if( !checkOrCreatePath( p_path ) ) { return; }
        if( !checkOrCreatePath( p_path + "/MAPS/" ) ) { return; }
        if( !checkOrCreatePath( p_path + "/MAPS/TILESETS/" ) ) { return; }
        if( !checkOrCreatePath( p_path + "/MAPS/BLOCKSETS/" ) ) { return; }
        if( !checkOrCreatePath( p_path + "/DATA/MAP_DATA/" ) ) { return; }
        FSROOT_PATH   = p_path;
        MAP_PATH      = FSROOT_PATH + "/MAPS/";
        TILESET_PATH  = FSROOT_PATH + "/MAPS/TILESETS/";
        BLOCKSET_PATH = FSROOT_PATH + "/MAPS/BLOCKSETS/";
        PALETTE_PATH  = FSROOT_PATH + "/MAPS/PALETTES/";
        MAPDATA_PATH  = FSROOT_PATH + "/DATA/MAP_DATA/";
        _fsRootLoaded = true;

        set_title( p_path + " - " + TITLE_STRING );
        if( _saveButton != nullptr ) { _saveButton->show( ); }

        _mainBox.show( );

        // update map banks
        _mapBanks.clear( );
        _blockSets.clear( );
        _blockSetNames.clear( );
        _selectedBank = -1;
        _mapNotebook.hide( );
        _loadMapLabel.show( );

        // traverse MAP_PATH and search for all dirs that are named with a number
        // (assuming they are a map bank)

        std::error_code ec;
        for( auto& p : fs::directory_iterator( MAP_PATH ) ) {
            if( p.is_directory( ec ) && !ec ) {
                std::string name         = p.path( ).filename( );
                int         nameAsNumber = -1;
                try {
                    nameAsNumber = std::stoi( name );
                } catch( ... ) { continue; }

                if( nameAsNumber < 0 || nameAsNumber > MAX_MAPBANK_NAME ) {
                    fprintf( stderr,
                             "[LOG] Skipping potential map bank %s: "
                             "name (as number) too large.\n",
                             name.c_str( ) );
                    continue;
                }

                auto info = exploreMapBank( p.path( ) );
                addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_scattered );
            }
        }

        // load all tilesets and blocksets
        for( auto& p : fs::directory_iterator( BLOCKSET_PATH ) ) {
            if( !p.is_regular_file( ec ) || ec ) { continue; }

            u8 bsname;
            if( 1 != sscanf( p.path( ).filename( ).c_str( ), BLOCKSET_FORMAT.c_str( ), &bsname ) ) {
                continue;
            }

            blockSetInfo res;
            FILE*        f = fopen( p.path( ).c_str( ), "r" );
            if( !DATA::readBlocks( f, res.m_blockSet.m_blocks ) ) {
                fprintf( stderr, "[LOG] Reading block set %hu failed.\n", bsname );
                continue;
            }
            fclose( f );

            // read corresponding tileset
            auto tspath = fs::path( TILESET_PATH ) / ( std::to_string( bsname ) + ".ts" );
            f           = fopen( tspath.c_str( ), "r" );
            if( !DATA::readTiles( f, res.m_tileSet.m_tiles ) ) {
                fprintf( stderr, "[LOG] Reading tile set %hu failed.\n", bsname );
                continue;
            }
            fclose( f );

            // read corresponding palettes
            auto palpath = fs::path( PALETTE_PATH ) / ( std::to_string( bsname ) + ".p2l" );
            f            = fopen( palpath.c_str( ), "r" );
            if( !DATA::readPal( f, res.m_pals, 8 * 5 ) ) {
                fprintf( stderr, "[LOG] Reading palette %hu failed.\n", bsname );
                continue;
            }
            fclose( f );

            // fprintf( stderr, "[LOG] Loaded blockset %hhu.\n", bsname );

            _blockSets[ bsname ] = res;
            _blockSetNames.insert( bsname );
        }

        auto bsnames = std::vector<Glib::ustring>( );

        for( auto bsname : _blockSetNames ) {
            bsnames.push_back( std::to_string( bsname ) );
            _blockSets[ bsname ].m_stringListItem = bsnames.size( ) - 1;
        }
        _mapBankStrList->splice( 0, _mapBankStrList->get_n_items( ), bsnames );
    }

    void root::addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, bool p_scattered,
                              mapBank::status p_status ) {
        if( !checkOrCreatePath( MAP_PATH + std::to_string( p_bank ) + "/" ) ) {
            fprintf( stderr, "[ERROR] Adding map bank %hu failed.\n", p_bank );
            return;
        }

        /*
        fprintf( stderr, "[LOG] Adding map bank %hu with initial size %hhu rows, %hhu cols.\n",
                 p_bank, p_sizeY + 1, p_sizeX + 1 );
                 */

        auto MB1 = std::make_shared<mapBank>( p_bank, p_sizeX, p_sizeY, p_status );
        MB1->connect( [ this ]( u16 p_bk, u8 p_y, u8 p_x ) { loadMap( p_bk, p_y, p_x ); } );
        _mapBanks[ p_bank ] = { MB1, false, p_scattered, nullptr, p_sizeX, p_sizeY };

        // keep the list sorted: insert after bank with largest id smaller than p_bank,
        // i.e., after the element before lower_bound( p_bank )
        auto ptr = _mapBanks.lower_bound( p_bank );
        if( ptr == _mapBanks.end( ) || ptr == _mapBanks.begin( ) ) {
            _mapBankBox.insert_child_after( *MB1, *_addMapBank );
        } else {
            --ptr;
            _mapBankBox.insert_child_after( *MB1, *ptr->second.m_widget );
        }
    }

    void root::createMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX ) {
        if( !_fsRootLoaded ) {
            fprintf( stderr, "[ERROR] No FSROOT loaded. Won't create a new map bank.\n" );
            return;
        }
        if( _mapBanks.count( p_bank ) ) {
            // map exists, do nothing
            fprintf( stderr, "[LOG] Map bank %hu already exists.\n", p_bank );
            return;
        }

        addNewMapBank( p_bank, p_sizeY, p_sizeX, true, mapBank::STATUS_NEW );
        loadMap( p_bank, 0, 0 );
    }

    void root::loadMapBank( u16 p_bank ) {
        if( p_bank == _selectedBank ) { return; }

        if( _selectedBank != -1 ) { _mapBanks[ _selectedBank ].m_widget->unselect( ); }
        _selectedBank = p_bank;

        auto& selb = _mapBanks[ _selectedBank ];

        selb.m_widget->select( );
        fprintf( stderr, "[LOG] Loading map bank %hu.\n", p_bank );

        // Load all maps of the bank into mem
        if( !selb.m_loaded ) {
            selb.m_bank = std::make_shared<DATA::mapBank>( );

            for( u16 y = 0; y <= selb.m_sizeY; ++y ) {
                auto row = std::vector<DATA::mapSlice>( );
                for( u16 x = 0; x <= selb.m_sizeX; ++x ) {
                    auto sl   = DATA::mapSlice( );
                    auto path = fs::path( MAP_PATH ) / std::to_string( p_bank );
                    if( selb.m_scattered ) { path /= std::to_string( y ); }
                    path /= ( std::to_string( y ) + "_" + std::to_string( x ) + ".map" );

                    FILE* f = fopen( path.c_str( ), "r" );

                    if( !DATA::readMapSlice( f, &sl, x, y ) ) {
                        fprintf( stderr, "[LOG] Loading map %hu/%hhu_%hhu.map failed. (path %s)\n",
                                 p_bank, y, x, path.c_str( ) );
                    }

                    row.push_back( sl );
                }
                selb.m_bank->m_mapData.push_back( row );
            }
            selb.m_loaded = true;
        }
    }

    void root::redrawMap( u8 p_mapY, u8 p_mapX ) {
        auto map = _mapBanks[ _selectedBank ].m_bank->m_mapData[ p_mapY ][ p_mapX ];
        u8   ts1 = map.m_tIdx1;
        u8   ts2 = map.m_tIdx2;

        fprintf( stderr, "TS %hhu %hhu %lu %lu\n", ts1, ts2, _blockSets.count( ts1 ),
                 _blockSets.count( ts2 ) );

        // update drop downs
        _mapEditorBS1CB.set_selected( _blockSets[ ts1 ].m_stringListItem );
        _mapEditorBS2CB.set_selected( _blockSets[ ts2 ].m_stringListItem );

        // compute tileset and blockset
        auto bs = DATA::blockSet<2>( );
        std::memcpy( bs.m_blocks, _blockSets[ ts1 ].m_blockSet.m_blocks,
                     sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
        std::memcpy( &bs.m_blocks[ DATA::MAX_BLOCKS_PER_TILE_SET ],
                     _blockSets[ ts2 ].m_blockSet.m_blocks,
                     sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );

        auto ts = DATA::tileSet<2>( );
        std::memcpy( ts.m_tiles, _blockSets[ ts1 ].m_tileSet.m_tiles,
                     sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
        std::memcpy( &ts.m_tiles[ DATA::MAX_TILES_PER_TILE_SET ],
                     _blockSets[ ts2 ].m_tileSet.m_tiles,
                     sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );

        auto computedMap = map.compute( &bs, &ts );

        // render each block
        auto renderedMap = std::vector<std::shared_ptr<Gtk::Image>>( );

        DATA::palette pals[ 16 * 5 ] = { 0 };
        for( u8 dt = 0; dt < 5; ++dt ) {
            std::memcpy( &pals[ 16 * dt ], &_blockSets[ ts1 ].m_pals[ 8 * dt ],
                         sizeof( DATA::palette ) * 8 );
            std::memcpy( &pals[ 16 * dt + 6 ], &_blockSets[ ts2 ].m_pals[ 8 * dt ],
                         sizeof( DATA::palette ) * 8 );
        }

        /*
        auto sb   = Gtk::ScrolledWindow( );
        auto grid = Gtk::Grid( );
        sb.set_child( grid );
        grid.set_column_homogeneous( );
        grid.set_row_homogeneous( );
        _mapEditorMapBox.append( sb );
        sb.set_vexpand( true );
        for( u8 y = 0; y < DATA::SIZE; ++y ) {
            for( u8 x = 0; x < DATA::SIZE; ++x ) {
                auto im = UI::block::createImage( computedMap[ DATA::SIZE * y + x ], pals,
                                                  _currentDayTime );
                im->set_size_request( DATA::BLOCK_SIZE * _blockScale,
                                      DATA::BLOCK_SIZE * _blockScale );
                grid.attach( *im, x, y );
            }
        }
        */
        _currentMap->splice( 0, _currentMap->get_n_items( ), renderedMap );
    }
    void root::loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( _selectedBank == -1 ) {
            _loadMapLabel.hide( );
            _mapNotebook.show( );
        }

        loadMapBank( p_bank );
        if( _mapBanks[ _selectedBank ].m_bank == nullptr || !_mapBanks[ _selectedBank ].m_loaded ) {
            return;
        }

        redrawMap( p_mapY, p_mapX );
        fprintf( stderr, "[LOG] Loading map %hu/%hhu_%hhu.\n", p_bank, p_mapY, p_mapX );
    }
} // namespace UI
