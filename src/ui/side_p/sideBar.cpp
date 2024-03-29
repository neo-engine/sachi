#include <gtkmm/scrolledwindow.h>

#include "../../defines.h"
#include "../pure/util.h"
#include "../root.h"
#include "sideBar.h"

namespace UI {
    sideBar::sideBar( model& p_model, root& p_root ) : _model( p_model ), _rootWindow( p_root ) {
        // main box
        _lMainBox = Gtk::Box( Gtk::Orientation::VERTICAL );

        _collapseMapBanksButton
            = createButton( "view-restore-symbolic", "_Collapse Sidebar",
                            [ & ]( ) { this->collapse( !_mapBankBarCollapsed ); } );

        _collapseMapBanksButton->set_margin( 0 );
        _collapseMapBanksButton->set_has_frame( false );

        {
            // collapse sidebar widget
            auto sideBarBarCollapseBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            sideBarBarCollapseBox.append( *_collapseMapBanksButton );
            sideBarBarCollapseBox.set_halign( Gtk::Align::CENTER );
            _collapseMapBanksButton->set_expand( false );
            _lMainBox.append( sideBarBarCollapseBox );
        }
        {
            // map banks
            auto lScrolledWindow = Gtk::ScrolledWindow( );
            _lMainBox.append( lScrolledWindow );
            auto lFrame = Gtk::Frame( );
            lFrame.set_margin( MARGIN );
            lFrame.set_margin_top( 0 );
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

            _addMapBank = std::make_shared<addMapBank>( p_model );

            if( _addMapBank ) {
                _addMapBank->connect( [ this ]( u16 p_bk, u8 p_y, u8 p_x ) {
                    _rootWindow.createMapBank( p_bk, p_y, p_x );
                } );
                _mapBankBox.append( *_addMapBank );
            }

            _mapBanks.clear( );
        }
        {
            // tile set
            auto lFrame = Gtk::Frame( );
            lFrame.set_margin( MARGIN );
            _lMainBox.append( lFrame );

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

            _editTileSet = std::make_shared<editTileSet>( p_model );
            if( _editTileSet ) {
                _editTileSet->connect(
                    [ this ]( u8 p_ts1, u8 p_ts2 ) { _rootWindow.editTileSets( p_ts1, p_ts2 ); } );
                _sbTileSetBox.append( *_editTileSet );
            }
        }
        {
            // data
            auto lFrame = Gtk::Frame( );
            lFrame.set_margin( MARGIN );
            lFrame.set_margin_top( 0 );
            _lMainBox.append( lFrame );

            auto sbDataBarLabelBox   = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            auto sbDataBarLabelImage = Gtk::Image( );
            sbDataBarLabelImage.set_from_icon_name( "drive-multidisk-symbolic" );
            sbDataBarLabelImage.set_margin( MARGIN );
            _sbDataBarLabel = Gtk::Label( "Data" );
            sbDataBarLabelBox.append( sbDataBarLabelImage );
            sbDataBarLabelBox.append( _sbDataBarLabel );

            lFrame.set_label_widget( sbDataBarLabelBox );
            lFrame.set_label_align( Gtk::Align::CENTER );

            _sbDataBox = Gtk::Box( Gtk::Orientation::VERTICAL, MARGIN );
            lFrame.set_child( _sbDataBox );
            _sbDataBox.set_margin( MARGIN );
            _sbDataBox.set_vexpand( false );

            _editTrainer = std::make_shared<editTrainer>( p_model, "TR" );
            if( _editTrainer ) {
                _editTrainer->connect(
                    [ this ]( u16 p_ts1 ) { _rootWindow.editTrainer( p_ts1 ); } );
                _sbDataBox.append( *_editTrainer );
            }
            _editPkmnData = std::make_shared<editTrainer>( p_model, "PK" );
            if( _editPkmnData ) {
                _editPkmnData->connect(
                    [ this ]( u16 p_ts1 ) { _rootWindow.editPkmnData( p_ts1 ); } );
                _sbDataBox.append( *_editPkmnData );
            }
            _editItemData = std::make_shared<editTrainer>( p_model, "IT" );
            if( _editItemData ) {
                _editItemData->connect(
                    [ this ]( u16 p_ts1 ) { _rootWindow.editItemData( p_ts1 ); } );
                _sbDataBox.append( *_editItemData );
            }
            _editMoveData = std::make_shared<editTrainer>( p_model, "MV" );
            if( _editMoveData ) {
                _editMoveData->connect(
                    [ this ]( u16 p_ts1 ) { _rootWindow.editMoveData( p_ts1 ); } );
                _sbDataBox.append( *_editMoveData );
            }
        }
    }

    void sideBar::collapse( bool p_collapse ) {
        for( auto& i : _mapBanks ) {
            if( i.second ) { i.second->collapse( p_collapse ); }
        }
        if( _addMapBank ) { _addMapBank->collapse( p_collapse ); }
        if( _editTileSet ) { _editTileSet->collapse( p_collapse ); }
        if( _editTrainer ) { _editTrainer->collapse( p_collapse ); }
        if( _editPkmnData ) { _editPkmnData->collapse( p_collapse ); }
        if( _editItemData ) { _editItemData->collapse( p_collapse ); }
        if( _editMoveData ) { _editMoveData->collapse( p_collapse ); }
        if( p_collapse ) {
            auto icon = Gtk::Image( );
            icon.set_from_icon_name( "view-fullscreen-symbolic" );
            _collapseMapBanksButton->set_child( icon );
            _mapBankBarLabel.hide( );
            _sbTileSetBarLabel.hide( );
            _sbDataBarLabel.hide( );
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
            _sbDataBarLabel.show( );
        }
        _mapBankBarCollapsed = p_collapse;
    }

    void sideBar::redraw( ) {
        collapse( _model.m_settings.m_focusMode || _mapBankBarCollapsed );

        for( auto& i : _mapBanks ) {
            if( i.second ) {
                i.second->redraw( );
                if( i.first == _model.selectedBank( ) % DIVE_MAP ) {
                    i.second->select( );
                    i.second->setSelectedMap( _model.selectedMapX( ), _model.selectedMapY( ) );
                } else {
                    i.second->unselect( );
                }
            }
        }
        if( _addMapBank ) { _addMapBank->redraw( ); }
        if( _editTileSet ) {
            _editTileSet->redraw( );
            _editTileSet->setSelectedMap( _model.m_settings.m_tseBS2, _model.m_settings.m_tseBS1 );
            if( _context == CONTEXT_TILE_EDITOR ) {
                _editTileSet->select( );
            } else {
                _editTileSet->unselect( );
            }
        }
        if( _editTrainer ) {
            _editTrainer->redraw( );
            if( _context == CONTEXT_TRAINER_EDITOR ) {
                _editTrainer->select( );
            } else {
                _editTrainer->unselect( );
            }
        }
    }

    void sideBar::reinit( ) {
        for( const auto& [ id, mb ] : _mapBanks ) {
            if( !mb ) { continue; }
            _mapBankBox.remove( *mb );
        }
        _mapBanks.clear( );

        for( const auto& [ id, mb ] : _model.m_fsdata.m_mapBanks ) {
            if( id <= MAX_MAPBANK_NAME ) {
                addNewMapBank( id, mb.getSizeY( ), mb.getSizeX( ), mb.getStatus( ) );
            }
        }
    }

    void sideBar::addNewMapBank( u16 p_bank, u8 p_sizeY, u8 p_sizeX, status p_status ) {
        if( _mapBanks.count( p_bank ) ) { return; }
        // if( !checkOrCreatePath( fs::path( MAP_PATH ) / std::to_string( p_bank ) ) ) {
        //     fprintf( stderr, "[ERROR] Adding map bank %hu failed.\n", p_bank );
        //     return;
        // }

        fprintf( stderr, "[LOG] Adding map bank %hu with initial size %hhu rows, %hhu cols.\n",
                 p_bank, p_sizeY + 1, p_sizeX + 1 );

        auto MB1 = std::make_shared<mapBank>( _model, p_bank, p_sizeX, p_sizeY, p_status );
        MB1->connect( [ this ]( u16 p_bk, u8 p_y, u8 p_x ) {
            if( _model.selectedBank( ) != -1 ) {
                _rootWindow.onUnloadMap( _model.selectedBank( ), _model.selectedMapY( ),
                                         _model.selectedMapX( ) );
            }
            _rootWindow.loadMap( p_bk, p_y, p_x );
        } );
        _mapBanks[ p_bank ] = MB1;

        // keep the list sorted: insert after bank with largest id smaller than p_bank,
        // i.e., after the element before lower_bound( p_bank )
        auto ptr = _mapBanks.lower_bound( p_bank );
        if( ptr == _mapBanks.end( ) || ptr == _mapBanks.begin( ) ) {
            _mapBankBox.insert_child_after( *MB1, *_addMapBank );
        } else {
            --ptr;
            _mapBankBox.insert_child_after( *MB1, *ptr->second );
        }
    }
} // namespace UI
