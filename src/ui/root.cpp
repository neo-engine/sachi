#include <gtkmm/cssprovider.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>

#include "../data/fs.h"
#include "../data/fs/util.h"
#include "../defines.h"
#include "../log.h"
#include "pure/util.h"
#include "root.h"

namespace UI {
    void root::initActions( ) {
        _loadActions = Gio::SimpleActionGroup::create( );
        _saveActions = Gio::SimpleActionGroup::create( );
        _loadFsrootAction
            = _loadActions->add_action( "fsroot", [ & ]( ) { this->onFsRootOpenClick( ); } );
        _loadReloadmapAction      = _loadActions->add_action( "reloadmap", [ & ]( ) {
            _model.readMapSlice( _model.selectedBank( ), _model.selectedMapX( ),
                                      _model.selectedMapY( ) );
            redraw( );
        } );
        _loadReloadmapbankAction  = _loadActions->add_action( "reloadmapbank", [ & ]( ) {
            _model.checkOrLoadBank( _model.selectedBank( ), true );
            redraw( );
        } );
        _loadImportmapAction      = _loadActions->add_action( "importmap", [ & ]( ) {
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
                    _model.readMapSlice( _model.selectedBank( ), _model.selectedMapX( ),
                                              _model.selectedMapY( ), dialog->get_file( )->get_path( ) );
                    // redrawMap( _model.selectedMapY( ), _model.selectedMapX( ) );
                    _model.markSelectedBankChanged( );
                    redraw( );
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
        _loadImportlargemapAction = _loadActions->add_action( "importlargemap", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Choose a map to import",
                                                      Gtk::FileChooser::Action::OPEN, true );

            auto posbox = Gtk::Box{ Gtk::Orientation::HORIZONTAL };
            posbox.set_halign( Gtk::Align::END );
            auto posbox2  = Gtk::Box{ Gtk::Orientation::HORIZONTAL };
            auto poslabel = Gtk::Label{ "Insert at position (x|y)" };
            poslabel.set_margin( MARGIN );
            auto sba1 = Gtk::Adjustment::create( 0, 0.0, 31.0, 1.0, 1.0, 0.0 );
            auto sba2 = Gtk::Adjustment::create( 0, 0.0, 31.0, 1.0, 1.0, 0.0 );

            _sb1 = Gtk::SpinButton{ sba1 };
            _sb2 = Gtk::SpinButton{ sba2 };

            posbox.set_margin( MARGIN );
            posbox.append( poslabel );
            posbox2.append( _sb1 );
            posbox2.append( _sb2 );
            posbox2.get_style_context( )->add_class( "linked" );
            posbox.append( posbox2 );

            auto sep = Gtk::Separator{ };

            dialog->get_content_area( )->prepend( sep );
            dialog->get_content_area( )->prepend( posbox );

            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.map" );
            mapFilter->set_name( "AdvanceMap 1.92 Map Files (any size)" );
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
                    _model.readLargeMap( _model.selectedBank( ), _model.selectedMapX( ),
                                         _model.selectedMapY( ),

                                         _sb1.get_value_as_int( ), _sb2.get_value_as_int( ),
                                         dialog->get_file( )->get_path( ) );
                    // redrawMap( _model.selectedMapY( ), _model.selectedMapX( ) );
                    _model.markSelectedBankChanged( );
                    redraw( );
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

        _loadImportblocks1Action = _loadActions->add_action( "importblocks1", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Choose a block set to import as block set 1",
                                                      Gtk::FileChooser::Action::OPEN, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.bvd" );
            mapFilter->set_name( "AdvanceMap 1.92 Block Set Files" );
            dialog->set_filter( mapFilter );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "importBlocks", "Dialog is nullptr." );
                    return;
                }

                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.readBlockSet( _model.m_settings.m_tseBS1,
                                         dialog->get_file( )->get_path( ) );
                    // redrawMap( _model.selectedMapY( ), _model.selectedMapX( ) );
                    _model.markTileSetsChanged( );
                    redraw( );
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
        _loadImportblocks2Action = _loadActions->add_action( "importblocks2", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Choose a block set to import as block set 2",
                                                      Gtk::FileChooser::Action::OPEN, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.bvd" );
            mapFilter->set_name( "AdvanceMap 1.92 Block Set Files" );
            dialog->set_filter( mapFilter );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "importBlocks", "Dialog is nullptr." );
                    return;
                }

                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.readBlockSet( _model.m_settings.m_tseBS2,
                                         dialog->get_file( )->get_path( ) );
                    // redrawMap( _model.selectedMapY( ), _model.selectedMapX( ) );
                    _model.markTileSetsChanged( );
                    redraw( );
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
        _loadImporttiles1Action  = _loadActions->add_action( "importtiles1", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Choose a tile set to import as tile set 1",
                                                       Gtk::FileChooser::Action::OPEN, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.ts" );
            mapFilter->set_name( "neo Tile Set Files" );
            dialog->set_filter( mapFilter );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "importTiles", "Dialog is nullptr." );
                    return;
                }

                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.readTileSet( _model.m_settings.m_tseBS1,
                                         dialog->get_file( )->get_path( ) );
                    // redrawMap( _model.selectedMapY( ), _model.selectedMapX( ) );
                    _model.markTileSetsChanged( );
                    redraw( );
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
        _loadImporttiles2Action  = _loadActions->add_action( "importtiles2", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Choose a tile set to import as tile set 2",
                                                       Gtk::FileChooser::Action::OPEN, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.ts" );
            mapFilter->set_name( "neo Tile Set Files" );
            dialog->set_filter( mapFilter );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "importTiles", "Dialog is nullptr." );
                    return;
                }

                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.readTileSet( _model.m_settings.m_tseBS2,
                                         dialog->get_file( )->get_path( ) );
                    // redrawMap( _model.selectedMapY( ), _model.selectedMapX( ) );
                    _model.markTileSetsChanged( );
                    redraw( );
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
        _saveMapAction       = _saveActions->add_action( "map", [ & ]( ) {
            _model.writeMapSlice( _model.selectedBank( ), _model.selectedMapX( ),
                                        _model.selectedMapY( ) );
            redraw( );
        } );
        _saveMapbankAction   = _saveActions->add_action( "mapbank", [ & ]( ) {
            _model.writeMapBank( _model.selectedBank( ) );
            redraw( );
        } );
        _saveExportmapAction = _saveActions->add_action( "exportmap", [ & ]( ) {
            auto dialog    = new Gtk::FileChooserDialog( "Save the current map",
                                                         Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.map" );
            mapFilter->set_name( "AdvanceMap 1.92 Map Files (32x32 blocks)" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _model.selectedMapY( ) ) + "_"
                                      + std::to_string( _model.selectedMapX( ) ) + ".map " );
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
                    _model.writeMapSlice( _model.selectedBank( ), _model.selectedMapX( ),
                                          _model.selectedMapY( ),
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

        _saveExportblocks1Action = _saveActions->add_action( "exportblocks1", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Export the blocks of the current tile set 1",
                                                      Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.bvd" );
            mapFilter->set_name( "AdvanceMap 1.92 Block Set File" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _model.m_settings.m_tseBS1 ) + ".bvd" );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "exportBlocks", "Dialog is nullptr." );
                    return;
                }
                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.writeBlockSet( _model.m_settings.m_tseBS1,
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
        _saveExportblocks2Action = _saveActions->add_action( "exportblocks2", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Export the blocks of the current tile set 2",
                                                      Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.bvd" );
            mapFilter->set_name( "AdvanceMap 1.92 Block Set File" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _model.m_settings.m_tseBS2 ) + ".bvd" );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "exportBlocks", "Dialog is nullptr." );
                    return;
                }
                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.writeBlockSet( _model.m_settings.m_tseBS2,
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
        _saveExporttiles1Action  = _saveActions->add_action( "exporttiles1", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Export the tiles of the current tile set 1",
                                                       Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.ts" );
            mapFilter->set_name( "neo Tile Set File" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _model.m_settings.m_tseBS1 ) + ".ts" );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "exportBlocks", "Dialog is nullptr." );
                    return;
                }
                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.writeTileSet( _model.m_settings.m_tseBS1,
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
        _saveExporttiles2Action  = _saveActions->add_action( "exporttiles2", [ & ]( ) {
            auto dialog = new Gtk::FileChooserDialog( "Export the tiles of the current tile set 2",
                                                       Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.ts" );
            mapFilter->set_name( "neo Tile Set File" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _model.m_settings.m_tseBS2 ) + ".ts" );
            dialog->set_transient_for( *this );
            dialog->set_modal( true );
            dialog->set_default_size( 800, 600 );
            dialog->signal_response( ).connect( [ dialog, this ]( int p_responseId ) {
                if( dialog == nullptr ) {
                    message_error( "exportBlocks", "Dialog is nullptr." );
                    return;
                }
                // Handle the response:
                switch( p_responseId ) {
                case Gtk::ResponseType::OK: {
                    _model.writeTileSet( _model.m_settings.m_tseBS2,
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

    void root::initEvents( ) {
        // Events / Button presses

        auto controller = Gtk::EventControllerKey::create( );
        controller->signal_key_pressed( ).connect(
            [ this ]( unsigned p_keyval, unsigned, Gdk::ModifierType p_state ) -> bool {
                // On Ctrl+0: Hide/Show map bank bar and navigation bar
                if( p_keyval == GDK_KEY_0
                    && ( p_state & ( Gdk::ModifierType::CONTROL_MASK ) )
                           == Gdk::ModifierType::CONTROL_MASK ) {
                    _model.m_settings.m_focusMode = !_model.m_settings.m_focusMode;
                    if( !_model.m_settings.m_focusMode && _sideBar ) {
                        _sideBar->collapse( _model.m_settings.m_focusMode );
                    }
                    redraw( );
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

        initActions( );

        _headerBar = std::make_shared<headerBar>( _model, *this );
        if( _headerBar ) { set_titlebar( *_headerBar ); }

        auto mbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _mainBox  = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        set_child( mbox );
        mbox.append( _mainBox );

        _welcome = std::make_shared<welcome>( *this );
        if( _welcome ) {
            _welcome->connect(
                [ &, this ]( const std::string& p_path ) { this->loadNewFsRoot( p_path ); } );
            mbox.append( *_welcome );
        }

        _sideBar = std::make_shared<sideBar>( _model, *this );
        if( _sideBar ) { _mainBox.append( *_sideBar ); }

        _loadMapLabel = Gtk::Label( );
        _loadMapLabel.set_markup( "<span size=\"x-large\">Add or load a map bank!</span>" );
        _loadMapLabel.set_expand( );
        _mainBox.append( _loadMapLabel );

        _bankEditor = std::make_shared<bankEditor>( _model, *this );
        if( _bankEditor ) { _mainBox.append( *_bankEditor ); }

        _tileSetEditor = std::make_shared<tileSetEditor>( _model, *this );
        if( _tileSetEditor ) { _mainBox.append( *_tileSetEditor ); }

        initEvents( );

        switchContext( CONTEXT_NONE );
    }

    void root::loadNewFsRoot( const std::string& p_path ) {
        _model = model::readFromPath( p_path );
        if( !_model.m_good ) {
            switchContext( CONTEXT_NONE );
            return;
        }

        if( _sideBar ) { _sideBar->reinit( ); }
        switchContext( CONTEXT_FSROOT_NONE );

        addFsRootToRecent( p_path );
    }

    void root::switchContext( context p_context ) {
        _context = p_context;
        _loadReloadmapAction->set_enabled( false );
        _loadReloadmapbankAction->set_enabled( false );
        _loadImportmapAction->set_enabled( false );
        _loadImportlargemapAction->set_enabled( false );
        _saveFsrootAction->set_enabled( false );
        _saveMapAction->set_enabled( false );
        _saveMapbankAction->set_enabled( false );
        _saveExportmapAction->set_enabled( false );

        _loadImportblocks1Action->set_enabled( false );
        _loadImportblocks2Action->set_enabled( false );
        _saveExportblocks1Action->set_enabled( false );
        _saveExportblocks2Action->set_enabled( false );
        _loadImporttiles1Action->set_enabled( false );
        _loadImporttiles2Action->set_enabled( false );
        _saveExporttiles1Action->set_enabled( false );
        _saveExporttiles2Action->set_enabled( false );

        _loadMapLabel.hide( );
        if( _tileSetEditor ) { _tileSetEditor->hide( ); }
        if( _bankEditor ) { _bankEditor->hide( ); }
        if( _welcome ) { _welcome->hide( ); }
        _mainBox.hide( );

        switch( p_context ) {
        case CONTEXT_FSROOT_NONE:
            _saveFsrootAction->set_enabled( true );
            _mainBox.show( );
            _loadMapLabel.show( );

            break;
        case CONTEXT_MAP_EDITOR:
            _mainBox.show( );
            if( _bankEditor ) { _bankEditor->show( ); }

            _loadReloadmapAction->set_enabled( true );
            _loadReloadmapbankAction->set_enabled( true );
            _loadImportmapAction->set_enabled( true );
            _loadImportlargemapAction->set_enabled( true );
            _saveFsrootAction->set_enabled( true );
            _saveMapAction->set_enabled( true );
            _saveExportmapAction->set_enabled( true );
            _saveMapbankAction->set_enabled( true );
            break;
        case CONTEXT_TILE_EDITOR:
            _mainBox.show( );
            if( _tileSetEditor ) { _tileSetEditor->show( ); }

            _saveFsrootAction->set_enabled( true );
            _loadImportblocks1Action->set_enabled( true );
            _loadImportblocks2Action->set_enabled( true );
            _saveExportblocks1Action->set_enabled( true );
            _saveExportblocks2Action->set_enabled( true );
            _loadImporttiles1Action->set_enabled( true );
            _loadImporttiles2Action->set_enabled( true );
            _saveExporttiles1Action->set_enabled( true );
            _saveExporttiles2Action->set_enabled( true );

            break;
        default:
            if( _welcome ) { _welcome->show( ); }
            break;
        }

        if( _headerBar ) { _headerBar->switchContext( p_context ); }
        if( _sideBar ) { _sideBar->switchContext( p_context ); }

        redraw( );
    }

    void root::redraw( ) {
        if( _headerBar ) { _headerBar->redraw( ); }
        redrawPanel( );
        if( _context == CONTEXT_MAP_EDITOR ) {
            if( _bankEditor ) { _bankEditor->redraw( ); }
        }
        if( _context == CONTEXT_TILE_EDITOR ) {
            if( _tileSetEditor ) { _tileSetEditor->redraw( ); }
        }
    }

    void root::redrawPanel( ) {
        if( _sideBar ) { _sideBar->redraw( ); }
    }

    void root::loadMapBank( u16 p_bank, bool p_redraw ) {
        if( p_bank == _model.selectedBank( ) || !_model.checkOrLoadBank( p_bank, false ) ) {
            return;
        }

        _model.selectBank( p_bank );
        _model.m_settings.m_overviewNeedsRedraw = true;

        if( p_redraw ) { redraw( ); }
    }

    void root::onUnloadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( !_model.existsBank( p_bank ) ) [[unlikely]] { return; }

        auto ts1 = _model.slice( p_bank, p_mapY, p_mapX ).m_data.m_tIdx1;
        auto ts2 = _model.slice( p_bank, p_mapY, p_mapX ).m_data.m_tIdx2;

        auto bs = DATA::blockSet<2>( );
        _model.buildBlockSet( &bs, ts1, ts2 );
        auto ts = DATA::tileSet<2>( );
        _model.buildTileSet( &ts, ts1, ts2 );

        _model.buildPalette( _model.bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX ].m_pals, ts1,
                             ts2 );
        _model.bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX ].m_computedBlocks
            = _model.slice( p_bank, p_mapY, p_mapX ).compute( &bs, &ts );

        if( p_bank == _model.selectedBank( ) && _bankEditor ) {
            _bankEditor->replaceOverviewMap( _model.bank( ).m_computedBank[ p_mapY ][ p_mapX ],
                                             p_mapY, p_mapX );
        }
    }

    void root::loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( _model.selectedBank( ) == -1 ) { switchContext( CONTEXT_MAP_EDITOR ); }

        loadMapBank( p_bank );
        if( !_model.bank( ).m_loaded ) { return; }

        _model.selectMap( p_mapX, p_mapY );
        _model.m_settings.m_tseBS1 = _model.slice( ).m_data.m_tIdx1;
        _model.m_settings.m_tseBS2 = _model.slice( ).m_data.m_tIdx2;

        if( _model.selectedBankIsDive( ) ) {
            _headerBar->setTitle( "",
                                  std::to_string( p_bank % DIVE_MAP ) + "/" + std::to_string( p_mapY ) + "_"
                                      + std::to_string( p_mapX ) + ".map (Underwater)",
                                  _model.m_fsdata.m_fsrootPath, false );
        } else {
            _headerBar->setTitle( "",
                                  std::to_string( p_bank ) + "/" + std::to_string( p_mapY ) + "_"
                                      + std::to_string( p_mapX ) + ".map",
                                  _model.m_fsdata.m_fsrootPath, false );
        }

        switchContext( CONTEXT_MAP_EDITOR );
        redraw( );
    }

    void root::moveToMap( s8 p_dy, s8 p_dx ) {
        if( !_model.selectedMapX( ) && p_dx < 0 ) { return; }
        if( !_model.selectedMapY( ) && p_dy < 0 ) { return; }

        if( int( _model.selectedMapY( ) + p_dy ) > int( MAX_MAPY ) ) { return; }
        if( int( _model.selectedMapX( ) + p_dx ) > int( MAX_MAPY ) ) { return; }

        onUnloadMap( _model.selectedBank( ), _model.selectedMapY( ), _model.selectedMapX( ) );

        _model.selectMap( _model.selectedMapX( ) + p_dx, _model.selectedMapY( ) + p_dy );

        loadMap( _model.selectedBank( ), _model.selectedMapY( ), _model.selectedMapX( ) );
    }

    void root::editTileSets( u8 p_ts1, u8 p_ts2 ) {
        if( _context == CONTEXT_NONE ) {
            message_log( "editTileSets", "No FSROOT loaded. Won't load any tile sets." );
            return;
        }

        message_log( "editTileSets",
                     "Loading tile set editor for " + std::to_string( p_ts1 ) + "|"
                         + std::to_string( p_ts2 ) + ".",
                     LOGLEVEL_STATUS );

        if( _model.selectedBank( ) != -1 ) {
            onUnloadMap( _model.selectedBank( ), _model.selectedMapY( ), _model.selectedMapX( ) );
        }
        _model.selectBank( -1 );
        _headerBar->setTitle( "",
                              "Tile Set " + std::to_string( p_ts1 ) + "|" + std::to_string( p_ts2 ),
                              _model.m_fsdata.m_fsrootPath, false );

        // check if block sets exist

        if( !_model.m_fsdata.m_blockSetNames.count( p_ts1 ) ) { _model.createBlockSet( p_ts1 ); }
        if( !_model.m_fsdata.m_blockSetNames.count( p_ts2 ) ) { _model.createBlockSet( p_ts2 ); }

        _model.m_settings.m_tseBS1 = p_ts1;
        _model.m_settings.m_tseBS2 = p_ts2;

        switchContext( CONTEXT_TILE_EDITOR );
        redraw( );
    }

    void root::createMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX ) {
        if( _context == CONTEXT_NONE ) {
            message_log( "createMapBank", "No FSROOT loaded. Won't create a new map bank." );
            return;
        }
        if( _model.existsBank( p_bank ) ) {
            // map exists, do nothing
            message_log( "createMapBank", std::string( "Map bank " ) + std::to_string( p_bank )
                                              + " already exists.\n" );
            return;
        }

        _model.addNewMapBank( p_bank, p_sizeY, p_sizeX, DATA::MAPMODE_COMBINED, STATUS_NEW );
        if( _sideBar ) { _sideBar->reinit( ); }
        redraw( );
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
        _model.writeFsRoot( );
        redraw( );
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
} // namespace UI
