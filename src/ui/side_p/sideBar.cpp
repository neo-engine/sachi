// #include <cstring>

#include <gtkmm/scrolledwindow.h>

#include "../../defines.h"
#include "../util.h"
#include "sideBar.h"

namespace UI {
    sideBar::sideBar( ) {
        // main box
        _lMainBox = Gtk::Box( Gtk::Orientation::VERTICAL, MARGIN );

        _collapseMapBanksButton
            = createButton( "view-restore-symbolic", "_Collapse Sidebar",
                            [ & ]( ) { this->collapse( !_mapBankBarCollapsed ); } );

        _collapseMapBanksButton->set_expand( true );
        _collapseMapBanksButton->set_margin( MARGIN );
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
            // tile set
            auto lFrame = Gtk::Frame( );
            lFrame.set_margin( MARGIN );
            lFrame.set_margin_top( 0 );
            lFrame.set_margin_bottom( 0 );
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

            _editTileSet = std::make_shared<editTileSet>( );
            if( _editTileSet ) { _sbTileSetBox.append( *_editTileSet ); }

            _sbTileSetSel1 = 0;
            _sbTileSetSel2 = 1;
        }
        {
            // map banks
            auto lScrolledWindow = Gtk::ScrolledWindow( );
            _lMainBox.append( lScrolledWindow );
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

            if( _addMapBank ) { _mapBankBox.append( *_addMapBank ); }

            _mapBanks.clear( );
            _selectedBank = -1;
        }
    }

    void sideBar::collapse( bool p_collapse ) {
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

    void sideBar::markTileSetsChanged( mapBank::status p_newStatus ) {
        if( _editTileSet ) { _editTileSet->setStatus( p_newStatus ); }
    }

    void sideBar::markBankChanged( u16 p_bank, mapBank::status p_newStatus ) {
        if( !_mapBanks.count( p_bank ) ) { return; }
        if( _mapBanks[ p_bank ].m_widget ) {
            _mapBanks[ p_bank ].m_widget->setStatus( p_newStatus );
        }
    }

} // namespace UI
