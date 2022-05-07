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

        auto provider = Gtk::CssProvider::create( );
        provider->load_from_data( EXTRA_CSS );
        Gtk::StyleContext::add_provider_for_display( Gdk::Display::get_default( ), provider,
                                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );

        // initActions( );

        _headerBar = std::make_shared<headerBar>( _model, this );
        if( _headerBar ) { set_titlebar( *_headerBar ); }

        auto mbox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        _mainBox  = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        set_child( mbox );
        mbox.append( _mainBox );

        _welcome = std::make_shared<welcome>( this );
        if( _welcome ) {
            _welcome->connect(
                [ &, this ]( const std::string& p_path ) { this->loadNewFsRoot( p_path ); } );
            mbox.append( *_welcome );
        }

        /*
        _sideBar = std::make_shared<sideBar>( );
        if( _sideBar ) {
            _sideBar->connect(
                [ this ]( u8 p_ts1, u8 p_ts2 ) { editTileSets( p_ts1, p_ts2 ); },
                [ this ]( u16 p_bk, u8 p_y, u8 p_x ) { createMapBank( p_bk, p_y, p_x ); } );
            _mainBox.append( *_sideBar );
        }
        */

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
        _fsRootLoaded = _model.loadNewFsRoot( p_path );

        /*
        // update map banks
        _sideBar->_mapBanks.clear( );
        _blockSets.clear( );
        _blockSetNames.clear( );
        _sideBar->selectBank( -1 );

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
        */

        addFsRootToRecent( p_path );
    }

    void root::switchContext( context p_context ) {
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
        // if( _sideBar->_editTileSet != nullptr ) { _sideBar->_editTileSet->unselect( ); }

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
            // if( _sideBar->_editTileSet != nullptr ) { _sideBar->_editTileSet->select( ); }

            // TODO: actions

            break;
        default:
            message_log( "init", "context none" );
            if( _welcome ) {
                message_log( "init", "welcome show" );
                _welcome->show( );
            }
            break;
        }

        if( _headerBar ) { _headerBar->switchContext( p_context ); }
    }

    void root::redraw( ) {
        if( _headerBar ) { _headerBar->redraw( ); }
    }
} // namespace UI
