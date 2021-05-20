#include <cstring>
#include <filesystem>
#include <queue>

#include <gtkmm/actionbar.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/eventcontrollerkey.h>
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
#include "ui_root.h"

namespace UI {
    const std::string APP_NAME     = std::string( "Sachi" );
    const std::string TITLE_STRING = APP_NAME + " " + std::string( VERSION );

    void root::addFsRootToRecent( const std::string& p_path ) {
        auto res       = Gtk::RecentManager::Data( );
        res.app_name   = APP_NAME;
        res.is_private = true;
        if( !_recentlyUsedFsRoots->add_item( "file://" + p_path, res ) ) {
            printf( "Adding %s failed.\n", p_path.c_str( ) );
        }
    }

    void root::removeFsRootFromRecent( const std::string& p_path ) {
        try {
            _recentlyUsedFsRoots->remove_item( p_path );
        } catch( ... ) {}
    }

    auto root::getRecentFsRoots( ) {
        auto unfiltered = _recentlyUsedFsRoots->get_items( );

        auto res = std::vector<std::shared_ptr<Gtk::RecentInfo>>( );

        for( auto in : unfiltered ) {

            if( in->has_application( APP_NAME ) ) {
                if( in->exists( ) ) {
                    res.push_back( in );
                } else {
                    removeFsRootFromRecent( in->get_uri( ) );
                }
            }
        }

        return res;
    }

    void root::populateRecentFsRootIconView( ) {
        for( auto rf : getRecentFsRoots( ) ) {
            auto row                           = *( _recentFsRootListModel->append( ) );
            row[ _recentViewColumns.m_path ]   = rf->get_uri_display( );
            row[ _recentViewColumns.m_pixbuf ] = Gdk::Pixbuf::create_from_file( "./icon.bmp" );
            //            row[ _recentViewColumns.m_modified ] = rf->get_modified( );
        }
    }

    auto root::createButton( const std::string& p_iconName, const std::string& p_labelText,
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

        _recentlyUsedFsRoots = Gtk::RecentManager::get_default( );

        // Header bar

        auto headerBar = Gtk::HeaderBar( );
        set_titlebar( headerBar );

        _openButton = createButton( "", "_Load FSROOT…", [ & ]( ) { this->onFsRootOpenClick( ); } );
        _saveButton = createButton( "", "_Save Changes", [ & ]( ) { this->onFsRootSaveClick( ); } );
        _collapseMapBanksButton
            = createButton( "view-restore-symbolic", "_Collapse Map Banks",
                            [ & ]( ) { this->collapseMapBankBar( !_mapBankBarCollapsed ); } );
        _collapseMapBanksButton->set_expand( true );
        _collapseMapBanksButton->set_margin( MARGIN );
        _collapseMapBanksButton->set_has_frame( false );

        headerBar.pack_start( *_openButton );
        headerBar.pack_start( *_saveButton );

        _saveButton->hide( );
        auto mbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _mainBox  = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        set_child( mbox );
        mbox.append( _mainBox );

        // Main window on application start:
        // Show icon view of recently used FSROOT folders

        _ivScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        mbox.append( _ivScrolledWindow );
        _ivScrolledWindow.set_expand( );
        _ivScrolledWindow.set_child( _recentFsRootIconView );

        _recentFsRootListModel = Gtk::ListStore::create( _recentViewColumns );
        //        _recentFsRootListModel->set_sort_column( _recentViewColumns.m_modified,
        //                                                 Gtk::SortType::DESCENDING );

        _recentFsRootIconView.set_model( _recentFsRootListModel );
        _recentFsRootIconView.set_pixbuf_column( _recentViewColumns.m_pixbuf );
        _recentFsRootIconView.set_text_column( _recentViewColumns.m_path );
        _recentFsRootIconView.set_activate_on_single_click( );
        _recentFsRootIconView.signal_item_activated( ).connect(
            [ this ]( const Gtk::TreeModel::Path& p_path ) {
                auto iter = _recentFsRootListModel->get_iter( p_path );
                auto row  = *iter;
                loadNewFsRoot( row[ _recentViewColumns.m_path ] );
            } );

        // Main window

        // map bank box

        auto lFrame = Gtk::Frame( );
        lFrame.set_margin( MARGIN );
        _mainBox.append( lFrame );

        auto lScrolledWindow = Gtk::ScrolledWindow( );
        lFrame.set_child( lScrolledWindow );
        lFrame.set_label_widget( *_collapseMapBanksButton );
        lFrame.set_label_align( Gtk::Align::CENTER );
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

        // Map editor

        _loadMapLabel = Gtk::Label( );
        _loadMapLabel.set_markup( "<span size=\"x-large\">Add or load a map bank!</span>" );
        _loadMapLabel.set_expand( );
        _mainBox.append( _loadMapLabel );

        _mapNotebook.set_margin( MARGIN );
        _mapNotebook.set_expand( );
        _mapNotebook.hide( );
        auto mapEditorMainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        mapEditorMainBox.set_margin( MARGIN );

        _mapNotebook.append_page( mapEditorMainBox, "Map _Editor", true );
        _mapNotebook.append_page( _mapOverviewBox, "Bank _Overview", true );
        _mainBox.append( _mapNotebook );
        _mainBox.hide( );

        mapEditorMainBox.append( _mapEditorMapBox );
        _mapEditorMapBox.set_expand( );

        // Blocksets
        // +-----------+
        // | Blocksets |
        // | [ 0 | 7 ] |
        // |+---------+|
        // ||         ||
        // ||         ||
        // ||         ||
        // |+---------+|
        // ||         ||
        // ||         ||
        // ||         ||
        // |+---------+|
        // +-----------+

        auto bsselbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        bsselbox.set_margin( MARGIN );
        bsselbox.set_halign( Gtk::Align::CENTER );

        auto bsself = Gtk::Frame( "Blocksets" );
        bsself.set_margin( MARGIN );
        bsself.set_label_align( Gtk::Align::CENTER );
        mapEditorMainBox.append( bsself );
        bsself.set_child( _mapEditorBlockSetBox );

        auto emptystr   = std::vector<Glib::ustring>( );
        _mapBankStrList = Gtk::StringList::create( emptystr );

        _mapEditorBS1CB.set_model( _mapBankStrList );
        _mapEditorBS1CB.property_selected_item( ).signal_changed( ).connect( [ this ]( ) {
            if( _disableRedraw || _mapEditorBS1CB.get_selected( ) == GTK_INVALID_LIST_POSITION
                || !_fsRootLoaded ) {
                return;
            }
            try {
                u16 newTS1
                    = std::stoi( _mapBankStrList->get_string( _mapEditorBS1CB.get_selected( ) ) );
                currentMapUpdateTS1( newTS1 );
            } catch( ... ) { return; }
        } );
        _mapEditorBS2CB.set_model( _mapBankStrList );
        _mapEditorBS2CB.property_selected_item( ).signal_changed( ).connect( [ this ]( ) {
            if( _disableRedraw || _mapEditorBS2CB.get_selected( ) == GTK_INVALID_LIST_POSITION
                || !_fsRootLoaded ) {
                return;
            }
            try {
                u16 newTS2
                    = std::stoi( _mapBankStrList->get_string( _mapEditorBS2CB.get_selected( ) ) );
                currentMapUpdateTS2( newTS2 );
            } catch( ... ) { return; }
        } );

        bsselbox.append( _mapEditorBS1CB );
        bsselbox.append( _mapEditorBS2CB );
        bsselbox.get_style_context( )->add_class( "linked" );
        _mapEditorBlockSetBox.append( bsselbox );

        auto meScrolledWindow1 = Gtk::ScrolledWindow( );
        meScrolledWindow1.set_child( _ts1widget );
        meScrolledWindow1.set_margin( MARGIN );
        meScrolledWindow1.set_vexpand( );
        meScrolledWindow1.set_halign( Gtk::Align::CENTER );
        meScrolledWindow1.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mapEditorBlockSetBox.append( meScrolledWindow1 );

        auto meScrolledWindow2 = Gtk::ScrolledWindow( );
        meScrolledWindow2.set_child( _ts2widget );
        meScrolledWindow2.set_margin( MARGIN );
        meScrolledWindow2.set_vexpand( );
        meScrolledWindow2.set_halign( Gtk::Align::CENTER );
        meScrolledWindow2.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mapEditorBlockSetBox.append( meScrolledWindow2 );

        // Map window
        // +--------------+
        // |              |
        // |     Map      |
        // |              |
        // | ------------ |
        // |  Action Bar  |
        // +--------------+

        auto rScrolledWindow = Gtk::ScrolledWindow( );
        rScrolledWindow.set_child( _mapGrid );
        _mapGrid.set_halign( Gtk::Align::CENTER );
        _mapGrid.set_valign( Gtk::Align::CENTER );

        for( u8 x = 0; x < 3; ++x ) {
            _currentMap.push_back( std::vector<mapSlice>( 3 ) );
            for( u8 y = 0; y < 3; ++y ) {
                _mapGrid.attach( _currentMap[ x ][ y ], x, y );
                _currentMap[ x ][ y ].connectClick(
                    [ this, x, y ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapClicked( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                      x == 1 && y == 1 );
                    } );
                _currentMap[ x ][ y ].connectDrag(
                    [ this, x, y ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapDragStart( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                        x == 1 && y == 1 );
                    },
                    [ this, x, y ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapDragUpdate( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                         x == 1 && y == 1 );
                    },
                    [ this, x, y ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapDragEnd( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                      x == 1 && y == 1 );
                    } );
            }
        }

        _mapGrid.set_row_spacing( _neighborSpacing );
        _mapGrid.set_column_spacing( _neighborSpacing );

        rScrolledWindow.set_expand( );
        _mapEditorMapBox.append( rScrolledWindow );

        // map editor action box
        // +--------------------+-----+----------------------------+
        // | Map Grid [ 0 |+|-] | Map | Blockset width   [ 8 |+|-] |
        // | Scale    [ 1 |+|-] | Nav | Neigh map blocks [ 8 |+|-] |
        // | Day Time [Day (2)] |     | Neigh distance   [ 9 |+|-] |
        // +--------------------+-----+----------------------------+
        auto abScrolledWindow = Gtk::ScrolledWindow( );
        _mapEditorMapBox.append( abScrolledWindow );
        abScrolledWindow.set_child( _mapEditorActionBar );
        abScrolledWindow.set_policy( Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER );

        // action bar start
        auto abStartBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abStartBox.set_valign( Gtk::Align::CENTER );
        _mapEditorActionBar.pack_start( abStartBox );
        auto abSb1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abSb2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abSb3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abStartBox.append( abSb1 );
        abStartBox.append( abSb2 );
        abStartBox.append( abSb3 );

        auto abSAdj1 = Gtk::Adjustment::create( _blockSpacing, 0.0, 9.0, 1.0, 1.0, 0.0 );
        auto abSAdj2 = Gtk::Adjustment::create( _blockScale, 1.0, 8.0, 1.0, 1.0, 0.0 );
        auto abSAdj3 = Gtk::Adjustment::create( _currentDayTime, 0.0, 4.0, 1.0, 1.0, 0.0 );

        _mapEditorSettings1 = Gtk::SpinButton( abSAdj1 );
        _mapEditorSettings1.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapEditorSettings1.get_value_as_int( );
            // Set spacing of everything map-py

            for( u8 x = 0; x < 3; ++x ) {
                for( u8 y = 0; y < 3; ++y ) {
                    _currentMap[ x ][ y ].setSpacing( value );
                    _currentMap[ x ][ y ].queue_resize( );
                }
            }
            _ts1widget.setSpacing( value );
            _ts1widget.queue_resize( );
            _ts2widget.setSpacing( value );
            _ts2widget.queue_resize( );
            _blockSpacing = value;
        } );
        _mapEditorSettings1.set_margin_start( MARGIN );
        _mapEditorSettings1.set_width_chars( 1 );
        _mapEditorSettings1.set_max_width_chars( 1 );

        _mapEditorSettings2 = Gtk::SpinButton( abSAdj2 );
        _mapEditorSettings2.set_margin_start( MARGIN );
        _mapEditorSettings2.set_width_chars( 1 );
        _mapEditorSettings2.set_max_width_chars( 1 );
        _mapEditorSettings2.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapEditorSettings2.get_value_as_int( );
            // Set scale of everything map-py

            for( u8 x = 0; x < 3; ++x ) {
                for( u8 y = 0; y < 3; ++y ) {
                    _currentMap[ x ][ y ].setScale( value );
                    _currentMap[ x ][ y ].queue_resize( );
                }
            }
            _ts1widget.setScale( value );
            _ts1widget.queue_resize( );
            _ts2widget.setScale( value );
            _ts2widget.queue_resize( );
            _blockScale = value;
        } );

        _mapEditorSettings3 = Gtk::SpinButton( abSAdj3 );
        _mapEditorSettings3.set_margin_start( MARGIN );
        _mapEditorSettings3.set_wrap( );
        _mapEditorSettings3.set_width_chars( 1 );
        _mapEditorSettings3.set_max_width_chars( 1 );
        _mapEditorSettings3.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapEditorSettings3.get_value_as_int( );
            // Change daytime of everything map-py

            for( u8 x = 0; x < 3; ++x ) {
                for( u8 y = 0; y < 3; ++y ) { _currentMap[ x ][ y ].redraw( value ); }
            }
            _ts1widget.redraw( value );
            _ts2widget.redraw( value );

            _currentDayTime = value;
        } );

        auto abSl1 = Gtk::Image( );
        abSl1.set_from_icon_name( "view-grid-symbolic" );
        auto abSl2 = Gtk::Image( );
        abSl2.set_from_icon_name( "edit-find-symbolic" );
        auto abSl3 = Gtk::Image( );
        abSl3.set_from_icon_name( "weather-clear-symbolic" );
        abSb1.append( abSl1 );
        abSb1.append( _mapEditorSettings1 );
        abSb2.append( abSl2 );
        abSb2.append( _mapEditorSettings2 );
        abSb3.append( abSl3 );
        abSb3.append( _mapEditorSettings3 );

        // action bar center
        auto mapNavGrid = Gtk::Grid( );
        for( u8 x = 0; x < 3; ++x ) {
            for( u8 y = 0; y < 3; ++y ) {
                if( x == 1 && y == 1 ) {
                    auto im = Gtk::Image( );
                    im.set_from_icon_name( "image-x-generic-symbolic" );
                    mapNavGrid.attach( im, x, y );
                    continue;
                }
                _mapNavButton[ x ][ y ] = createButton(
                    "", "", [ this, y, x ]( ) { moveToMap( s8( y ) - 1, s8( x ) - 1 ); } );
                _mapNavButton[ x ][ y ]->set_has_frame( false );
                if( x == 1 && y == 2 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-down-symbolic" );
                } else if( x == 1 && y == 0 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-up-symbolic" );
                } else if( x == 0 && y == 1 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-start-symbolic" );
                } else if( x == 2 && y == 1 ) {
                    _mapNavButton[ x ][ y ]->set_icon_name( "pan-end-symbolic" );
                }
                mapNavGrid.attach( *_mapNavButton[ x ][ y ], x, y );
            }
        }
        _mapEditorActionBar.set_center_widget( mapNavGrid );

        // action bar end

        auto abEndBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abEndBox.set_valign( Gtk::Align::CENTER );
        _mapEditorActionBar.pack_end( abEndBox );
        auto abEb1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abEb2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abEb3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abEndBox.append( abEb1 );
        abEndBox.append( abEb2 );
        abEndBox.append( abEb3 );

        auto abEAdj1 = Gtk::Adjustment::create( _blockSetWidth, 1.0, 32.0, 1.0, 1.0, 0.0 );
        auto abEAdj2 = Gtk::Adjustment::create( _adjacentBlocks, 0.0, 32.0, 1.0, 1.0, 0.0 );
        auto abEAdj3 = Gtk::Adjustment::create( _neighborSpacing, 0.0, 50.0, 1.0, 1.0, 0.0 );

        _mapEditorSettings4 = Gtk::SpinButton( abEAdj1 );
        _mapEditorSettings4.set_margin_start( MARGIN );
        _mapEditorSettings4.set_width_chars( 1 );
        _mapEditorSettings4.set_max_width_chars( 1 );
        _mapEditorSettings4.signal_value_changed( ).connect( [ & ]( ) {
            _blockSetWidth = _mapEditorSettings4.get_value_as_int( );
            auto mp
                = _mapBanks[ _selectedBank ].m_bank->m_mapData[ _selectedMapY ][ _selectedMapX ];
            auto ts = DATA::tileSet<2>( );
            buildTileSet( &ts );
            DATA::palette pals[ 16 * 5 ] = { 0 };
            buildPalette( pals );

            _ts1widget.set(
                DATA::mapBlockAtom::computeBlockSet( &_blockSets[ mp.m_tIdx1 ].m_blockSet, &ts ),
                pals, _blockSetWidth );
            _ts1widget.redraw( _currentDayTime );

            _ts2widget.set(
                DATA::mapBlockAtom::computeBlockSet( &_blockSets[ mp.m_tIdx2 ].m_blockSet, &ts ),
                pals, _blockSetWidth );
            _ts2widget.redraw( _currentDayTime );
        } );

        _mapEditorSettings5 = Gtk::SpinButton( abEAdj2 );
        _mapEditorSettings5.set_margin_start( MARGIN );
        _mapEditorSettings5.set_width_chars( 1 );
        _mapEditorSettings5.set_max_width_chars( 1 );
        _mapEditorSettings5.signal_value_changed( ).connect( [ & ]( ) {
            _adjacentBlocks = _mapEditorSettings5.get_value_as_int( );
            redrawMap( _selectedMapY, _selectedMapX );
        } );

        _mapEditorSettings6 = Gtk::SpinButton( abEAdj3 );
        _mapEditorSettings6.set_margin_start( MARGIN );
        _mapEditorSettings6.set_width_chars( 1 );
        _mapEditorSettings6.set_max_width_chars( 1 );

        _mapEditorSettings6.signal_value_changed( ).connect( [ & ]( ) {
            _neighborSpacing = _mapEditorSettings6.get_value_as_int( );
            _mapGrid.set_row_spacing( _neighborSpacing );
            _mapGrid.set_column_spacing( _neighborSpacing );
        } );

        auto abEl1 = Gtk::Image( );
        abEl1.set_from_icon_name( "window-restore-symbolic" );
        auto abEl2 = Gtk::Image( );
        abEl2.set_from_icon_name( "process-stop-symbolic" );
        auto abEl3 = Gtk::Image( );
        abEl3.set_from_icon_name( "application-x-appliance-symbolic" );
        abEb1.append( abEl1 );
        abEb1.append( _mapEditorSettings4 );
        abEb2.append( abEl2 );
        abEb2.append( _mapEditorSettings5 );
        abEb3.append( abEl3 );
        abEb3.append( _mapEditorSettings6 );

        // Events / Button presses

        auto controller = Gtk::EventControllerKey::create( );
        controller->signal_key_pressed( ).connect(
            [ this ]( unsigned p_keyval, unsigned, Gdk::ModifierType p_state ) -> bool {
                // On Ctrl+0: Hide/Show map bank bar and navigation bar
                if( p_keyval == GDK_KEY_0
                    && ( p_state & ( Gdk::ModifierType::CONTROL_MASK ) )
                           == Gdk::ModifierType::CONTROL_MASK ) {
                    _focusMode = !_focusMode;
                    collapseMapBankBar( _focusMode );
                    _mapEditorActionBar.set_revealed( !_focusMode );
                    return true;
                }

                return false;
            },
            false );

        add_controller( controller );
        populateRecentFsRootIconView( );
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
        // Only write map banks that have been changed

        for( auto [ bank, info ] : _mapBanks ) {
            if( info.m_widget != nullptr && info.m_bank != nullptr
                && ( info.m_widget->getStatus( ) == mapBank::STATUS_NEW
                     || info.m_widget->getStatus( ) == mapBank::STATUS_EDITED_UNSAVED ) ) {
                // something changed here, save the maps

                fprintf( stderr, "[LOG] Saving map bank %hu.\n", bank );
                bool error = false;

                for( u8 y = 0; y < info.m_sizeY; ++y ) {
                    for( u8 x = 0; x < info.m_sizeX; ++x ) {
                        auto path = fs::path( MAP_PATH ) / std::to_string( bank );
                        if( info.m_scattered ) { path /= std::to_string( y ); }
                        path /= std::to_string( y ) + "_" + std::to_string( x ) + ".map";

                        FILE* f = fopen( path.c_str( ), "w" );
                        if( !DATA::writeMapSlice( f, &info.m_bank->m_mapData[ y ][ x ] ) ) {
                            fprintf( stderr,
                                     "[ERROR] Writing map %hu/%hhu_%hhu.map to %s failed.\n", bank,
                                     y, x, path.c_str( ) );
                            error = true;
                        }
                    }
                }
                if( !error ) { info.m_widget->setStatus( mapBank::STATUS_SAVED ); }
            }
        }
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

    void root::updateSelectedBlock( DATA::mapBlockAtom p_block ) {
        _currentlySelectedBlock = p_block;
        if( _currentlySelectedBlock.m_blockidx < DATA::MAX_BLOCKS_PER_TILE_SET ) {
            _currentlySelectedComputedBlock
                = _currentBlockset1[ _currentlySelectedBlock.m_blockidx ];
        } else {
            _currentlySelectedComputedBlock = _currentBlockset2[ _currentlySelectedBlock.m_blockidx
                                                                 - DATA::MAX_BLOCKS_PER_TILE_SET ];
        }
        // TODO:        markBlockInTS( _currentlySelectedBlock.m_blockidx );
    }

    void root::onMapDragStart( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                               s8 p_mapX, s8 p_mapY, bool p_allowEdit ) {
        _dragStart = { p_blockX, p_blockY, p_mapX, p_mapY };
    }

    void root::onMapDragUpdate( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                                s8 p_mapY, bool p_allowEdit ) {

        auto [ sx, sy, _1, _2 ] = _dragStart;
        (void) _1;
        (void) _2;

        auto blockwd = _blockScale * DATA::BLOCK_SIZE + _blockSpacing;
        auto nx      = sx + ( p_dX / blockwd );
        auto ny      = sy + ( p_dY / blockwd );

        fprintf( stderr, "DragUpdate butto %hu bx: %hu by: %hu, %hhi %hhi %hhu\n", p_button, nx, ny,
                 p_mapX, p_mapY, p_allowEdit );

        if( p_allowEdit && p_button == mapSlice::clickType::LEFT
            && isInMapBounds( nx, ny, p_mapX, p_mapY ) ) {
            onMapClicked( p_button, nx, ny, p_mapX, p_mapY, p_allowEdit );
        }
    }

    void root::onMapDragEnd( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                             s8 p_mapY, bool p_allowEdit ) {
        auto [ sx, sy, _1, _2 ] = _dragStart;
        (void) _1;
        (void) _2;

        auto blockwd = _blockScale * DATA::BLOCK_SIZE + _blockSpacing;
        auto nx      = sx + ( p_dX / blockwd );
        auto ny      = sy + ( p_dY / blockwd );

        fprintf( stderr, "DragEnd butto %hu bx: %hu by: %hu, %hhi %hhi %hhu\n", p_button, nx, ny,
                 p_mapX, p_mapY, p_allowEdit );
    }

    void root::onMapClicked( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                             s8 p_mapX, s8 p_mapY, bool p_allowEdit ) {

        // fprintf( stderr, "onMapClicked butto %hu bx: %hu by: %hu, %hhi %hhi %hhu\n", p_button,
        //         p_blockX, p_blockY, p_mapX, p_mapY, p_allowEdit );

        // compute block
        u16 xcorr = 0, ycorr = 0;
        if( p_mapX < 0 ) { xcorr = DATA::SIZE - _adjacentBlocks; }
        if( p_mapY < 0 ) { ycorr = DATA::SIZE - _adjacentBlocks; }

        if( _selectedMapY + p_mapY < 0 || _selectedMapX + p_mapX < 0
            || _selectedMapX + p_mapX > _mapBanks[ _selectedBank ].m_sizeX
            || _selectedMapY + p_mapY > _mapBanks[ _selectedBank ].m_sizeY ) {
            if( p_button == mapSlice::clickType::RIGHT ) { updateSelectedBlock( { 0, 1 } ); }
            return;
        }

        auto& mp = _mapBanks[ _selectedBank ]
                       .m_bank->m_mapData[ _selectedMapY + p_mapY ][ _selectedMapX + p_mapX ];
        auto& block = mp.m_blocks[ p_blockY + ycorr ][ p_blockX + xcorr ];

        switch( p_button ) {
        case mapSlice::clickType::LEFT:
            if( p_allowEdit ) {
                block = _currentlySelectedBlock;
                _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock(
                    _currentlySelectedComputedBlock, p_blockX, p_blockY );
                markBankChanged( _selectedBank );
            } else {
                updateSelectedBlock( block );
            }
            break;
        case mapSlice::clickType::RIGHT: updateSelectedBlock( block ); break;
        case mapSlice::clickType::MIDDLE:
            if( p_allowEdit ) {
                DATA::mapBlockAtom oldb = block;
                // flood fill
                auto bqueue = std::queue<std::pair<s16, s16>>( );
                bqueue.push( { p_blockY, p_blockX } );
                while( !bqueue.empty( ) ) {
                    auto [ cy, cx ] = bqueue.front( );
                    bqueue.pop( );

                    if( cx < 0 || cx >= DATA::SIZE || cy < 0 || cy >= DATA::SIZE
                        || mp.m_blocks[ cy ][ cx ].m_blockidx != oldb.m_blockidx ) {
                        continue;
                    }

                    mp.m_blocks[ cy ][ cx ] = _currentlySelectedBlock;
                    for( s8 i = -1; i <= 1; ++i ) {
                        for( s8 j = -1; j <= 1; ++j ) { bqueue.push( { cy + i, cx + j } ); }
                    }
                }
                markBankChanged( _selectedBank );
                redrawMap( _selectedMapY + p_mapY, _selectedMapX + p_mapX );
            } else {
                updateSelectedBlock( block );
            }
            break;
        default: break;
        }
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

        _ivScrolledWindow.hide( );
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

        _disableRedraw = true;
        auto bsnames   = std::vector<Glib::ustring>( );

        for( auto bsname : _blockSetNames ) {
            bsnames.push_back( std::to_string( bsname ) );
            _blockSets[ bsname ].m_stringListItem = bsnames.size( ) - 1;
        }
        _mapBankStrList->splice( 0, _mapBankStrList->get_n_items( ), bsnames );
        _disableRedraw = false;

        addFsRootToRecent( p_path );
    }

    void root::addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, bool p_scattered,
                              mapBank::status p_status ) {
        if( !checkOrCreatePath( fs::path( MAP_PATH ) / std::to_string( p_bank ) ) ) {
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

    void root::collapseMapBankBar( bool p_collapse ) {
        for( auto i : _mapBanks ) {
            if( i.second.m_widget ) { i.second.m_widget->collapse( p_collapse ); }
        }
        _addMapBank->collapse( p_collapse );
        if( p_collapse ) {
            auto icon = Gtk::Image( );
            icon.set_from_icon_name( "view-fullscreen-symbolic" );
            _collapseMapBanksButton->set_child( icon );
        } else {
            auto icon = Gtk::Image( );
            icon.set_from_icon_name( "view-restore-symbolic" );
            auto label = Gtk::Label( "_Collapse Map Banks" );
            label.set_expand( true );
            label.set_use_underline( );

            auto hbox = Gtk::Box( Gtk::Orientation::HORIZONTAL, 5 );
            hbox.append( icon );
            hbox.append( label );
            _collapseMapBanksButton->set_child( hbox );
        }
        _mapBankBarCollapsed = p_collapse;
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

    void root::markBankChanged( u16 p_bank, mapBank::status p_newStatus ) {
        if( !_mapBanks.count( p_bank ) ) { return; }
        if( _mapBanks[ p_bank ].m_widget ) {
            _mapBanks[ p_bank ].m_widget->setStatus( p_newStatus );
        }
    }

    void root::currentMapUpdateTS1( u8 p_newTS ) {
        if( _selectedBank == -1 || _selectedMapY == -1 || _selectedMapX == -1 ) { return; }
        auto& mp = _mapBanks[ _selectedBank ].m_bank->m_mapData[ _selectedMapY ][ _selectedMapX ];

        if( p_newTS != mp.m_tIdx1 ) {
            mp.m_tIdx1 = p_newTS;
            markBankChanged( _selectedBank );
            redrawMap( _selectedMapY, _selectedMapX );
        }

        // although it shouldn't, bs1 can use tiles or palettes from ts2
        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        buildPalette( pals );

        _currentBlockset1
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ mp.m_tIdx1 ].m_blockSet, &ts );
        _currentBlockset2
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ mp.m_tIdx2 ].m_blockSet, &ts );

        _ts1widget.set( _currentBlockset1, pals, _blockSetWidth );
        _ts2widget.set( _currentBlockset2, pals, _blockSetWidth );
        _ts1widget.redraw( _currentDayTime );
        _ts2widget.redraw( _currentDayTime );
    }

    void root::currentMapUpdateTS2( u8 p_newTS ) {
        if( _selectedBank == -1 || _selectedMapY == -1 || _selectedMapX == -1 ) { return; }
        auto& mp = _mapBanks[ _selectedBank ].m_bank->m_mapData[ _selectedMapY ][ _selectedMapX ];

        if( p_newTS != mp.m_tIdx2 ) {
            mp.m_tIdx2 = p_newTS;
            markBankChanged( _selectedBank );
            redrawMap( _selectedMapY, _selectedMapX );
        }

        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        buildPalette( pals );

        _currentBlockset1
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ mp.m_tIdx1 ].m_blockSet, &ts );
        _currentBlockset2
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ mp.m_tIdx2 ].m_blockSet, &ts );

        _ts1widget.set( _currentBlockset1, pals, _blockSetWidth );
        _ts2widget.set( _currentBlockset2, pals, _blockSetWidth );
        _ts1widget.redraw( _currentDayTime );
        _ts2widget.redraw( _currentDayTime );
    }

    void root::buildBlockSet( DATA::blockSet<2>* p_out ) {
        auto mp  = _mapBanks[ _selectedBank ].m_bank->m_mapData[ _selectedMapY ][ _selectedMapX ];
        u8   ts1 = mp.m_tIdx1;
        u8   ts2 = mp.m_tIdx2;
        std::memcpy( p_out->m_blocks, _blockSets[ ts1 ].m_blockSet.m_blocks,
                     sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
        std::memcpy( &p_out->m_blocks[ DATA::MAX_BLOCKS_PER_TILE_SET ],
                     _blockSets[ ts2 ].m_blockSet.m_blocks,
                     sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
    }

    void root::buildTileSet( DATA::tileSet<2>* p_out ) {
        auto mp  = _mapBanks[ _selectedBank ].m_bank->m_mapData[ _selectedMapY ][ _selectedMapX ];
        u8   ts1 = mp.m_tIdx1;
        u8   ts2 = mp.m_tIdx2;
        std::memcpy( p_out->m_tiles, _blockSets[ ts1 ].m_tileSet.m_tiles,
                     sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
        std::memcpy( &p_out->m_tiles[ DATA::MAX_TILES_PER_TILE_SET ],
                     _blockSets[ ts2 ].m_tileSet.m_tiles,
                     sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
    }

    void root::buildPalette( DATA::palette p_out[ 5 * 16 ] ) {
        auto mp  = _mapBanks[ _selectedBank ].m_bank->m_mapData[ _selectedMapY ][ _selectedMapX ];
        u8   ts1 = mp.m_tIdx1;
        u8   ts2 = mp.m_tIdx2;
        for( u8 dt = 0; dt < 5; ++dt ) {
            std::memcpy( &p_out[ 16 * dt ], &_blockSets[ ts1 ].m_pals[ 8 * dt ],
                         sizeof( DATA::palette ) * 8 );
            std::memcpy( &p_out[ 16 * dt + 6 ], &_blockSets[ ts2 ].m_pals[ 8 * dt ],
                         sizeof( DATA::palette ) * 8 );
        }
    }

    void root::redrawMap( u8 p_mapY, u8 p_mapX ) {
        auto mp       = _mapBanks[ _selectedBank ].m_bank->m_mapData[ p_mapY ][ p_mapX ];
        _selectedMapX = p_mapX;
        _selectedMapY = p_mapY;

        // compute tileset and blockset
        auto bs = DATA::blockSet<2>( );
        buildBlockSet( &bs );
        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts );
        DATA::palette pals[ 16 * 5 ];
        buildPalette( pals );

        for( s8 x = -1; x <= 1; ++x ) {
            for( s8 y = -1; y <= 1; ++y ) {
                if( p_mapY + y >= 0 && p_mapY + y <= _mapBanks[ _selectedBank ].m_sizeY
                    && p_mapX + x >= 0 && p_mapX + x <= _mapBanks[ _selectedBank ].m_sizeX ) {
                    mp = _mapBanks[ _selectedBank ].m_bank->m_mapData[ p_mapY + y ][ p_mapX + x ];
                } else {
                    mp = DATA::mapSlice( );
                }

                u8 mapwd = DATA::SIZE, maphg = DATA::SIZE;
                if( x ) { mapwd = _adjacentBlocks; }
                if( y ) { maphg = _adjacentBlocks; }

                if( !mapwd || !maphg ) {
                    _currentMap[ x + 1 ][ y + 1 ].hide( );
                    continue;
                } else {
                    _currentMap[ x + 1 ][ y + 1 ].show( );
                }

                auto computed = mp.compute( &bs, &ts );
                auto filtered = std::vector<DATA::computedBlock>( );
                for( u8 y2 = 0; y2 < DATA::SIZE; ++y2 ) {
                    for( u8 x2 = 0; x2 < DATA::SIZE; ++x2 ) {
                        bool addx = false, addy = false;
                        if( x2 < mapwd && x >= 0 ) {
                            addx = true;
                        } else if( x2 >= DATA::SIZE - mapwd && x <= 0 ) {
                            addx = true;
                        } else if( x == 0 ) {
                            addx = true;
                        }
                        if( y2 < maphg && y >= 0 ) {
                            addy = true;
                        } else if( y2 >= DATA::SIZE - maphg && y <= 0 ) {
                            addy = true;
                        } else if( y == 0 ) {
                            addy = true;
                        }

                        if( addx && addy ) {
                            filtered.push_back( computed[ DATA::SIZE * y2 + x2 ] );
                        }
                    }
                }

                _currentMap[ x + 1 ][ y + 1 ].set( filtered, pals, mapwd );
                _currentMap[ x + 1 ][ y + 1 ].redraw( _currentDayTime );
            }
        }
    }

    void root::moveToMap( s8 p_dy, s8 p_dx ) {
        if( !_selectedMapX && p_dx < 0 ) { return; }
        if( !_selectedMapY && p_dy < 0 ) { return; }

        if( int( _selectedMapY + p_dy ) > int( MAX_MAPY ) ) { return; }
        if( int( _selectedMapX + p_dx ) > int( MAX_MAPY ) ) { return; }

        _selectedMapX += p_dx;
        _selectedMapY += p_dy;

        if( _selectedMapY > _mapBanks[ _selectedBank ].m_sizeY ) {
            _mapBanks[ _selectedBank ].m_sizeY = _selectedMapY;
            _mapBanks[ _selectedBank ].m_widget->setSizeY( _selectedMapY );
            _mapBanks[ _selectedBank ].m_bank->m_mapData.push_back(
                std::vector<DATA::mapSlice>( _mapBanks[ _selectedBank ].m_sizeX ) );
            markBankChanged( _selectedBank );
        }
        if( _selectedMapX > _mapBanks[ _selectedBank ].m_sizeX ) {
            _mapBanks[ _selectedBank ].m_sizeX = _selectedMapX;
            _mapBanks[ _selectedBank ].m_widget->setSizeX( _selectedMapX );
            for( u8 y = 0; y < _mapBanks[ _selectedBank ].m_sizeY; ++y ) {
                _mapBanks[ _selectedBank ].m_bank->m_mapData[ y ].push_back( DATA::mapSlice( ) );
            }
            markBankChanged( _selectedBank );
        }

        loadMap( _selectedBank, _selectedMapY, _selectedMapX );
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

        // update drop downs
        auto& mp       = _mapBanks[ _selectedBank ].m_bank->m_mapData[ p_mapY ][ p_mapX ];
        u8    ts1      = mp.m_tIdx1;
        u8    ts2      = mp.m_tIdx2;
        _disableRedraw = true;
        _mapEditorBS1CB.set_selected( _blockSets[ ts1 ].m_stringListItem );
        _mapEditorBS2CB.set_selected( _blockSets[ ts2 ].m_stringListItem );
        _disableRedraw = false;
        currentMapUpdateTS1( ts1 );
        currentMapUpdateTS2( ts2 );
        fprintf( stderr, "[LOG] Loading map %hu/%hhu_%hhu.\n", p_bank, p_mapY, p_mapX );
    }
} // namespace UI
