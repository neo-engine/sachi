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
#include "log.h"
#include "ui_root.h"

namespace UI {
    const std::string APP_NAME     = std::string( "Sachi" );
    const std::string TITLE_STRING = APP_NAME + " " + std::string( VERSION );

    void root::addFsRootToRecent( const std::string& p_path ) {
        auto res       = Gtk::RecentManager::Data( );
        res.app_name   = APP_NAME;
        res.is_private = true;
        if( !_recentlyUsedFsRoots->add_item( "file://" + p_path, res ) ) {
            message_log( "addFsRootToRecent", "Adding \"" + p_path + "\" failed.", LOGLEVEL_DEBUG );
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

    void root::initActions( ) {
        _loadActions = Gio::SimpleActionGroup::create( );
        _saveActions = Gio::SimpleActionGroup::create( );
        _loadFsrootAction
            = _loadActions->add_action( "fsroot", [ & ]( ) { this->onFsRootOpenClick( ); } );
        _loadReloadmapAction     = _loadActions->add_action( "reloadmap", [ & ]( ) {
            readMapSlice( _selectedBank, _selectedMapX, _selectedMapY );
            redrawMap( _selectedMapY, _selectedMapX );
        } );
        _loadReloadmapbankAction = _loadActions->add_action( "reloadmapbank", [ & ]( ) {
            readMapBank( _selectedBank, true );
            redrawMap( _selectedMapY, _selectedMapX );
        } );
        _loadImportmapAction     = _loadActions->add_action( "importmap", [ & ]( ) {
            auto dialog    = new Gtk::FileChooserDialog( "Choose a map to import",
                                                      Gtk::FileChooser::Action::OPEN, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.map" );
            mapFilter->set_name( "AdvanceMap 1.92 Map Files (32x32 blocks)" );
            dialog->set_filter( mapFilter );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "importMap", "Dialog is nullptr." );
                    return;
                }

                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    readMapSlice( _selectedBank, _selectedMapX, _selectedMapY,
                                  dialog->get_file( )->get_path( ) );
                    redrawMap( _selectedMapY, _selectedMapX );
                    markBankChanged( _selectedBank );
                    break;
                }
                default:
                case Gtk::ResponseType::CANCEL: break;
                }
                delete dialog;
            } );

            dialog->add_button( "_Cancel", Gtk::ResponseType::CANCEL );
            dialog->add_button( "_Select", Gtk::ResponseType::OK );
            dialog->show( );
        } );

        _saveFsrootAction
            = _saveActions->add_action( "fsroot", [ & ]( ) { this->onFsRootSaveClick( ); } );
        _saveMapAction = _saveActions->add_action(
            "map", [ & ]( ) { writeMapSlice( _selectedBank, _selectedMapX, _selectedMapY ); } );
        _saveMapbankAction
            = _saveActions->add_action( "mapbank", [ & ]( ) { writeMapBank( _selectedBank ); } );
        _saveExportmapAction = _saveActions->add_action( "exportmap", [ & ]( ) {
            auto dialog    = new Gtk::FileChooserDialog( "Save the current map",
                                                      Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.map" );
            mapFilter->set_name( "AdvanceMap 1.92 Map Files (32x32 blocks)" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _selectedMapY ) + "_"
                                      + std::to_string( _selectedMapX ) + ".map " );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "exportMap", "Dialog is nullptr." );
                    return;
                }
                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    writeMapSlice( _selectedBank, _selectedMapX, _selectedMapY,
                                   dialog->get_file( )->get_path( ) );
                    break;
                }
                default:
                case Gtk::ResponseType::CANCEL: break;
                }
                delete dialog;
            } );

            dialog->add_button( "_Cancel", Gtk::ResponseType::CANCEL );
            dialog->add_button( "_Select", Gtk::ResponseType::OK );
            dialog->show( );
        } );

        insert_action_group( "load", _loadActions );
        insert_action_group( "save", _saveActions );
    }

    void root::initHeaderBar( ) {
        // Header bar

        auto headerBar = Gtk::HeaderBar( );
        set_titlebar( headerBar );

        auto headerTitleBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        headerTitleBox.append( _titleLabel );
        headerTitleBox.append( _subtitleLabel );

        _titleLabel.get_style_context( )->add_class( "title" );
        _subtitleLabel.get_style_context( )->add_class( "subtitle" );
        headerBar.set_title_widget( headerTitleBox );
        headerTitleBox.set_valign( Gtk::Align::CENTER );

        _openButton = createButton( "", "_Load FSROOT…", [ & ]( ) { this->onFsRootOpenClick( ); } );
        _saveButton = createButton( "", "_Save Changes", [ & ]( ) { this->onFsRootSaveClick( ); } );

        _openMenu = Gio::Menu::create( );
        _saveMenu = Gio::Menu::create( );

        _openMenu->append( "Reload Map", "load.reloadmap" );
        _openMenu->append( "Reload Map Bank", "load.reloadmapbank" );
        _openMenu->append( "Import Map", "load.importmap" );

        _saveMenu->append( "Save Single Map", "save.map" );
        _saveMenu->append( "Save Single Mapbank", "save.mapbank" );
        _saveMenu->append( "Export Map", "save.exportmap" );

        _openMenuPopover = Gtk::PopoverMenu( _openMenu );
        _openMenuPopover.set_has_arrow( false );
        _saveMenuPopover = Gtk::PopoverMenu( _saveMenu );
        _saveMenuPopover.set_has_arrow( false );

        _openMenuButton = std::make_shared<Gtk::MenuButton>( );
        _openMenuButton->set_popover( _openMenuPopover );
        _saveMenuButton = std::make_shared<Gtk::MenuButton>( );
        _saveMenuButton->set_popover( _saveMenuPopover );

        _collapseMapBanksButton
            = createButton( "view-restore-symbolic", "_Collapse Sidebar",
                            [ & ]( ) { this->collapseMapBankBar( !_mapBankBarCollapsed ); } );
        _collapseMapBanksButton->set_expand( true );
        _collapseMapBanksButton->set_margin( MARGIN );
        _collapseMapBanksButton->set_has_frame( false );

        auto openBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        openBox.get_style_context( )->add_class( "linked" );
        openBox.append( *_openButton );
        openBox.append( *_openMenuButton );

        auto saveBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        saveBox.get_style_context( )->add_class( "linked" );
        saveBox.append( *_saveButton );
        saveBox.append( *_saveMenuButton );
        headerBar.pack_start( openBox );
        headerBar.pack_start( saveBox );
    }

    void root::initSideBar( ) {
        // main box
        auto lMainBox = Gtk::Box( Gtk::Orientation::VERTICAL, MARGIN );
        _mainBox.append( lMainBox );

        {
            // collapse sidebar widget
            auto sideBarBarCollapseBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            sideBarBarCollapseBox.append( *_collapseMapBanksButton );
            sideBarBarCollapseBox.set_halign( Gtk::Align::CENTER );
            _collapseMapBanksButton->set_expand( false );
            lMainBox.append( sideBarBarCollapseBox );
        }
        {
            // tile set
            auto lFrame = Gtk::Frame( );
            lFrame.set_margin( MARGIN );
            lFrame.set_margin_top( 0 );
            lFrame.set_margin_bottom( 0 );
            lMainBox.append( lFrame );

            auto sbTileSetBarLabelBox   = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            auto sbTileSetBarLabelImage = Gtk::Image( );
            sbTileSetBarLabelImage.set_from_icon_name( "applications-graphics-symbolic" );
            sbTileSetBarLabelImage.set_margin( MARGIN );
            _sbTileSetBarLabel = Gtk::Label( "Tile Sets" );
            sbTileSetBarLabelBox.append( sbTileSetBarLabelImage );
            sbTileSetBarLabelBox.append( _sbTileSetBarLabel );

            lFrame.set_label_widget( sbTileSetBarLabelBox );
            lFrame.set_label_align( Gtk::Align::CENTER );

            _sbTileSetBox = Gtk::Box( Gtk::Orientation::VERTICAL, MARGIN );
            lFrame.set_child( _sbTileSetBox );
            _sbTileSetBox.set_margin( MARGIN );
            _sbTileSetBox.set_vexpand( false );

            _editTileSet = std::make_shared<editTileSet>( );
            if( _editTileSet ) {
                _sbTileSetBox.append( *_editTileSet );
                _editTileSet->connect(
                    [ this ]( u8 p_ts1, u8 p_ts2 ) { editTileSets( p_ts1, p_ts2 ); } );
            }

            _sbTileSetSel1 = 0;
            _sbTileSetSel2 = 1;
        }
        {
            // map banks
            auto lScrolledWindow = Gtk::ScrolledWindow( );
            lMainBox.append( lScrolledWindow );
            auto lFrame = Gtk::Frame( );
            lFrame.set_margin( MARGIN );
            lScrolledWindow.set_child( lFrame );
            lScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );

            auto mapBankBarLabelBox   = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            auto mapBankBarLabelImage = Gtk::Image( );
            mapBankBarLabelImage.set_from_icon_name( "image-x-generic-symbolic" );
            mapBankBarLabelImage.set_margin( MARGIN );
            _mapBankBarLabel = Gtk::Label( "Map Banks" );
            mapBankBarLabelBox.append( mapBankBarLabelImage );
            mapBankBarLabelBox.append( _mapBankBarLabel );

            lFrame.set_label_widget( mapBankBarLabelBox );
            lFrame.set_label_align( Gtk::Align::CENTER );

            _mapBankBox = Gtk::Box( Gtk::Orientation::VERTICAL, MARGIN );
            lFrame.set_child( _mapBankBox );
            _mapBankBox.set_margin( MARGIN );
            _mapBankBox.set_vexpand( true );

            _addMapBank = std::make_shared<addMapBank>( );

            if( _addMapBank ) {
                _mapBankBox.append( *_addMapBank );
                _addMapBank->connect(
                    [ this ]( u16 p_bk, u8 p_y, u8 p_x ) { createMapBank( p_bk, p_y, p_x ); } );
            }

            _mapBanks.clear( );
            _selectedBank = -1;
        }
    }

    void root::initWelcome( ) {
        // Main window on application start:
        // Show icon view of recently used FSROOT folders

        _ivScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
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
    }

    void root::initTileSetEditor( ) {
        // Tile set editor
        _tseNotebook.set_expand( );
        auto tseMainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        tseMainBox.set_margin_top( MARGIN );

        auto tseScrolledWindow0 = Gtk::ScrolledWindow( );
        tseScrolledWindow0.set_child( tseMainBox );
        tseScrolledWindow0.set_policy( Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER );

        auto tseSettingsBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        tseSettingsBox.set_margin( MARGIN );

        _tseNotebook.append_page( tseScrolledWindow0, "Tile Set _Editor", true );
        _tseNotebook.append_page( tseSettingsBox, "Tile Set Settin_gs", true );
        _mainBox.append( _tseNotebook );

        // ts editor

        auto ehbox1f1 = Gtk::Frame( "Blocks" );
        ehbox1f1.set_label_align( Gtk::Align::CENTER );

        tseMainBox.append( ehbox1f1 );
        tseMainBox.set_spacing( MARGIN );
        tseMainBox.set_margin( MARGIN );

        // block chooser

        auto eboxv1 = Gtk::Box( Gtk::Orientation::VERTICAL );
        ehbox1f1.set_child( eboxv1 );
        eboxv1.set_margin( MARGIN );
        eboxv1.set_vexpand( true );

        auto tseScrolledWindow1 = Gtk::ScrolledWindow( );
        tseScrolledWindow1.set_child( _tsets1widget );
        tseScrolledWindow1.set_margin( MARGIN );
        tseScrolledWindow1.set_vexpand( );
        tseScrolledWindow1.set_halign( Gtk::Align::CENTER );
        tseScrolledWindow1.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        eboxv1.append( tseScrolledWindow1 );

        _tsets1widget.connectClick(
            [ this ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                onTSETSClicked( p_button, p_blockX, p_blockY, 0 );
            } );

        auto tseScrolledWindow2 = Gtk::ScrolledWindow( );
        tseScrolledWindow2.set_child( _tsets2widget );
        tseScrolledWindow2.set_margin( MARGIN );
        tseScrolledWindow2.set_vexpand( );
        tseScrolledWindow2.set_halign( Gtk::Align::CENTER );
        tseScrolledWindow2.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        eboxv1.append( tseScrolledWindow2 );

        _tsets2widget.connectClick(
            [ this ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                onTSETSClicked( p_button, p_blockX, p_blockY, 1 );
            } );

        auto eboxv2 = Gtk::Box( Gtk::Orientation::VERTICAL );
        eboxv2.set_spacing( MARGIN );
        tseMainBox.append( eboxv2 );

        // block being edited
        _editBlock = std::make_shared<editableBlock>( );
        eboxv2.append( *_editBlock );
        _editBlock->connect(
            [ this ]( u8 p_newMajBehave ) {
                if( _tseSelectedBlockIdx > DATA::MAX_BLOCKS_PER_TILE_SET ) {
                    auto blk = _tseSelectedBlockIdx - DATA::MAX_BLOCKS_PER_TILE_SET;
                    printf( "Change from %hx to %hx\n",
                            _currentBlockset2[ blk ].first.m_bottombehave, p_newMajBehave );
                    _currentBlockset2[ blk ].first.m_bottombehave = p_newMajBehave;
                    _blockSets[ _sbTileSetSel2 ].m_blockSet.m_blocks[ blk ].m_bottombehave
                        = p_newMajBehave;
                } else {
                    auto blk                                      = _tseSelectedBlockIdx;
                    _currentBlockset1[ blk ].first.m_bottombehave = p_newMajBehave;
                    _blockSets[ _sbTileSetSel1 ].m_blockSet.m_blocks[ blk ].m_bottombehave
                        = p_newMajBehave;
                }
                markTileSetsChanged( );
            },
            [ this ]( u8 p_newMinBehave ) {
                if( _tseSelectedBlockIdx > DATA::MAX_BLOCKS_PER_TILE_SET ) {
                    auto blk = _tseSelectedBlockIdx - DATA::MAX_BLOCKS_PER_TILE_SET;
                    _currentBlockset2[ blk ].first.m_topbehave = p_newMinBehave;
                    _blockSets[ _sbTileSetSel2 ].m_blockSet.m_blocks[ blk ].m_topbehave
                        = p_newMinBehave;
                } else {
                    auto blk                                   = _tseSelectedBlockIdx;
                    _currentBlockset1[ blk ].first.m_topbehave = p_newMinBehave;
                    _blockSets[ _sbTileSetSel1 ].m_blockSet.m_blocks[ blk ].m_topbehave
                        = p_newMinBehave;
                }
                markTileSetsChanged( );
            } );

        // current palette selector

        // ts / palette
        auto eboxh1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        eboxv2.append( eboxh1 );
        eboxh1.set_spacing( MARGIN );

        // ts
        auto ehbox1f2 = Gtk::Frame( "Tiles" );
        ehbox1f2.set_label_align( Gtk::Align::CENTER );
        auto eboxv3 = Gtk::Box( Gtk::Orientation::VERTICAL );
        eboxv3.set_expand( true );
        eboxh1.append( ehbox1f2 );
        ehbox1f2.set_child( eboxv3 );

        // palette
        auto ehbox1f3 = Gtk::Frame( "Palette" );
        ehbox1f3.set_label_align( Gtk::Align::CENTER );
        auto eboxv4 = Gtk::Box( Gtk::Orientation::VERTICAL );
        eboxv4.set_vexpand( true );
        eboxh1.append( ehbox1f3 );
        ehbox1f3.set_child( eboxv4 );

        // ts settings
        // - tile set mode
        //

        auto shbox1f1 = Gtk::Frame( "General Settings" );
        shbox1f1.set_label_align( Gtk::Align::CENTER );

        auto sboxv1 = Gtk::Box( Gtk::Orientation::VERTICAL );
        shbox1f1.set_child( sboxv1 );
        sboxv1.set_margin( MARGIN );

        auto shbox1 = Gtk::CenterBox( );
        shbox1.set_hexpand( true );
        auto shbox1l = Gtk::Label( "Tile Set Mode" );
        shbox1.set_start_widget( shbox1l );

        _tseTileModeToggles.push_back( std::make_shared<Gtk::ToggleButton>( "_Simple", true ) );
        _tseTileModeToggles.push_back( std::make_shared<Gtk::ToggleButton>( "_Combined", true ) );

        auto mapBankSettingsMapModeBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        mapBankSettingsMapModeBox.get_style_context( )->add_class( "linked" );
        mapBankSettingsMapModeBox.set_halign( Gtk::Align::CENTER );
        for( u8 i = 0; i < _tseTileModeToggles.size( ); ++i ) {
            mapBankSettingsMapModeBox.append( *_tseTileModeToggles[ i ] );
            _tseTileModeToggles[ i ]->signal_clicked( ).connect( [ this, i ]( ) {
                setTileSetMode( i );
                markTileSetsChanged( );
            } );
            if( i ) { _tseTileModeToggles[ i ]->set_group( *_tseTileModeToggles[ 0 ] ); }
        }
        _tseTileModeToggles[ 0 ]->set_active( );
        shbox1.set_end_widget( mapBankSettingsMapModeBox );

        sboxv1.append( shbox1 );
        tseSettingsBox.append( shbox1f1 );
    }

    void root::initMapEditor( ) {
        // Map editor
        _mapNotebook.set_expand( );
        auto mapEditorMainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        mapEditorMainBox.set_margin_top( MARGIN );
        auto mapEditorMainBox0 = Gtk::Box( Gtk::Orientation::VERTICAL );
        mapEditorMainBox0.set_margin( MARGIN );
        auto mapEditorModeBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        mapEditorModeBox.set_margin( MARGIN );
        mapEditorMainBox0.append( mapEditorModeBox );
        mapEditorMainBox0.append( mapEditorMainBox );

        // mode change (block edit, movement edit, locations, events, map data) buttons
        _mapEditorModeToggles.push_back( std::make_shared<Gtk::ToggleButton>( "_Blocks", true ) );
        _mapEditorModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "_Movements", true ) );
        _mapEditorModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "Loca_tions", true ) );
        _mapEditorModeToggles.push_back( std::make_shared<Gtk::ToggleButton>( "E_vents", true ) );
        _mapEditorModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "Meta Data / _Wild PKMN", true ) );

        mapEditorModeBox.get_style_context( )->add_class( "linked" );
        mapEditorModeBox.set_halign( Gtk::Align::CENTER );
        for( u8 i = 0; i < 5; ++i ) {
            mapEditorModeBox.append( *_mapEditorModeToggles[ i ] );
            _mapEditorModeToggles[ i ]->signal_clicked( ).connect(
                [ this, i ]( ) { setNewMapEditMode( mapDisplayMode( i ) ); } );
            if( i ) { _mapEditorModeToggles[ i ]->set_group( *_mapEditorModeToggles[ 0 ] ); }
        }
        _mapEditorModeToggles[ 0 ]->set_active( );

        _mapNotebook.append_page( mapEditorMainBox0, "Map _Editor", true );
        _mapNotebook.append_page( _mapOverviewBox, "Bank _Overview", true );
        _mapNotebook.append_page( _mapSettingsBox, "Bank Settin_gs", true );
        _mainBox.append( _mapNotebook );

        mapEditorMainBox.append( _mapEditorMapBox );
        _mapEditorMapBox.set_expand( );

        // Movements
        _movementFrame = Gtk::Frame( "Movements" );
        _movementFrame.set_margin_start( MARGIN );
        _movementFrame.set_label_align( Gtk::Align::CENTER );
        mapEditorMainBox.append( _movementFrame );

        auto meScrolledWindow = Gtk::ScrolledWindow( );
        meScrolledWindow.set_margin( MARGIN );
        meScrolledWindow.set_child( _movementWidget );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _movementWidget.setOverlayOpacity( .9 );
        _movementWidget.set( DATA::mapBlockAtom::computeMovementSet( ), pals, 1 );
        _movementWidget.setOverlayHidden( false );
        _movementWidget.draw( );
        _movementWidget.setScale( 2 );
        _movementWidget.queue_resize( );
        _movementWidget.connectClick( [ this ]( UI::mapSlice::clickType, u16, u16 p_blockY ) {
            updateSelectedBlock( { 0, DATA::mapBlockAtom::MOVEMENT_ORDER[ p_blockY ] } );
        } );

        meScrolledWindow.set_margin( MARGIN );
        meScrolledWindow.set_vexpand( );
        meScrolledWindow.set_halign( Gtk::Align::CENTER );
        meScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _movementFrame.set_child( meScrolledWindow );

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

        _blockSetFrame = Gtk::Frame( "Tile Sets" );
        _blockSetFrame.set_margin_start( MARGIN );
        _blockSetFrame.set_label_align( Gtk::Align::CENTER );
        mapEditorMainBox.append( _blockSetFrame );
        _blockSetFrame.set_child( _mapEditorBlockSetBox );

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

        _ts1widget.connectClick(
            [ this ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                onTSClicked( p_button, p_blockX, p_blockY, 0 );
            } );

        auto meScrolledWindow2 = Gtk::ScrolledWindow( );
        meScrolledWindow2.set_child( _ts2widget );
        meScrolledWindow2.set_margin( MARGIN );
        meScrolledWindow2.set_vexpand( );
        meScrolledWindow2.set_halign( Gtk::Align::CENTER );
        meScrolledWindow2.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mapEditorBlockSetBox.append( meScrolledWindow2 );

        _ts2widget.connectClick(
            [ this ]( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                onTSClicked( p_button, p_blockX, p_blockY, 1 );
            } );

        // Map window
        // +---------------+
        // |               |
        // |      Map      |
        // |               |
        // | ------------- |
        // |  Action Bar   |
        // +---------------+

        auto rScrolledWindow = Gtk::ScrolledWindow( );
        rScrolledWindow.set_child( _mapGrid );
        _mapGrid.set_halign( Gtk::Align::CENTER );
        _mapGrid.set_valign( Gtk::Align::CENTER );

        for( u8 x = 0; x < 3; ++x ) {
            _currentMap.push_back( std::vector<lookupMapSlice>( 3 ) );
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
        _mapEditorMapBox.append( _mapEditorActionBar );
        _mapEditorActionBar.set_child( abScrolledWindow );
        _mapEditorActionBar.set_margin_top( MARGIN );
        abScrolledWindow.set_policy( Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER );
        auto mapEditorActions = Gtk::CenterBox( );

        abScrolledWindow.set_child( mapEditorActions );

        // action bar start
        auto abStartBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abStartBox.set_valign( Gtk::Align::CENTER );
        mapEditorActions.set_start_widget( abStartBox );
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
            _movementWidget.setSpacing( value );
            _movementWidget.queue_resize( );
            _blockStampMap.setSpacing( value );
            _blockStampMap.queue_resize( );
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
            _movementWidget.setScale( value > 1 ? value : 2 );
            _movementWidget.queue_resize( );
            _blockStampMap.setScale( value );
            _blockStampMap.queue_resize( );
            _blockScale = value;
        } );

        _mapEditorSettings3 = Gtk::SpinButton( abSAdj3 );
        _mapEditorSettings3.set_margin_start( MARGIN );
        _mapEditorSettings3.set_wrap( );
        _mapEditorSettings3.set_width_chars( 1 );
        _mapEditorSettings3.set_max_width_chars( 1 );
        _mapEditorSettings3.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapEditorSettings3.get_value_as_int( );
            setCurrentDaytime( value );
        } );

        auto abSl1 = Gtk::Image( );
        abSl1.set_from_icon_name( "view-grid-symbolic" );
        abSl1.set_margin_start( MARGIN );
        auto abSl2 = Gtk::Image( );
        abSl2.set_from_icon_name( "edit-find-symbolic" );
        abSl2.set_margin_start( MARGIN );
        auto abSl3 = Gtk::Image( );
        abSl3.set_from_icon_name( "weather-clear-symbolic" );
        abSl3.set_margin_start( MARGIN );
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
        mapEditorActions.set_center_widget( mapNavGrid );

        // action bar end

        auto abEndBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        abEndBox.set_valign( Gtk::Align::CENTER );
        mapEditorActions.set_end_widget( abEndBox );
        _abEb1     = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abEb2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abEb3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abEndBox.append( _abEb1 );
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
            auto mp = _mapBanks[ _selectedBank ].m_bank->m_slices[ _selectedMapY ][ _selectedMapX ];
            auto ts = DATA::tileSet<2>( );
            buildTileSet( &ts );
            DATA::palette pals[ 16 * 5 ] = { 0 };
            buildPalette( pals );

            _ts1widget.set( DATA::mapBlockAtom::computeBlockSet(
                                &_blockSets[ mp.m_data.m_tIdx1 ].m_blockSet, &ts ),
                            pals, _blockSetWidth );

            _ts2widget.set( DATA::mapBlockAtom::computeBlockSet(
                                &_blockSets[ mp.m_data.m_tIdx2 ].m_blockSet, &ts ),
                            pals, _blockSetWidth );

            _ts1widget.setDaytime( _currentDayTime );
            _ts2widget.setDaytime( _currentDayTime );
            _ts1widget.draw( );
            _ts2widget.draw( );
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
        _abEb1.append( abEl1 );
        _abEb1.append( _mapEditorSettings4 );
        abEb2.append( abEl2 );
        abEb2.append( _mapEditorSettings5 );
        abEb3.append( abEl3 );
        abEb3.append( _mapEditorSettings6 );

        // Map overview box
        // +----------------------+
        // | ( Maps | Locations ) |
        // |+--------------------+|
        // ||                    ||
        // ||        Maps        ||
        // ||                    ||
        // |+--------------------+|
        // |     ( Actionbar )    |
        // +----------------------+

        _mapOverviewBox.set_margin( MARGIN );

        // mode change (block edit, locations) buttons
        auto bankOverviewModeBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        bankOverviewModeBox.set_margin( MARGIN );
        _bankOverviewModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "_Blocks", true ) );
        _bankOverviewModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "Loca_tions", true ) );

        bankOverviewModeBox.get_style_context( )->add_class( "linked" );
        bankOverviewModeBox.set_halign( Gtk::Align::CENTER );
        for( u8 i = 0; i < _bankOverviewModeToggles.size( ); ++i ) {
            bankOverviewModeBox.append( *_bankOverviewModeToggles[ i ] );
            _bankOverviewModeToggles[ i ]->signal_clicked( ).connect(
                [ this, i ]( ) { setNewBankOverviewMode( bankOverviewMode( i ) ); } );
            if( i ) { _bankOverviewModeToggles[ i ]->set_group( *_bankOverviewModeToggles[ 0 ] ); }
        }
        _bankOverviewModeToggles[ 0 ]->set_active( );

        auto boswbox = Gtk::Box( );
        boswbox.set_halign( Gtk::Align::CENTER );
        boswbox.set_valign( Gtk::Align::CENTER );
        boswbox.set_expand( true );

        auto bankOverviewSW = Gtk::ScrolledWindow( );
        bankOverviewSW.set_margin( MARGIN );
        bankOverviewSW.set_policy( Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC );
        bankOverviewSW.set_child( boswbox );
        boswbox.append( _mapBankOverview );

        auto abBOBox = Gtk::CenterBox( );
        auto abSbBO1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abSbBO2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto abSbBO3 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        abBOBox.set_start_widget( abSbBO1 );
        abBOBox.set_center_widget( abSbBO2 );
        abBOBox.set_end_widget( abSbBO3 );

        auto abBOFrame = Gtk::Frame( );
        abBOFrame.set_child( abBOBox );

        auto abSAdjBO1 = Gtk::Adjustment::create( _bankOverviewSpacing, 0.0, 9.0, 1.0, 1.0, 0.0 );
        auto abSAdjBO2 = Gtk::Adjustment::create( _bankOverviewScale, 1.0, 8.0, 1.0, 1.0, 0.0 );

        _mapBankOverviewSettings1 = Gtk::SpinButton( abSAdjBO1 );
        _mapBankOverviewSettings1.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapBankOverviewSettings1.get_value_as_int( );

            _mapBankOverview.setSpacing( value );
            _mapBankOverview.queue_resize( );

            _bankOverviewSpacing = value;
        } );
        _mapBankOverviewSettings1.set_margin_start( MARGIN );
        _mapBankOverviewSettings1.set_width_chars( 1 );
        _mapBankOverviewSettings1.set_max_width_chars( 1 );

        _mapBankOverviewSettings2 = Gtk::SpinButton( abSAdjBO2 );
        _mapBankOverviewSettings2.set_margin_start( MARGIN );
        _mapBankOverviewSettings2.set_width_chars( 1 );
        _mapBankOverviewSettings2.set_max_width_chars( 1 );
        _mapBankOverviewSettings2.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapBankOverviewSettings2.get_value_as_int( );

            _mapBankOverview.setScale( value );
            _mapBankOverview.queue_resize( );

            _bankOverviewScale = value;
        } );

        _mapBankOverviewSettings3 = Gtk::SpinButton( abSAdj3 );
        _mapBankOverviewSettings3.set_margin_start( MARGIN );
        _mapBankOverviewSettings3.set_wrap( );
        _mapBankOverviewSettings3.set_width_chars( 1 );
        _mapBankOverviewSettings3.set_max_width_chars( 1 );
        _mapBankOverviewSettings3.signal_value_changed( ).connect( [ & ]( ) {
            auto value = _mapBankOverviewSettings3.get_value_as_int( );
            setCurrentDaytime( value );
        } );

        _mapBankOverview.connectClick(
            [ this ]( UI::mapBankOverview::clickType, u16 p_mapX, u16 p_mapY ) {
                onUnloadMap( _selectedBank, _selectedMapY, _selectedMapX );
                loadMap( _selectedBank, p_mapY, p_mapX );
            } );

        auto abSlBO1 = Gtk::Image( );
        abSlBO1.set_from_icon_name( "view-grid-symbolic" );
        abSlBO1.set_margin_start( MARGIN );
        auto abSlBO2 = Gtk::Image( );
        abSlBO2.set_from_icon_name( "edit-find-symbolic" );
        abSlBO2.set_margin_start( MARGIN );
        auto abSlBO3 = Gtk::Image( );
        abSlBO3.set_from_icon_name( "weather-clear-symbolic" );
        abSlBO3.set_margin_start( MARGIN );
        abSbBO1.append( abSlBO1 );
        abSbBO1.append( _mapBankOverviewSettings1 );
        abSbBO2.append( abSlBO2 );
        abSbBO2.append( _mapBankOverviewSettings2 );
        abSbBO3.append( abSlBO3 );
        abSbBO3.append( _mapBankOverviewSettings3 );

        _mapOverviewBox.append( bankOverviewModeBox );
        _mapOverviewBox.append( bankOverviewSW );
        _mapOverviewBox.append( abBOFrame );
        _mapOverviewBox.set_margin( MARGIN );

        // Map bank settings
        // - sizeX, sizeY
        // - map mode
        //

        _mapSettingsBox.set_margin( MARGIN );

        auto shbox1f1 = Gtk::Frame( "General Settings" );
        shbox1f1.set_label_align( Gtk::Align::CENTER );

        auto sboxv1 = Gtk::Box( Gtk::Orientation::VERTICAL );
        shbox1f1.set_child( sboxv1 );
        sboxv1.set_margin( MARGIN );

        auto shbox1 = Gtk::CenterBox( );
        shbox1.set_hexpand( true );
        auto shbox1l = Gtk::Label( "Map Mode" );
        shbox1.set_start_widget( shbox1l );

        _mapBankSettingsMapModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "_Simple", true ) );
        _mapBankSettingsMapModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "Sc_attered", true ) );
        _mapBankSettingsMapModeToggles.push_back(
            std::make_shared<Gtk::ToggleButton>( "_Combined", true ) );

        auto mapBankSettingsMapModeBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        mapBankSettingsMapModeBox.get_style_context( )->add_class( "linked" );
        mapBankSettingsMapModeBox.set_halign( Gtk::Align::CENTER );
        for( u8 i = 0; i < _mapBankSettingsMapModeToggles.size( ); ++i ) {
            mapBankSettingsMapModeBox.append( *_mapBankSettingsMapModeToggles[ i ] );
            _mapBankSettingsMapModeToggles[ i ]->signal_clicked( ).connect( [ this, i ]( ) {
                _mapBanks[ _selectedBank ].setMapMode( i );
                markBankChanged( _selectedBank );
            } );
            if( i ) {
                _mapBankSettingsMapModeToggles[ i ]->set_group(
                    *_mapBankSettingsMapModeToggles[ 0 ] );
            }
        }
        _mapBankSettingsMapModeToggles[ 0 ]->set_active( );
        shbox1.set_end_widget( mapBankSettingsMapModeBox );

        sboxv1.append( shbox1 );
        _mapSettingsBox.append( shbox1f1 );
    }

    void root::initEvents( ) {
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
                    if( _focusMode ) {
                        _mapEditorActionBar.hide( );
                    } else {
                        _mapEditorActionBar.show( );
                    }
                    return true;
                }

                return false;
            },
            false );

        add_controller( controller );
    }

    root::root( ) {
        set_default_size( 800, 600 );

        auto provider = Gtk::CssProvider::create( );
        provider->load_from_data( EXTRA_CSS );
        Gtk::StyleContext::add_provider_for_display( Gdk::Display::get_default( ), provider,
                                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );

        _recentlyUsedFsRoots = Gtk::RecentManager::get_default( );

        initActions( );
        initHeaderBar( );

        auto mbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _mainBox  = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        set_child( mbox );
        mbox.append( _mainBox );
        mbox.append( _ivScrolledWindow );

        initSideBar( );

        initWelcome( );

        _loadMapLabel = Gtk::Label( );
        _loadMapLabel.set_markup( "<span size=\"x-large\">Add or load a map bank!</span>" );
        _loadMapLabel.set_expand( );
        _mainBox.append( _loadMapLabel );

        initTileSetEditor( );
        initMapEditor( );

        initEvents( );

        setNewMapEditMode( MODE_EDIT_TILES );
        populateRecentFsRootIconView( );
        switchContext( NONE );
    }

    root::~root( ) {
    }

    void root::setNewBankOverviewMode( bankOverviewMode p_newMode ) {
        // TDOO
    }

    void root::setCurrentDaytime( u8 p_newDaytime ) {
        // Change daytime of everything map-py
        _currentDayTime = p_newDaytime;

        // update block sets first, their image data is used for the other maps
        _ts1widget.setDaytime( _currentDayTime );
        _ts2widget.setDaytime( _currentDayTime );
        _ts1widget.draw( );
        _ts2widget.draw( );

        _blockStampMap.draw( );

        for( u8 x = 0; x < 3; ++x ) {
            for( u8 y = 0; y < 3; ++y ) { _currentMap[ x ][ y ].draw( ); }
        }

        _mapBankOverview.redraw( p_newDaytime );
    }

    void root::setNewMapEditMode( mapDisplayMode p_newMode ) {
        _movementFrame.hide( );
        _blockSetFrame.hide( );
        _abEb1.hide( );
        for( u8 x = 0; x < 3; ++x ) {
            for( u8 y = 0; y < 3; ++y ) { _currentMap[ x ][ y ].setOverlayHidden( true ); }
        }
        _blockStampMap.setOverlayHidden( true );
        switch( p_newMode ) {
        case MODE_EDIT_TILES:
            // Show tileset bar
            _blockSetFrame.show( );
            _abEb1.show( );

            break;
        case MODE_EDIT_MOVEMENT:
            // Show movement data bar
            _movementFrame.show( );
            for( u8 x = 0; x < 3; ++x ) {
                for( u8 y = 0; y < 3; ++y ) { _currentMap[ x ][ y ].setOverlayHidden( false ); }
            }
            _blockStampMap.setOverlayHidden( false );

            break;
        case MODE_EDIT_LOCATIONS:
            // Show locations overlay
            if( _blockStampDialog && !_blockStampDialogInvalid ) { _blockStampDialog->hide( ); }
            _blockStampDialogInvalid = true;
            break;
        case MODE_EDIT_EVENTS:
            // Show events
            if( _blockStampDialog && !_blockStampDialogInvalid ) { _blockStampDialog->hide( ); }
            _blockStampDialogInvalid = true;
            break;
        case MODE_EDIT_DATA:
            // show map information box
            if( _blockStampDialog && !_blockStampDialogInvalid ) { _blockStampDialog->hide( ); }
            _blockStampDialogInvalid = true;
            break;
        default: return;
        }
        _currentMapDisplayMode = p_newMode;
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

    bool root::writeMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path,
                              bool p_writeMapData ) {
        if( !_mapBanks.count( p_bank ) ) { return true; }

        auto        path = fs::path( MAP_PATH ) / std::to_string( p_bank );
        const auto& info = _mapBanks[ p_bank ];
        if( info.isScattered( ) ) { path /= std::to_string( p_mapY ); }
        path /= std::to_string( p_mapY ) + "_" + std::to_string( p_mapX ) + ".map";

        if( p_path != "" ) { path = p_path; }

        fs::create_directories( path.parent_path( ) );
        FILE* f = fopen( path.c_str( ), "w" );
        if( !DATA::writeMapSlice( f, &info.m_bank->m_slices[ p_mapY ][ p_mapX ] ) ) {
            message_log( "writeMapSlice", ( "Writing map "s ) + std::to_string( p_bank ) + ( "/"s )
                                              + std::to_string( p_mapY ) + ( "_"s )
                                              + std::to_string( p_mapX ) + ( ".map to "s )
                                              + path.string( ) + ( " failed."s ) );
            return true;
        }
        if( p_writeMapData ) {
            path += ".data";
            f = fopen( path.c_str( ), "wb" );
            if( !DATA::writeMapData( f, &info.m_bank->m_mapData[ p_mapY ][ p_mapX ] ) ) {
                message_log( "writeMapSlice", ( "Writing map data "s ) + std::to_string( p_bank )
                                                  + ( "/"s ) + std::to_string( p_mapY ) + ( "_"s )
                                                  + std::to_string( p_mapX ) + ( ".map.data to "s )
                                                  + path.string( ) + ( " failed."s ) );
                return true;
            }
        }

        return false;
    }

    bool root::readTileSets( ) {
        // first check if a combined file exists
        bool error = false;

        FILE* f;
        auto  pth = fs::path( MAP_PATH ) / "tileset.tsb";
        f         = fopen( pth.c_str( ), "rb" );

        if( f ) {
            _tileSetMode = DATA::TILEMODE_COMBINED;
            _tseTileModeToggles[ 1 ]->set_active( );

            // read header
            auto header = DATA::blockSetBankHeader( );
            fread( &header, sizeof( DATA::blockSetBankHeader ), 1, f );

            for( auto i = 0; i < header.m_blockSetCount; ++i ) {
                auto bInfo = blockSetInfo( );
                error |= !DATA::readTiles( f, bInfo.m_tileSet.m_tiles );
                error |= !DATA::readBlocks( f, bInfo.m_blockSet.m_blocks );
                if( header.m_dayTimeCount <= DAYTIMES ) {
                    error |= !DATA::readPal( f, bInfo.m_pals, 8 * header.m_dayTimeCount );
                } else {
                    error |= !DATA::readPal( f, bInfo.m_pals, 8 * DAYTIMES );
                    DATA::readNop( f,
                                   sizeof( u16 ) * 16 * 8 * ( header.m_dayTimeCount - DAYTIMES ) );
                }

                _blockSets[ i ] = bInfo;
                _blockSetNames.insert( i );
            }

            message_log( "readTileSets", "Read " + std::to_string( header.m_blockSetCount )
                                             + " TS from combined tile set." );
            fclose( f );
        } else {
            if( !checkOrCreatePath( TILESET_PATH ) ) { return true; }
            if( !checkOrCreatePath( BLOCKSET_PATH ) ) { return true; }
            if( !checkOrCreatePath( PALETTE_PATH ) ) { return true; }
            _tileSetMode = DATA::TILEMODE_DEFAULT;
            _tseTileModeToggles[ 0 ]->set_active( );
            std::error_code ec;

            for( auto& p : fs::directory_iterator( BLOCKSET_PATH ) ) {
                if( !p.is_regular_file( ec ) || ec ) { continue; }

                u8 bsname;
                if( 1
                    != sscanf( p.path( ).filename( ).c_str( ), BLOCKSET_FORMAT.c_str( ),
                               &bsname ) ) {
                    continue;
                }

                auto res = blockSetInfo( );
                f        = fopen( p.path( ).c_str( ), "r" );
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
                    // continue;
                } else {
                    fclose( f );
                }

                // read corresponding palettes
                auto palpath = fs::path( PALETTE_PATH ) / ( std::to_string( bsname ) + ".p2l" );
                f            = fopen( palpath.c_str( ), "r" );
                if( !DATA::readPal( f, res.m_pals, 8 * 5 ) ) {
                    fprintf( stderr, "[LOG] Reading palette %hu failed.\n", bsname );
                    // continue;
                } else {
                    fclose( f );
                }

                // fprintf( stderr, "[LOG] Loaded blockset %hhu.\n", bsname );

                _blockSets[ bsname ] = res;
                _blockSetNames.insert( bsname );
            }
        }

        _disableRedraw = true;
        auto bsnames   = std::vector<Glib::ustring>( );

        for( auto bsname : _blockSetNames ) {
            bsnames.push_back( std::to_string( bsname ) );
            _blockSets[ bsname ].m_stringListItem = bsnames.size( ) - 1;
        }
        _mapBankStrList->splice( 0, _mapBankStrList->get_n_items( ), bsnames );
        _disableRedraw = false;

        return error;
    }

    bool root::writeTileSets( ) {
        if( _blockSetNames.empty( ) ) { return true; }
        bool error = false;

        switch( _tileSetMode ) {
        default: {
        case DATA::TILEMODE_DEFAULT:
            // store everything in separate files
            if( !checkOrCreatePath( TILESET_PATH ) ) { return true; }
            if( !checkOrCreatePath( BLOCKSET_PATH ) ) { return true; }
            if( !checkOrCreatePath( PALETTE_PATH ) ) { return true; }
            for( const auto& [ name, bInfo ] : _blockSets ) {
                FILE* f;

                auto tspath = fs::path( TILESET_PATH ) / ( std::to_string( name ) + ".ts" );
                f           = fopen( tspath.c_str( ), "wb" );
                error |= !DATA::writeTiles( f, bInfo.m_tileSet.m_tiles );
                fclose( f );

                auto blpath = fs::path( BLOCKSET_PATH ) / ( std::to_string( name ) + ".bvd" );
                f           = fopen( blpath.c_str( ), "wb" );
                error |= !DATA::writeBlocks( f, bInfo.m_blockSet.m_blocks );
                fclose( f );

                auto palpath = fs::path( PALETTE_PATH ) / ( std::to_string( name ) + ".p2l" );
                f            = fopen( palpath.c_str( ), "wb" );
                error |= !DATA::writePal( f, bInfo.m_pals, 8 * DAYTIMES );
                fclose( f );
            }
            break;
        }
        case DATA::TILEMODE_COMBINED: {
            // store everything in a single file
            FILE* f;
            auto  pth = fs::path( MAP_PATH ) / "tileset.tsb";
            f         = fopen( pth.c_str( ), "wb" );

            // compute header
            auto header            = DATA::blockSetBankHeader( );
            header.m_blockSetCount = *_blockSetNames.crbegin( ) + 1;
            header.m_dayTimeCount  = DAYTIMES;

            fwrite( &header, sizeof( DATA::blockSetBankHeader ), 1, f );

            for( auto i = 0; i < header.m_blockSetCount; ++i ) {
                if( _blockSets.count( i ) ) {
                    const auto& bInfo = _blockSets[ i ];
                    error |= !DATA::writeTiles( f, bInfo.m_tileSet.m_tiles );
                    error |= !DATA::writeBlocks( f, bInfo.m_blockSet.m_blocks );
                    error |= !DATA::writePal( f, bInfo.m_pals, 8 * header.m_dayTimeCount );
                } else {
                    // write dummy values
                    // TODO: directly write dummy values?
                    auto bInfo = blockSetInfo( );
                    error |= !DATA::writeTiles( f, bInfo.m_tileSet.m_tiles );
                    error |= !DATA::writeBlocks( f, bInfo.m_blockSet.m_blocks );
                    error |= !DATA::writePal( f, bInfo.m_pals, 8 * header.m_dayTimeCount );
                }
            }
            fclose( f );
            break;
        }
        }

        if( !error ) { _editTileSet->setStatus( mapBank::STATUS_SAVED ); }

        return error;
    }

    bool root::writeMapBank( u16 p_bank ) {
        if( !_mapBanks.count( p_bank ) ) { return true; }
        auto& info = _mapBanks[ p_bank ];

        message_log( "writeMapBank", "Saving map bank " + std::to_string( p_bank ) + ".",
                     LOGLEVEL_STATUS );
        bool            error = false;
        std::error_code ec;

        auto path = fs::path( MAP_PATH ) / fs::path( std::to_string( p_bank ) + ".bank" );
        fs::create_directories( path.parent_path( ) );
        FILE* f = fopen( path.c_str( ), "wb" );

        error = !DATA::writeMapBank( f, &info.m_info, info.m_bank.get( ) );

        if( !info.isCombined( ) ) {
            for( u8 y = 0; y <= info.getSizeY( ); ++y ) {
                for( u8 x = 0; x <= info.getSizeX( ); ++x ) {
                    error |= writeMapSlice( p_bank, x, y );
                    if( info.isScattered( ) ) {
                        // delete old map files

                        auto p = fs::path( MAP_PATH ) / fs::path( std::to_string( p_bank ) )
                                 / ( std::to_string( y ) + "_" + std::to_string( x ) + ".map" );
                        fs::remove( p, ec );
                        if( ec ) {
                            fprintf( stderr, "[ERROR] Could not delete old map file: %s\n",
                                     ec.message( ).c_str( ) );
                        }
                        p += ".data";
                        fs::remove( p, ec );
                        if( ec ) {
                            fprintf( stderr, "[ERROR] Could not delete old map data file: %s\n",
                                     ec.message( ).c_str( ) );
                        }
                    }
                }
                if( !info.isScattered( ) ) {
                    // delete any folder created when the map bank was scattered
                    auto p = fs::path( MAP_PATH ) / fs::path( std::to_string( p_bank ) )
                             / fs::path( std::to_string( y ) );
                    fs::remove_all( p, ec );
                    if( ec ) {
                        fprintf( stderr, "[ERROR] Could not delete old map bank folder: %s\n",
                                 ec.message( ).c_str( ) );
                    }
                }
            }
        } else {
            // clean up the directory
            auto p = fs::path( MAP_PATH ) / fs::path( std::to_string( p_bank ) );
            fs::remove_all( p, ec );
            if( ec ) {
                fprintf( stderr, "[ERROR] Could not delete old map bank folder: %s\n",
                         ec.message( ).c_str( ) );
            }
        }
        if( !error ) { info.m_widget->setStatus( mapBank::STATUS_SAVED ); }

        fclose( f );
        return error;
    }

    void root::onFsRootSaveClick( ) {
        // Only write map banks that have been changed

        for( const auto& [ bank, info ] : _mapBanks ) {
            if( info.m_widget != nullptr && info.m_bank != nullptr
                && ( info.m_widget->getStatus( ) == mapBank::STATUS_NEW
                     || info.m_widget->getStatus( ) == mapBank::STATUS_EDITED_UNSAVED ) ) {
                // something changed here, save the maps

                writeMapBank( bank );
            }
        }

        if( _editTileSet && _editTileSet->getStatus( ) == mapBank::STATUS_EDITED_UNSAVED ) {
            writeTileSets( );
        }
    }

    void root::onFolderDialogResponse( int p_responseId, Gtk::FileChooserDialog* p_dialog ) {
        if( p_dialog == nullptr ) {
            message_log( "onFolderDialogResponse", "p_dialog is nullptr.", LOGLEVEL_DEBUG );
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
        for( u8 i = 0; i < DATA::MAX_MOVEMENTS; ++i ) {
            if( DATA::mapBlockAtom::MOVEMENT_ORDER[ i ] == p_block.m_movedata ) {
                _movementWidget.selectBlock( i );
                break;
            }
        }
        if( _currentlySelectedBlock.m_blockidx < DATA::MAX_BLOCKS_PER_TILE_SET ) {
            _ts1widget.selectBlock( _currentlySelectedBlock.m_blockidx );
            _ts2widget.selectBlock( -1 );
        } else {
            _ts1widget.selectBlock( -1 );
            _ts2widget.selectBlock( _currentlySelectedBlock.m_blockidx
                                    - DATA::MAX_BLOCKS_PER_TILE_SET );
        }
    }

    void root::onMapDragStart( UI::mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                               s8 p_mapX, s8 p_mapY, bool ) {
        _dragStart = { p_blockX, p_blockY, p_mapX, p_mapY };
        _dragLast  = { p_blockX, p_blockY };
        if( p_button == mapSlice::clickType::RIGHT ) {
            // reset blockStamp
            if( _blockStampDialog && !_blockStampDialogInvalid ) { _blockStampDialog->hide( ); }
            _blockStampDialogInvalid = true;
        }
    }

    void root::onMapDragUpdate( UI::mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                                s8 p_mapY, bool p_allowEdit ) {

        auto [ sx, sy, _1, _2 ] = _dragStart;
        auto [ lx, ly ]         = _dragLast;
        (void) _1;
        (void) _2;

        auto blockwd = _blockScale * DATA::BLOCK_SIZE + _blockSpacing;
        auto nx      = sx + ( p_dX / blockwd );
        auto ny      = sy + ( p_dY / blockwd );

        if( nx == lx && ny == ly ) { return; }
        _dragLast = { nx, ny };

        //        fprintf( stderr, "DragUpdate butto %hu bx: %hu by: %hu, %hhi %hhi %hhu\n",
        //        p_button, nx, ny,
        //                 p_mapX, p_mapY, p_allowEdit );

        if( p_allowEdit && p_button == mapSlice::clickType::LEFT
            && isInMapBounds( nx, ny, p_mapX, p_mapY ) ) {
            onMapClicked( p_button, nx, ny, p_mapX, p_mapY, p_allowEdit );
        }
        if( p_button == mapSlice::clickType::RIGHT ) {
            if( _currentMapDisplayMode == MODE_EDIT_TILES
                || _currentMapDisplayMode == MODE_EDIT_MOVEMENT ) {

                // update and show the stamp box

                if( _blockStampDialogInvalid ) { // Block stamp dialog
                    _blockStampDialog
                        = std::make_shared<Gtk::Dialog>( "Block Stamp", *this, false, true );
                    _blockStampDialog->get_content_area( )->append( _blockStampMap );
                    _blockStampDialog->signal_close_request( ).connect(
                        [ this ]( ) -> bool {
                            _blockStampDialogInvalid = true;
                            return false;
                        },
                        false );
                    _blockStampDialogInvalid = false;
                }

                // Check if the stamp should go across map borders
                if( p_mapX == 1 ) {
                    if( nx < -DATA::SIZE - _adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( nx < -DATA::SIZE ) {
                        // crossing two borders
                        nx = sx + ( p_dX + 2 * _neighborSpacing + 2 * _blockSpacing ) / blockwd - 1;
                    } else if( nx < 0 ) {
                        // crossing one border
                        nx = sx + ( p_dX + _neighborSpacing + _blockSpacing ) / blockwd - 1;
                    } else if( nx >= _adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else if( p_mapX == 0 ) {
                    if( nx < -_adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( nx < 0 ) {
                        // crossing one border
                        nx = sx + ( p_dX + _neighborSpacing + _blockSpacing ) / blockwd - 1;
                    } else if( nx >= DATA::SIZE ) {
                        // crossing one border
                        nx = sx + ( p_dX - _neighborSpacing + _blockSpacing ) / blockwd;
                    } else if( nx >= DATA::SIZE + _adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else {
                    if( nx < 0 ) {
                        // out of bounds
                        return;
                    } else if( nx >= _adjacentBlocks ) {
                        // crossing one border
                        nx = sx + ( p_dX - _neighborSpacing + _blockSpacing ) / blockwd;
                    } else if( nx >= DATA::SIZE + _adjacentBlocks ) {
                        // crossing two borders
                        nx = sx + ( p_dX - 2 * _neighborSpacing + 2 * _blockSpacing ) / blockwd;
                    } else if( nx >= DATA::SIZE + 2 * _adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                }
                if( p_mapY == 1 ) {
                    if( ny < -DATA::SIZE - _adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( ny < -DATA::SIZE ) {
                        // crossing two borders
                        ny = sy + ( p_dY + 2 * _neighborSpacing - 2 * _blockSpacing ) / blockwd - 1;
                    } else if( ny < 0 ) {
                        // crossing one border
                        ny = sy + ( p_dY + _neighborSpacing - _blockSpacing ) / blockwd - 1;
                    } else if( ny >= _adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else if( p_mapY == 0 ) {
                    if( ny < -_adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( ny < 0 ) {
                        // crossing one border
                        ny = sy + ( p_dY + _neighborSpacing + _blockSpacing ) / blockwd - 1;
                    } else if( ny >= DATA::SIZE ) {
                        // crossing one border
                        ny = sy + ( p_dY - _neighborSpacing + _blockSpacing ) / blockwd;
                    } else if( ny >= DATA::SIZE + _adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else {
                    if( ny < 0 ) {
                        // out of bounds
                        return;
                    } else if( ny >= _adjacentBlocks ) {
                        // crossing one border
                        ny = sy + ( p_dY - _neighborSpacing + _blockSpacing ) / blockwd;
                    } else if( ny >= DATA::SIZE + _adjacentBlocks ) {
                        // crossing two borders
                        ny = sy + ( p_dY - 2 * _neighborSpacing + 2 * _blockSpacing ) / blockwd;
                    } else if( ny >= DATA::SIZE + 2 * _adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                }

                _dragLast = { nx, ny };
                if( nx == lx && ny == ly ) { return; }

                bool revx = nx < sx;
                bool revy = ny < sy;
                s16  posx = sx, posy = sy;

                auto tmpMap = std::deque<std::deque<DATA::mapBlockAtom>>( );

                while( 1 ) {
                    if( revy && posy < ny ) { break; }
                    if( !revy && posy > ny ) { break; }

                    // build a new row
                    auto row         = std::deque<DATA::mapBlockAtom>( );
                    _blockStampWidth = 0;
                    posx             = sx;
                    while( 1 ) {
                        if( revx && posx < nx ) { break; }
                        if( !revx && posx > nx ) { break; }

                        s8  mx = p_mapX, my = p_mapY;
                        s16 remx = posx, remy = posy;

                        // compute block position

                        while( mx >= -1 && mx <= 1 && !isInMapBounds( remx, 0, mx, my ) ) {
                            if( mx == 1 ) {
                                if( remx < 0 ) {
                                    remx += DATA::SIZE;
                                    mx--;
                                } else [[unlikely]] {
                                    break;
                                }
                            } else if( mx == 0 ) {
                                if( remx < 0 ) {
                                    remx += _adjacentBlocks;
                                    mx--;
                                } else {
                                    remx -= DATA::SIZE;
                                    mx++;
                                }
                            } else {
                                if( remx < 0 ) [[unlikely]] {
                                    break;
                                } else {
                                    remx -= _adjacentBlocks;
                                    mx++;
                                }
                            }
                        }
                        while( my >= -1 && my <= 1 && !isInMapBounds( 0, remy, mx, my ) ) {
                            if( my == 1 ) {
                                if( remy < 0 ) {
                                    remy += DATA::SIZE;
                                    my--;
                                } else [[unlikely]] {
                                    break;
                                }
                            } else if( my == 0 ) {
                                if( remy < 0 ) {
                                    remy += _adjacentBlocks;
                                    my--;
                                } else {
                                    remy -= DATA::SIZE;
                                    my++;
                                }
                            } else {
                                if( remy < 0 ) [[unlikely]] {
                                    break;
                                } else {
                                    remy -= _adjacentBlocks;
                                    my++;
                                }
                            }
                        }

                        // compute block

                        u16 xcorr = 0, ycorr = 0;
                        if( mx < 0 ) { xcorr = DATA::SIZE - _adjacentBlocks; }
                        if( my < 0 ) { ycorr = DATA::SIZE - _adjacentBlocks; }

                        DATA::mapBlockAtom currentBlock;
                        if( _selectedMapY + mx < 0 || _selectedMapX + my < 0
                            || _selectedMapX + mx > _mapBanks[ _selectedBank ].getSizeX( )
                            || _selectedMapY + my > _mapBanks[ _selectedBank ].getSizeY( ) ) {
                            // out of map bank bounds, add a blank block
                            currentBlock = { 0, 1 };
                        } else {
                            auto& mp
                                = _mapBanks[ _selectedBank ]
                                      .m_bank->m_slices[ _selectedMapY + my ][ _selectedMapX + mx ];
                            currentBlock = mp.m_data.m_blocks[ remy + ycorr ][ remx + xcorr ];
                        }

                        if( revx ) {
                            row.push_front( currentBlock );
                            posx--;
                        } else {
                            row.push_back( currentBlock );
                            posx++;
                        }
                        _blockStampWidth++;
                    }

                    if( revy ) {
                        tmpMap.push_front( row );
                        posy--;
                    } else {
                        tmpMap.push_back( row );
                        posy++;
                    }
                }

                _blockStampData.clear( );
                for( auto row : tmpMap ) {
                    for( auto block : row ) { _blockStampData.push_back( block ); }
                }

                _blockStampMap.set(
                    _blockStampData,
                    [ this ]( DATA::mapBlockAtom p_block ) {
                        return blockSetLookup( p_block.m_blockidx );
                    },
                    _blockStampWidth );
                _blockStampMap.draw( );
                _blockStampMap.setOverlayHidden( _currentMapDisplayMode != MODE_EDIT_MOVEMENT );
                _blockStampDialog->show( );
            }
        }
    }

    void root::onMapDragEnd( UI::mapSlice::clickType /* p_button */, s16 /* p_dX */, s16 /* p_dY */,
                             s8 /* p_mapX */, s8 /* p_mapY */, bool /* p_allowEdit */ ) {
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
            || _selectedMapX + p_mapX > _mapBanks[ _selectedBank ].getSizeX( )
            || _selectedMapY + p_mapY > _mapBanks[ _selectedBank ].getSizeY( ) ) {
            if( p_button == mapSlice::clickType::RIGHT ) { updateSelectedBlock( { 0, 1 } ); }
            return;
        }

        auto& mp = _mapBanks[ _selectedBank ]
                       .m_bank->m_slices[ _selectedMapY + p_mapY ][ _selectedMapX + p_mapX ];
        auto& block = mp.m_data.m_blocks[ p_blockY + ycorr ][ p_blockX + xcorr ];

        switch( p_button ) {
        case mapSlice::clickType::LEFT:
            if( p_allowEdit ) {
                if( _currentMapDisplayMode == MODE_EDIT_TILES ) {
                    // check if there is a valid block stamp
                    if( !_blockStampDialogInvalid ) {
                        // paste the block stamp
                        auto bx = p_blockX + xcorr, by = p_blockY + ycorr, pos = 0;
                        for( size_t y = 0; y < _blockStampData.size( ) / _blockStampWidth; ++y ) {
                            for( size_t x = 0; x < _blockStampWidth; ++x, ++pos ) {
                                if( bx + x < DATA::SIZE && by + y < DATA::SIZE ) {
                                    mp.m_data.m_blocks[ by + y ][ bx + x ] = _blockStampData[ pos ];
                                    _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock(
                                        _blockStampData[ pos ], bx + x, by + y );
                                    _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                                        _blockStampData[ pos ].m_movedata, bx + x, by + y );
                                }
                            }
                        }
                    } else {
                        block.m_blockidx = _currentlySelectedBlock.m_blockidx;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock(
                            _currentlySelectedBlock, p_blockX, p_blockY );
                    }
                } else if( _currentMapDisplayMode == MODE_EDIT_MOVEMENT ) {
                    if( !_blockStampDialogInvalid ) {
                        // paste the block stamp
                        auto bx = p_blockX + xcorr, by = p_blockY + ycorr, pos = 0;
                        for( size_t y = 0; y < _blockStampData.size( ) / _blockStampWidth; ++y ) {
                            for( size_t x = 0; x < _blockStampWidth; ++x, ++pos ) {
                                if( bx + x < DATA::SIZE && by + y < DATA::SIZE ) {
                                    mp.m_data.m_blocks[ by + y ][ bx + x ].m_movedata
                                        = _blockStampData[ pos ].m_movedata;
                                    _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                                        _blockStampData[ pos ].m_movedata, bx + x, by + y );
                                }
                            }
                        }
                    } else {
                        block.m_movedata = _currentlySelectedBlock.m_movedata;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                            block.m_movedata, p_blockX, p_blockY );
                    }
                }
                markBankChanged( _selectedBank );
            } else {
                updateSelectedBlock( block );
            }
            break;
        case mapSlice::clickType::RIGHT:
            if( _currentMapDisplayMode == MODE_EDIT_TILES
                || _currentMapDisplayMode == MODE_EDIT_MOVEMENT ) {
                updateSelectedBlock( block );
            }
            break;
        case mapSlice::clickType::MIDDLE:
            if( p_allowEdit ) {
                DATA::mapBlockAtom oldb = block;
                // flood fill
                auto bqueue = std::queue<std::pair<s16, s16>>( );
                bqueue.push( { p_blockY, p_blockX } );
                while( !bqueue.empty( ) ) {
                    auto [ cy, cx ] = bqueue.front( );
                    bqueue.pop( );

                    if( cx < 0 || cx >= DATA::SIZE || cy < 0 || cy >= DATA::SIZE ) { continue; }

                    if( _currentMapDisplayMode == MODE_EDIT_TILES ) {
                        if( mp.m_data.m_blocks[ cy ][ cx ].m_blockidx != oldb.m_blockidx ) {
                            continue;
                        }
                        mp.m_data.m_blocks[ cy ][ cx ].m_blockidx
                            = _currentlySelectedBlock.m_blockidx;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock(
                            _currentlySelectedBlock, p_blockX, p_blockY );
                    } else if( _currentMapDisplayMode == MODE_EDIT_MOVEMENT ) {
                        if( mp.m_data.m_blocks[ cy ][ cx ].m_movedata != oldb.m_movedata ) {
                            continue;
                        }
                        mp.m_data.m_blocks[ cy ][ cx ].m_movedata
                            = _currentlySelectedBlock.m_movedata;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                            mp.m_data.m_blocks[ cy ][ cx ].m_movedata, p_blockX, p_blockY );
                    }

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

    void root::onTSClicked( UI::mapSlice::clickType, u16 p_blockX, u16 p_blockY, u8 p_ts ) {
        u16 block = p_blockY * _blockSetWidth + p_blockX;
        updateSelectedBlock( { u16( block + p_ts * DATA::MAX_BLOCKS_PER_TILE_SET ), 0 } );
    }

    void root::onTSETSClicked( UI::mapSlice::clickType, u16 p_blockX, u16 p_blockY, u8 p_ts ) {
        u16 block = p_blockY * _blockSetWidth + p_blockX;

        if( p_ts == 0 ) {
            _tsets1widget.selectBlock( block );
            _tsets2widget.selectBlock( -1 );
            _editBlock->setBlock( _currentBlockset1[ block ].first );
        } else {
            _tsets1widget.selectBlock( -1 );
            _tsets2widget.selectBlock( block );
            _editBlock->setBlock( _currentBlockset2[ block ].first );
        }

        _tseSelectedBlockIdx = block + p_ts * DATA::MAX_BLOCKS_PER_TILE_SET;
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

    DATA::mapBankInfo root::exploreMapBank( const fs::path& p_path ) {
        std::error_code ec;

        u8 sx = 0, sy = 0;
        u8 mapMode = 0;
        for( auto& p : fs::directory_iterator( p_path, ec ) ) {
            if( !p.is_directory( ec ) || ec ) { continue; }
            // check if the directory has a number as name
            std::string name         = p.path( ).filename( );
            int         nameAsNumber = -1;
            try {
                nameAsNumber = std::stoi( name );
            } catch( ... ) { continue; }

            if( nameAsNumber >= 0 && nameAsNumber <= int( MAX_MAPY ) ) {
                mapMode = 1;
                sy      = std::max( u8( nameAsNumber ), sy );
            }
        }
        if( mapMode == 1 ) {
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

        } else if( mapMode == 0 ) {
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

        return { sx, sy, mapMode };
    }

    void root::setTitle( const std::string& p_windowTitle, const std::string& p_mainTitle,
                         const std::string& p_subTitle ) {
        if( p_windowTitle != "" ) {
            set_title( p_windowTitle );
        } else {
            set_title( TITLE_STRING );
        }

        if( p_mainTitle == "" ) {
            if( p_subTitle != "" ) {
                _titleLabel.set_text( p_subTitle );
            } else {
                _titleLabel.set_text( get_title( ) );
            }
            _subtitleLabel.hide( );
        } else {
            _titleLabel.set_text( p_mainTitle );
            if( p_subTitle != "" ) {
                _subtitleLabel.set_text( p_subTitle );
                _subtitleLabel.show( );
            } else {
                _subtitleLabel.hide( );
            }
        }
    }

    void root::loadNewFsRoot( const std::string& p_path ) {
        if( !checkOrCreatePath( p_path ) ) { return; }
        if( !checkOrCreatePath( p_path + "/MAPS/" ) ) { return; }
        FSROOT_PATH   = p_path;
        MAP_PATH      = FSROOT_PATH + "/MAPS/";
        TILESET_PATH  = FSROOT_PATH + "/MAPS/TILESETS/";
        BLOCKSET_PATH = FSROOT_PATH + "/MAPS/BLOCKSETS/";
        PALETTE_PATH  = FSROOT_PATH + "/MAPS/PALETTES/";
        MAPDATA_PATH  = FSROOT_PATH + "/DATA/MAP_DATA/";
        _fsRootLoaded = true;

        // update map banks
        _mapBanks.clear( );
        _blockSets.clear( );
        _blockSetNames.clear( );
        _selectedBank = -1;

        switchContext( FSROOT_NONE );

        // traverse MAP_PATH and search for all dirs that are named with a number
        // (assuming they are a map bank)

        std::error_code ec;
        for( auto& p : fs::directory_iterator( MAP_PATH, ec ) ) {
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
                addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_mapMode );
            } else if( p.is_regular_file( ec ) && !ec ) {
                // try to open the file and read the map bank information from it
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

                DATA::mapBankInfo info;
                FILE*             f = fopen( p.path( ).c_str( ), "rb" );
                if( !f ) {
                    fprintf( stderr,
                             "[LOG] Skipping potential map bank %s: "
                             "name (as number) too large.\n",
                             name.c_str( ) );
                    continue;
                }

                fread( &info, sizeof( DATA::mapBankInfo ), 1, f );
                fclose( f );

                addNewMapBank( u16( nameAsNumber ), info.m_sizeY, info.m_sizeX, info.m_mapMode );
            }
        }

        // load all tilesets and blocksets
        if( readTileSets( ) ) { return; }

        addFsRootToRecent( p_path );
    }

    void root::addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, u8 p_mapMode,
                              mapBank::status p_status ) {
        if( _mapBanks.count( p_bank ) ) { return; }
        // if( !checkOrCreatePath( fs::path( MAP_PATH ) / std::to_string( p_bank ) ) ) {
        //     fprintf( stderr, "[ERROR] Adding map bank %hu failed.\n", p_bank );
        //     return;
        // }

        /*
        fprintf( stderr, "[LOG] Adding map bank %hu with initial size %hhu rows, %hhu cols.\n",
                 p_bank, p_sizeY + 1, p_sizeX + 1 );
                 */

        auto MB1 = std::make_shared<mapBank>( p_bank, p_sizeX, p_sizeY, p_status );
        MB1->connect( [ this ]( u16 p_bk, u8 p_y, u8 p_x ) {
            if( _selectedBank != -1 ) {
                onUnloadMap( _selectedBank, _selectedMapY, _selectedMapX );
            }
            loadMap( p_bk, p_y, p_x );
        } );
        _mapBanks[ p_bank ]
            = { MB1, nullptr, nullptr, false, DATA::mapBankInfo( p_sizeX, p_sizeY, p_mapMode ) };

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
            message_log( "createMapBank", "No FSROOT loaded. Won't create a new map bank." );
            return;
        }
        if( _mapBanks.count( p_bank ) ) {
            // map exists, do nothing
            fprintf( stderr, "[LOG] Map bank %hu already exists.\n", p_bank );
            return;
        }

        addNewMapBank( p_bank, p_sizeY, p_sizeX, DATA::MAPMODE_COMBINED, mapBank::STATUS_NEW );
        if( _selectedBank != -1 ) { onUnloadMap( _selectedBank, _selectedMapY, _selectedMapX ); }
        loadMap( p_bank, 0, 0 );
    }

    void root::createBlockSet( u8 p_tsIdx ) {
        if( _blockSets.count( p_tsIdx ) ) { return; }

        _blockSetNames.insert( p_tsIdx );
        _blockSets[ p_tsIdx ] = blockSetInfo( );

        auto bsnames = std::vector<Glib::ustring>( );

        for( auto bsname : _blockSetNames ) {
            bsnames.push_back( std::to_string( bsname ) );
            _blockSets[ bsname ].m_stringListItem = bsnames.size( ) - 1;
        }
        _mapBankStrList->splice( 0, _mapBankStrList->get_n_items( ), bsnames );

        markTileSetsChanged( );
    }

    void root::editTileSets( u8 p_ts1, u8 p_ts2 ) {
        if( !_fsRootLoaded ) {
            message_log( "editTileSets", "No FSROOT loaded. Won't load any tile sets." );
            return;
        }

        message_log( "editTileSets",
                     "Loading tile set editor for " + std::to_string( p_ts1 ) + "|"
                         + std::to_string( p_ts2 ) + ".",
                     LOGLEVEL_STATUS );

        if( _selectedBank != -1 ) {
            _mapBanks[ _selectedBank ].m_widget->unselect( );
            onUnloadMap( _selectedBank, _selectedMapY, _selectedMapX );
        }
        _selectedBank = -1;
        switchContext( TILE_EDITOR );

        setTitle( "", "Tile Set " + std::to_string( p_ts1 ) + "|" + std::to_string( p_ts2 ),
                  FSROOT_PATH );

        // check if block sets exist

        if( !_blockSetNames.count( p_ts1 ) ) { createBlockSet( p_ts1 ); }
        if( !_blockSetNames.count( p_ts2 ) ) { createBlockSet( p_ts2 ); }

        // compute block set

        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts, p_ts1, p_ts2 );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        buildPalette( pals, p_ts1, p_ts2 );

        _currentBlockset1
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ p_ts1 ].m_blockSet, &ts );
        _currentBlockset2
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ p_ts2 ].m_blockSet, &ts );

        _tsets1widget.setScale( 2 * _blockScale );
        _tsets2widget.setScale( 2 * _blockScale );
        _tsets1widget.set( _currentBlockset1, pals, _blockSetWidth );
        _tsets1widget.set( _currentBlockset1, pals, _blockSetWidth );
        _tsets2widget.set( _currentBlockset2, pals, _blockSetWidth );
        _tsets1widget.setDaytime( _currentDayTime );
        _tsets2widget.setDaytime( _currentDayTime );
        _tsets1widget.draw( );
        _tsets2widget.draw( );
        _sbTileSetSel1 = p_ts1;
        _sbTileSetSel2 = p_ts2;
        _editBlock->redraw( pals, _currentDayTime );
    }

    void root::collapseMapBankBar( bool p_collapse ) {
        for( auto& i : _mapBanks ) {
            if( i.second.m_widget ) { i.second.m_widget->collapse( p_collapse ); }
        }
        _addMapBank->collapse( p_collapse );
        _editTileSet->collapse( p_collapse );
        if( p_collapse ) {
            auto icon = Gtk::Image( );
            icon.set_from_icon_name( "view-fullscreen-symbolic" );
            _collapseMapBanksButton->set_child( icon );
            _mapBankBarLabel.hide( );
            _sbTileSetBarLabel.hide( );
        } else {
            auto icon = Gtk::Image( );
            icon.set_from_icon_name( "view-restore-symbolic" );
            auto label = Gtk::Label( "_Collapse Sidebar" );
            label.set_expand( true );
            label.set_use_underline( );

            auto hbox = Gtk::Box( Gtk::Orientation::HORIZONTAL, 5 );
            hbox.append( icon );
            hbox.append( label );
            _collapseMapBanksButton->set_child( hbox );
            _mapBankBarLabel.show( );
            _sbTileSetBarLabel.show( );
        }
        _mapBankBarCollapsed = p_collapse;
    }

    bool root::readMapSlice( u16 p_bank, u8 p_mapX, u8 p_mapY, std::string p_path,
                             bool p_readMapData ) {
        if( !_mapBanks.count( p_bank ) ) { return false; }
        auto& selb = _mapBanks[ p_bank ];
        auto  sl   = DATA::mapSlice( );
        auto  dt   = DATA::mapData( );

        auto path = fs::path( MAP_PATH );

        if( !selb.isCombined( ) || p_path != "" ) {
            path /= std::to_string( p_bank );
            if( selb.isScattered( ) ) { path /= std::to_string( p_mapY ); }
            path /= ( std::to_string( p_mapY ) + "_" + std::to_string( p_mapX ) + ".map" );

            if( p_path != "" ) { path = p_path; }

            FILE* f = fopen( path.c_str( ), "rb" );

            if( !DATA::readMapSlice( f, &sl, p_mapX, p_mapY ) ) {
                fprintf( stderr, "[LOG] Loading map %hu/%hhu_%hhu.map failed. (path %s)\n", p_bank,
                         p_mapY, p_mapX, path.c_str( ) );
            } else {
                selb.m_bank->m_slices[ p_mapY ][ p_mapX ] = sl;
            }

            if( p_readMapData ) {
                path += ".data";
                f = fopen( path.c_str( ), "rb" );
                if( !DATA::readMapData( f, &dt ) ) {
                    fprintf( stderr,
                             "[LOG] Loading map data %hu/%hhu_%hhu.map.data failed. (path %s)\n",
                             p_bank, p_mapY, p_mapX, path.c_str( ) );
                } else {
                    selb.m_bank->m_mapData[ p_mapY ][ p_mapX ] = dt;
                }
            }
        } else {
            path /= ( std::to_string( p_bank ) + ".bank" );

            // Load map bank, FSEEK to correct position
            FILE* f = fopen( path.c_str( ), "rb" );

            if( !DATA::readMapSliceAndData( f, &sl, &dt, p_mapX, p_mapY ) ) {
                fprintf(
                    stderr,
                    "[LOG] Loading map and map data %hhu_%hhu from bank %hu.bank failed. (path %s)\n",
                    p_mapY, p_mapX, p_bank, path.c_str( ) );

            } else {
                selb.m_bank->m_slices[ p_mapY ][ p_mapX ] = sl;
                if( p_readMapData ) { selb.m_bank->m_mapData[ p_mapY ][ p_mapX ] = dt; }
            }
        }
        return true;
    }

    bool root::readMapBank( u16 p_bank, bool p_forceReread ) {
        if( !_mapBanks.count( p_bank ) ) { return false; }

        auto& selb = _mapBanks[ p_bank ];
        fprintf( stderr, "[LOG] Loading map bank %hu.\n", p_bank );

        // Load all maps of the bank into mem
        if( !selb.m_loaded || p_forceReread ) {
            selb.m_bank = std::make_unique<DATA::mapBank>( );

            // Load the *bank file
            auto  path = fs::path( MAP_PATH ) / fs::path( std::to_string( p_bank ) + ".bank" );
            FILE* f    = fopen( path.c_str( ), "rb" );

            if( !DATA::readMapBank( f, &selb.m_info, selb.m_bank.get( ) ) ) {
                fprintf(
                    stderr,
                    "[LOG] Map bank file %hu.bank does not exist (path %s). Trying on my own.\n",
                    p_bank, path.c_str( ) );
                // If the file does not exist, selb.m_info should already exist and
                // contain useful information, so we are good
            }
            if( f ) { fclose( f ); }

            if( !selb.isCombined( ) ) {
                // not a combined map bank, we need to read the data ourselves
                selb.m_bank->m_slices = std::vector<std::vector<DATA::mapSlice>>(
                    selb.getSizeY( ) + 1,
                    std::vector<DATA::mapSlice>( selb.getSizeX( ) + 1, DATA::mapSlice( ) ) );
                selb.m_bank->m_mapData = std::vector<std::vector<DATA::mapData>>(
                    selb.getSizeY( ) + 1,
                    std::vector<DATA::mapData>( selb.getSizeX( ) + 1, DATA::mapData( ) ) );
                for( u16 y = 0; y <= selb.getSizeY( ); ++y ) {
                    for( u16 x = 0; x <= selb.getSizeX( ); ++x ) { readMapSlice( p_bank, x, y ); }
                }
            }

            selb.m_computedBank
                = std::make_unique<std::vector<std::vector<DATA::computedMapSlice>>>(
                    selb.getSizeY( ) + 1, std::vector<DATA::computedMapSlice>(
                                              selb.getSizeX( ) + 1, DATA::computedMapSlice( ) ) );
            for( u16 y = 0; y <= selb.getSizeY( ); ++y ) {
                for( u16 x = 0; x <= selb.getSizeX( ); ++x ) {
                    auto bs = DATA::blockSet<2>( );
                    buildBlockSet( &bs, selb.m_bank->m_slices[ y ][ x ].m_data.m_tIdx1,
                                   selb.m_bank->m_slices[ y ][ x ].m_data.m_tIdx2 );
                    auto ts = DATA::tileSet<2>( );
                    buildTileSet( &ts, selb.m_bank->m_slices[ y ][ x ].m_data.m_tIdx1,
                                  selb.m_bank->m_slices[ y ][ x ].m_data.m_tIdx2 );

                    buildPalette( ( *selb.m_computedBank )[ y ][ x ].m_pals,
                                  selb.m_bank->m_slices[ y ][ x ].m_data.m_tIdx1,
                                  selb.m_bank->m_slices[ y ][ x ].m_data.m_tIdx2 );
                    ( *selb.m_computedBank )[ y ][ x ].m_computedBlocks
                        = selb.m_bank->m_slices[ y ][ x ].compute( &bs, &ts );
                }
            }

            selb.m_loaded = true;
        }
        return true;
    }

    void root::loadMapBank( u16 p_bank ) {
        if( p_bank == _selectedBank ) { return; }

        if( _selectedBank != -1 ) { _mapBanks[ _selectedBank ].m_widget->unselect( ); }
        _selectedBank = p_bank;

        if( _mapBanks.count( _selectedBank ) ) {
            auto& selb = _mapBanks[ _selectedBank ];
            selb.m_widget->select( );
            readMapBank( p_bank, false );

            if( selb.m_computedBank != nullptr ) {
                _mapBankOverview.set( *selb.m_computedBank );
                _mapBankOverview.redraw( _currentDayTime );
            }

            for( auto& tb : _mapBankSettingsMapModeToggles ) { tb->set_active( false ); }
            _mapBankSettingsMapModeToggles[ selb.getMapMode( ) ]->set_active( );
        }
    }

    void root::markTileSetsChanged( mapBank::status p_newStatus ) {
        if( _editTileSet ) { _editTileSet->setStatus( p_newStatus ); }
    }

    void root::markBankChanged( u16 p_bank, mapBank::status p_newStatus ) {
        if( !_mapBanks.count( p_bank ) ) { return; }
        if( _mapBanks[ p_bank ].m_widget ) {
            _mapBanks[ p_bank ].m_widget->setStatus( p_newStatus );
        }
    }

    void root::currentMapUpdateTS1( u8 p_newTS ) {
        if( _selectedBank == -1 || _selectedMapY == -1 || _selectedMapX == -1 ) { return; }
        auto& mp = _mapBanks[ _selectedBank ].m_bank->m_slices[ _selectedMapY ][ _selectedMapX ];

        // although it shouldn't, bs1 can use tiles or palettes from ts2
        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts, p_newTS, mp.m_data.m_tIdx2 );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        buildPalette( pals, p_newTS, mp.m_data.m_tIdx2 );

        _currentBlockset1
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ p_newTS ].m_blockSet, &ts );
        _currentBlockset2 = DATA::mapBlockAtom::computeBlockSet(
            &_blockSets[ mp.m_data.m_tIdx2 ].m_blockSet, &ts );

        _ts1widget.set( _currentBlockset1, pals, _blockSetWidth );
        _ts2widget.set( _currentBlockset2, pals, _blockSetWidth );
        _ts1widget.setDaytime( _currentDayTime );
        _ts2widget.setDaytime( _currentDayTime );
        _ts1widget.draw( );
        _ts2widget.draw( );

        if( p_newTS != mp.m_data.m_tIdx1 ) {
            mp.m_data.m_tIdx1 = p_newTS;
            markBankChanged( _selectedBank );
            redrawMap( _selectedMapY, _selectedMapX );
        }
    }

    void root::currentMapUpdateTS2( u8 p_newTS ) {
        if( _selectedBank == -1 || _selectedMapY == -1 || _selectedMapX == -1 ) { return; }
        auto& mp = _mapBanks[ _selectedBank ].m_bank->m_slices[ _selectedMapY ][ _selectedMapX ];

        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts, mp.m_data.m_tIdx1, p_newTS );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        buildPalette( pals, mp.m_data.m_tIdx1, p_newTS );

        _currentBlockset1 = DATA::mapBlockAtom::computeBlockSet(
            &_blockSets[ mp.m_data.m_tIdx1 ].m_blockSet, &ts );
        _currentBlockset2
            = DATA::mapBlockAtom::computeBlockSet( &_blockSets[ p_newTS ].m_blockSet, &ts );

        _ts1widget.set( _currentBlockset1, pals, _blockSetWidth );
        _ts2widget.set( _currentBlockset2, pals, _blockSetWidth );
        _ts1widget.setDaytime( _currentDayTime );
        _ts2widget.setDaytime( _currentDayTime );
        _ts1widget.draw( );
        _ts2widget.draw( );

        if( p_newTS != mp.m_data.m_tIdx2 ) {
            mp.m_data.m_tIdx2 = p_newTS;
            markBankChanged( _selectedBank );
            redrawMap( _selectedMapY, _selectedMapX );
        }
    }

    void root::buildBlockSet( DATA::blockSet<2>* p_out, s8 p_ts1, s8 p_ts2 ) {
        u8 ts1 = 0;
        u8 ts2 = 0;
        if( p_ts1 != -1 ) {
            ts1 = p_ts1;
        } else {
            ts1 = _mapBanks[ _selectedBank ]
                      .m_bank->m_slices[ _selectedMapY ][ _selectedMapX ]
                      .m_data.m_tIdx1;
        }
        if( p_ts2 != -1 ) {
            ts2 = p_ts2;
        } else {
            ts2 = _mapBanks[ _selectedBank ]
                      .m_bank->m_slices[ _selectedMapY ][ _selectedMapX ]
                      .m_data.m_tIdx2;
        }
        std::memcpy( p_out->m_blocks, _blockSets[ ts1 ].m_blockSet.m_blocks,
                     sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
        std::memcpy( &p_out->m_blocks[ DATA::MAX_BLOCKS_PER_TILE_SET ],
                     _blockSets[ ts2 ].m_blockSet.m_blocks,
                     sizeof( DATA::block ) * DATA::MAX_BLOCKS_PER_TILE_SET );
    }

    void root::buildTileSet( DATA::tileSet<2>* p_out, s8 p_ts1, s8 p_ts2 ) {
        u8 ts1 = 0;
        u8 ts2 = 0;
        if( p_ts1 != -1 ) {
            ts1 = p_ts1;
        } else {
            ts1 = _mapBanks[ _selectedBank ]
                      .m_bank->m_slices[ _selectedMapY ][ _selectedMapX ]
                      .m_data.m_tIdx1;
        }
        if( p_ts2 != -1 ) {
            ts2 = p_ts2;
        } else {
            ts2 = _mapBanks[ _selectedBank ]
                      .m_bank->m_slices[ _selectedMapY ][ _selectedMapX ]
                      .m_data.m_tIdx2;
        }
        std::memcpy( p_out->m_tiles, _blockSets[ ts1 ].m_tileSet.m_tiles,
                     sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
        std::memcpy( &p_out->m_tiles[ DATA::MAX_TILES_PER_TILE_SET ],
                     _blockSets[ ts2 ].m_tileSet.m_tiles,
                     sizeof( DATA::tile ) * DATA::MAX_TILES_PER_TILE_SET );
    }

    void root::buildPalette( DATA::palette p_out[ 5 * 16 ], s8 p_ts1, s8 p_ts2 ) {
        u8 ts1 = 0;
        u8 ts2 = 0;
        if( p_ts1 != -1 ) {
            ts1 = p_ts1;
        } else {
            ts1 = _mapBanks[ _selectedBank ]
                      .m_bank->m_slices[ _selectedMapY ][ _selectedMapX ]
                      .m_data.m_tIdx1;
        }
        if( p_ts2 != -1 ) {
            ts2 = p_ts2;
        } else {
            ts2 = _mapBanks[ _selectedBank ]
                      .m_bank->m_slices[ _selectedMapY ][ _selectedMapX ]
                      .m_data.m_tIdx2;
        }
        for( u8 dt = 0; dt < 5; ++dt ) {
            std::memcpy( &p_out[ 16 * dt ], &_blockSets[ ts1 ].m_pals[ 8 * dt ],
                         sizeof( DATA::palette ) * 8 );
            std::memcpy( &p_out[ 16 * dt + 6 ], &_blockSets[ ts2 ].m_pals[ 8 * dt ],
                         sizeof( DATA::palette ) * 8 );
        }
    }

    void root::redrawMap( u8 p_mapY, u8 p_mapX ) {
        _selectedMapX = p_mapX;
        _selectedMapY = p_mapY;

        const auto& mb    = _mapBanks[ _selectedBank ];
        const auto& mbank = mb.m_bank->m_slices;

        for( s8 x = -1; x <= 1; ++x ) {
            for( s8 y = -1; y <= 1; ++y ) {
                bool empty = false;
                if( not( p_mapY + y >= 0 && p_mapY + y <= mb.getSizeY( ) && p_mapX + x >= 0
                         && p_mapX + x <= mb.getSizeX( ) ) ) {
                    empty = true;
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

                auto filtered = std::vector<DATA::mapBlockAtom>( );
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
                            if( empty ) {
                                filtered.push_back( { 0, 1 } );
                            } else {
                                filtered.push_back(
                                    mbank[ p_mapY + y ][ p_mapX + x ].m_data.m_blocks[ y2 ][ x2 ] );
                            }
                        }
                    }
                }

                _currentMap[ x + 1 ][ y + 1 ].set(
                    filtered,
                    [ this ]( DATA::mapBlockAtom p_block ) {
                        return blockSetLookup( p_block.m_blockidx );
                    },
                    mapwd );
                _currentMap[ x + 1 ][ y + 1 ].setOverlayHidden( _currentMapDisplayMode
                                                                != MODE_EDIT_MOVEMENT );
                _currentMap[ x + 1 ][ y + 1 ].draw( );
            }
        }
    }

    void root::moveToMap( s8 p_dy, s8 p_dx ) {
        if( !_selectedMapX && p_dx < 0 ) { return; }
        if( !_selectedMapY && p_dy < 0 ) { return; }

        if( int( _selectedMapY + p_dy ) > int( MAX_MAPY ) ) { return; }
        if( int( _selectedMapX + p_dx ) > int( MAX_MAPY ) ) { return; }

        onUnloadMap( _selectedBank, _selectedMapY, _selectedMapX );

        _selectedMapX += p_dx;
        _selectedMapY += p_dy;

        if( _selectedMapY > _mapBanks[ _selectedBank ].getSizeY( ) ) {
            _mapBanks[ _selectedBank ].setSizeY( _selectedMapY );
            _mapBanks[ _selectedBank ].m_widget->setSizeY( _selectedMapY );
            _mapBanks[ _selectedBank ].m_bank->m_slices.push_back( std::vector<DATA::mapSlice>(
                _mapBanks[ _selectedBank ].getSizeX( ) + 1, DATA::mapSlice( ) ) );
            _mapBanks[ _selectedBank ].m_bank->m_mapData.push_back( std::vector<DATA::mapData>(
                _mapBanks[ _selectedBank ].getSizeX( ) + 1, DATA::mapData( ) ) );

            _mapBanks[ _selectedBank ].m_computedBank->push_back(
                std::vector<DATA::computedMapSlice>( _mapBanks[ _selectedBank ].getSizeX( ) + 1,
                                                     DATA::computedMapSlice( ) ) );
            _mapBankOverview.set( *_mapBanks[ _selectedBank ].m_computedBank );
            _mapBankOverview.redraw( _currentDayTime );
            markBankChanged( _selectedBank );
        }
        if( _selectedMapX > _mapBanks[ _selectedBank ].getSizeX( ) ) {
            _mapBanks[ _selectedBank ].setSizeX( _selectedMapX );
            _mapBanks[ _selectedBank ].m_widget->setSizeX( _selectedMapX );
            for( u8 y = 0; y <= _mapBanks[ _selectedBank ].getSizeY( ); ++y ) {
                _mapBanks[ _selectedBank ].m_bank->m_slices[ y ].push_back( DATA::mapSlice( ) );
                _mapBanks[ _selectedBank ].m_bank->m_mapData[ y ].push_back( DATA::mapData( ) );
                ( *_mapBanks[ _selectedBank ].m_computedBank )[ y ].push_back(
                    DATA::computedMapSlice( ) );
            }
            _mapBankOverview.set( *_mapBanks[ _selectedBank ].m_computedBank );
            _mapBankOverview.redraw( _currentDayTime );
            markBankChanged( _selectedBank );
        }

        loadMap( _selectedBank, _selectedMapY, _selectedMapX );
    }

    void root::onUnloadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( !_mapBanks.count( p_bank ) ) [[unlikely]] { return; }

        auto ts1 = _mapBanks[ p_bank ].m_bank->m_slices[ p_mapY ][ p_mapX ].m_data.m_tIdx1;
        auto ts2 = _mapBanks[ p_bank ].m_bank->m_slices[ p_mapY ][ p_mapX ].m_data.m_tIdx2;

        auto bs = DATA::blockSet<2>( );
        buildBlockSet( &bs, ts1, ts2 );
        auto ts = DATA::tileSet<2>( );
        buildTileSet( &ts, ts1, ts2 );

        buildPalette( ( *_mapBanks[ p_bank ].m_computedBank )[ p_mapY ][ p_mapX ].m_pals, ts1,
                      ts2 );
        ( *_mapBanks[ p_bank ].m_computedBank )[ p_mapY ][ p_mapX ].m_computedBlocks
            = _mapBanks[ p_bank ].m_bank->m_slices[ p_mapY ][ p_mapX ].compute( &bs, &ts );

        if( p_bank == _selectedBank ) {
            _mapBankOverview.replaceMap(
                ( *_mapBanks[ p_bank ].m_computedBank )[ p_mapY ][ p_mapX ], p_mapY, p_mapX );
        }
    }

    void root::loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( _selectedBank == -1 ) { switchContext( MAP_EDITOR ); }

        loadMapBank( p_bank );
        if( _mapBanks[ _selectedBank ].m_bank == nullptr || !_mapBanks[ _selectedBank ].m_loaded ) {
            return;
        }

        // update drop downs
        auto& mp       = _mapBanks[ _selectedBank ].m_bank->m_slices[ p_mapY ][ p_mapX ];
        u8    ts1      = mp.m_data.m_tIdx1;
        u8    ts2      = mp.m_data.m_tIdx2;
        _disableRedraw = true;
        _mapEditorBS1CB.set_selected( _blockSets[ ts1 ].m_stringListItem );
        _mapEditorBS2CB.set_selected( _blockSets[ ts2 ].m_stringListItem );
        _disableRedraw = false;

        _selectedMapX = p_mapX;
        _selectedMapY = p_mapY;
        currentMapUpdateTS1( ts1 );
        currentMapUpdateTS2( ts2 );
        redrawMap( p_mapY, p_mapX );
        fprintf( stderr, "[LOG] Loading map %hu/%hhu_%hhu.\n", p_bank, p_mapY, p_mapX );
        updateSelectedBlock( _currentlySelectedBlock );

        switchContext( context::MAP_EDITOR );

        setTitle( "",
                  std::to_string( p_bank ) + "/" + std::to_string( p_mapY ) + "_"
                      + std::to_string( p_mapX ) + ".map",
                  FSROOT_PATH );
        _mapBankOverview.selectMap( p_mapX, p_mapY );
    }

    void root::switchContext( root::context p_context ) {
        _loadReloadmapAction->set_enabled( false );
        _loadReloadmapbankAction->set_enabled( false );
        _loadImportmapAction->set_enabled( false );
        _saveFsrootAction->set_enabled( false );
        _saveMapAction->set_enabled( false );
        _saveMapbankAction->set_enabled( false );
        _saveExportmapAction->set_enabled( false );
        _openMenuButton->hide( );
        _saveMenuButton->hide( );
        _loadMapLabel.hide( );
        _mapNotebook.hide( );
        _tseNotebook.hide( );
        _ivScrolledWindow.hide( );
        _mainBox.hide( );
        if( _editTileSet != nullptr ) { _editTileSet->unselect( ); }
        if( _saveButton != nullptr ) { _saveButton->hide( ); }

        switch( p_context ) {
        case FSROOT_NONE:
            _mainBox.show( );
            _loadMapLabel.show( );
            setTitle( "", FSROOT_PATH );

            break;
        case MAP_EDITOR:
            _mainBox.show( );
            _mapNotebook.show( );
            if( _saveButton != nullptr ) { _saveButton->show( ); }

            _loadReloadmapAction->set_enabled( true );
            _loadReloadmapbankAction->set_enabled( true );
            _loadImportmapAction->set_enabled( true );
            _saveFsrootAction->set_enabled( true );
            _saveMapAction->set_enabled( true );
            _saveExportmapAction->set_enabled( true );
            _saveMapbankAction->set_enabled( true );
            // populate open menu buttons
            _openMenuButton->show( );
            _saveMenuButton->show( );
            break;
        case TILE_EDITOR:
            _mainBox.show( );
            _tseNotebook.show( );
            if( _saveButton != nullptr ) { _saveButton->show( ); }
            if( _editTileSet != nullptr ) { _editTileSet->select( ); }

            // TODO: actions

            break;
        default:
            setTitle( );
            _ivScrolledWindow.show( );
            break;
        }
    }
} // namespace UI
