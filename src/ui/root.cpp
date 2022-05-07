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

#include "../data/fs.h"
#include "../data/fs/util.h"
#include "../defines.h"
#include "../log.h"
#include "root.h"
#include "util.h"

namespace UI {
    /*
    void root::initActions( ) {
        _loadActions = Gio::SimpleActionGroup::create( );
        _saveActions = Gio::SimpleActionGroup::create( );
        _loadFsrootAction
            = _loadActions->add_action( "fsroot", [ & ]( ) { this->onFsRootOpenClick( ); } );
        _loadReloadmapAction     = _loadActions->add_action( "reloadmap", [ & ]( ) {
            readMapSlice( _sideBar->selectedBank( ), _sideBar->selectedMapX( ),
                              _sideBar->selectedMapY( ) );
            redrawMap( _sideBar->selectedMapY( ), _sideBar->selectedMapX( ) );
            } );
        _loadReloadmapbankAction = _loadActions->add_action( "reloadmapbank", [ & ]( ) {
            readMapBank( _sideBar->selectedBank( ), true );
            redrawMap( _sideBar->selectedMapY( ), _sideBar->selectedMapX( ) );
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
                    readMapSlice( _sideBar->selectedBank( ), _sideBar->selectedMapX( ),
                                      _sideBar->selectedMapY( ), dialog->get_file( )->get_path( ) );
                    redrawMap( _sideBar->selectedMapY( ), _sideBar->selectedMapX( ) );
                    _sideBar->markSelectedBankChanged( );
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
        _saveMapAction     = _saveActions->add_action( "map", [ & ]( ) {
            writeMapSlice( _sideBar->selectedBank( ), _sideBar->selectedMapX( ),
                               _sideBar->selectedMapY( ) );
            } );
        _saveMapbankAction = _saveActions->add_action(
            "mapbank", [ & ]( ) { writeMapBank( _sideBar->selectedBank( ) ); } );
        _saveExportmapAction = _saveActions->add_action( "exportmap", [ & ]( ) {
            auto dialog    = new Gtk::FileChooserDialog( "Save the current map",
                                                         Gtk::FileChooser::Action::SAVE, true );
            auto mapFilter = Gtk::FileFilter::create( );
            mapFilter->add_pattern( "*.map" );
            mapFilter->set_name( "AdvanceMap 1.92 Map Files (32x32 blocks)" );
            dialog->set_filter( mapFilter );
            dialog->set_current_name( std::to_string( _sideBar->selectedMapY( ) ) + "_"
                                      + std::to_string( _sideBar->selectedMapX( ) ) + ".map " );
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
                    writeMapSlice( _sideBar->selectedBank( ), _sideBar->selectedMapX( ),
                                   _sideBar->selectedMapY( ), dialog->get_file( )->get_path( ) );
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
*/

    /*
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
    */

    root::root( ) {
        set_default_size( 800, 600 );
        _model = std::make_unique<model>( );

        if( !_model ) { message_error( "init", "OOF: Creating data representation failed :/" ); }

        auto provider = Gtk::CssProvider::create( );
        provider->load_from_data( EXTRA_CSS );
        Gtk::StyleContext::add_provider_for_display( Gdk::Display::get_default( ), provider,
                                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );

        // initActions( );

        _headerBar = std::make_shared<headerBar>( *_model, *this );
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

        _sideBar = std::make_shared<sideBar>( *_model, *this );
        if( _sideBar ) { _mainBox.append( *_sideBar ); }

        _loadMapLabel = Gtk::Label( );
        _loadMapLabel.set_markup( "<span size=\"x-large\">Add or load a map bank!</span>" );
        _loadMapLabel.set_expand( );
        _mainBox.append( _loadMapLabel );

        //_bankEditor = std::make_shared<bankEditor>( );
        // if( _bankEditor ) { _mainBox.append( *_bankEditor ); }

        // initEvents( );

        switchContext( CONTEXT_NONE );
    }

    void root::loadNewFsRoot( const std::string& p_path ) {
        if( !_model->loadNewFsRoot( p_path ) ) {
            switchContext( CONTEXT_NONE );
            return;
        }

        if( _sideBar ) { _sideBar->reinit( ); }
        switchContext( CONTEXT_FSROOT_NONE );

        addFsRootToRecent( p_path );
    }

    void root::switchContext( context p_context ) {
        _context = p_context;
        /*
        _loadReloadmapAction->set_enabled( false );
        _loadReloadmapbankAction->set_enabled( false );
        _loadImportmapAction->set_enabled( false );
        _saveFsrootAction->set_enabled( false );
        _saveMapAction->set_enabled( false );
        _saveMapbankAction->set_enabled( false );
        _saveExportmapAction->set_enabled( false );
*/

        _loadMapLabel.hide( );
        // if( _bankEditor ) {        _bankEditor->hide( );}
        if( _welcome ) { _welcome->hide( ); }
        _mainBox.hide( );

        switch( p_context ) {
        case CONTEXT_FSROOT_NONE:
            _mainBox.show( );
            _loadMapLabel.show( );

            break;
        case CONTEXT_MAP_EDITOR:
            _mainBox.show( );
            // if( _bankEditor ) { _bankEditor->show( ); }

            /*
            _loadReloadmapAction->set_enabled( true );
            _loadReloadmapbankAction->set_enabled( true );
            _loadImportmapAction->set_enabled( true );
            _saveFsrootAction->set_enabled( true );
            _saveMapAction->set_enabled( true );
            _saveExportmapAction->set_enabled( true );
            _saveMapbankAction->set_enabled( true );
            */
            break;
        case CONTEXT_TILE_EDITOR:
            _mainBox.show( );

            // TODO: actions

            break;
        default:
            if( _welcome ) { _welcome->show( ); }
            break;
        }

        if( _headerBar ) { _headerBar->switchContext( p_context ); }
        if( _sideBar ) { _sideBar->switchContext( p_context ); }
    }

    void root::redraw( ) {
        if( _headerBar ) { _headerBar->redraw( ); }
        if( _sideBar ) { _sideBar->redraw( ); }
    }

    void root::loadMapBank( u16 p_bank, bool p_redraw ) {
        if( p_bank == _model->selectedBank( ) || !_model->checkOrLoadBank( p_bank, false ) ) {
            return;
        }

        _model->selectBank( p_bank );

        if( p_redraw ) { redraw( ); }
    }

    void root::onUnloadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( !_model->existsBank( p_bank ) ) [[unlikely]] { return; }

        auto ts1 = _model->slice( p_bank, p_mapY, p_mapX ).m_data.m_tIdx1;
        auto ts2 = _model->slice( p_bank, p_mapY, p_mapX ).m_data.m_tIdx2;

        auto bs = DATA::blockSet<2>( );
        _model->buildBlockSet( &bs, ts1, ts2 );
        auto ts = DATA::tileSet<2>( );
        _model->buildTileSet( &ts, ts1, ts2 );

        _model->buildPalette( _model->bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX ].m_pals, ts1,
                              ts2 );
        _model->bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX ].m_computedBlocks
            = _model->slice( p_bank, p_mapY, p_mapX ).compute( &bs, &ts );

        if( p_bank == _model->selectedBank( ) ) {
            // _mapBankOverview.replaceMap( _model->bank( p_bank ).m_computedBank[ p_mapY ][ p_mapX
            // ],
            //                              p_mapY, p_mapX );
        }
    }

    void root::loadMap( u16 p_bank, u8 p_mapY, u8 p_mapX ) {
        if( _model->selectedBank( ) == -1 ) { switchContext( CONTEXT_MAP_EDITOR ); }

        loadMapBank( p_bank );
        if( !_model->bank( ).m_loaded ) { return; }

        _model->selectMap( p_mapX, p_mapY );
        _headerBar->setTitle( "",
                              std::to_string( p_bank ) + "/" + std::to_string( p_mapY ) + "_"
                                  + std::to_string( p_mapX ) + ".map",
                              _model->m_fsdata.m_fsrootPath, false );

        switchContext( CONTEXT_MAP_EDITOR );
        redraw( );
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

        if( _model->selectedBank( ) != -1 ) {
            onUnloadMap( _model->selectedBank( ), _model->selectedMapY( ),
                         _model->selectedMapX( ) );
        }
        _model->selectBank( -1 );
        _headerBar->setTitle( "",
                              "Tile Set " + std::to_string( p_ts1 ) + "|" + std::to_string( p_ts2 ),
                              _model->m_fsdata.m_fsrootPath, false );

        // check if block sets exist

        if( !_model->m_fsdata.m_blockSetNames.count( p_ts1 ) ) { _model->createBlockSet( p_ts1 ); }
        if( !_model->m_fsdata.m_blockSetNames.count( p_ts2 ) ) { _model->createBlockSet( p_ts2 ); }

        // compute block set

        auto ts = DATA::tileSet<2>( );
        _model->buildTileSet( &ts, p_ts1, p_ts2 );
        DATA::palette pals[ 16 * 5 ] = { 0 };
        _model->buildPalette( pals, p_ts1, p_ts2 );

        _model->m_fsdata.m_currentBlocksets[ 0 ] = DATA::mapBlockAtom::computeBlockSet(
            &_model->m_fsdata.m_blockSets[ p_ts1 ].m_blockSet, &ts );
        _model->m_fsdata.m_currentBlocksets[ 1 ] = DATA::mapBlockAtom::computeBlockSet(
            &_model->m_fsdata.m_blockSets[ p_ts2 ].m_blockSet, &ts );

        switchContext( CONTEXT_TILE_EDITOR );
    }

    void root::createMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX ) {
        if( _context == CONTEXT_NONE ) {
            message_log( "createMapBank", "No FSROOT loaded. Won't create a new map bank." );
            return;
        }
        if( _model->existsBank( p_bank ) ) {
            // map exists, do nothing
            message_log( "createMapBank", std::string( "Map bank " ) + std::to_string( p_bank )
                                              + " already exists.\n" );
            return;
        }

        _model->addNewMapBank( p_bank, p_sizeY, p_sizeX, DATA::MAPMODE_COMBINED, STATUS_NEW );
        if( _model->selectedBank( ) != -1 ) {
            onUnloadMap( _model->selectedBank( ), _model->selectedMapY( ),
                         _model->selectedMapX( ) );
        }
        loadMap( p_bank, 0, 0 );
    }
} // namespace UI
