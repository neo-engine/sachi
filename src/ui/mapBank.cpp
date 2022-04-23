#include <gtkmm/image.h>

#include "../defines.h"
#include "mapBank.h"

namespace UI {
    mapBank::mapBank( const std::string& p_yicon, const std::string& p_xicon )
        : _mapXAdj( Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 5.0, 0.0 ) ),
          _mapYAdj( Gtk::Adjustment::create( 0.0, 0.0, 0.0, 1.0, 5.0, 0.0 ) ),
          _mapXEntry( _mapXAdj ), _mapYEntry( _mapYAdj ) {
        _loadMapButton.set_size_request( 70, 25 );
        _nameBox.append( _nameLabel );

        auto mainBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto box3    = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto box4    = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        mainBox.get_style_context( )->add_class( "linked" );
        _entryBox.get_style_context( )->add_class( "linked" );
        box3.get_style_context( )->add_class( "linked" );
        box4.get_style_context( )->add_class( "linked" );

        mainBox.set_expand( false );

        _outerFrame.set_child( mainBox );

        mainBox.append( _nameBox );
        mainBox.set_valign( Gtk::Align::CENTER );

        _nameLabel.set_size_request( 40, 25 );

        auto YL = Gtk::Label( );
        auto YI = Gtk::Image( );
        YI.set_from_icon_name( p_yicon );
        YI.set_margin( MARGIN );
        box3.append( YI );
        box3.append( _mapYEntry );
        box3.append( YL );
        _mapYEntry.set_width_chars( 2 );
        _mapYEntry.set_max_width_chars( 2 );
        _mapYEntry.set_numeric( true );

        auto XI = Gtk::Image( );
        auto XL = Gtk::Label( );
        XI.set_from_icon_name( p_xicon );
        XI.set_margin( MARGIN );
        box4.append( XI );
        box4.append( _mapXEntry );
        box4.append( XL );
        _mapXEntry.set_width_chars( 2 );
        _mapXEntry.set_max_width_chars( 2 );
        _mapXEntry.set_numeric( true );

        _entryBox.append( box3 );
        _entryBox.append( box4 );
        mainBox.append( _entryBox );

        mainBox.append( _loadMapButton );
    }

    mapBank::mapBank( u16 p_bankName, u8 p_sizeX, u8 p_sizeY, status p_initialStatus )
        : mapBank( ) {
        setBankName( p_bankName );
        setSizeX( p_sizeX );
        setSizeY( p_sizeY );
        setStatus( p_initialStatus );
    }

    addMapBank::addMapBank( )
        //: mapBank( "go-bottom", "go-last" ),
        : mapBank( "object-flip-vertical-symbolic", "object-flip-horizontal-symbolic" ),
          _nameAdj( Gtk::Adjustment::create( 10.0, 10.0, MAX_MAPBANK_NAME, 1.0, 10.0, 0.0 ) ),
          _nameEntry( _nameAdj ) {
        _nameLabel.set_markup( "<span size=\"xx-large\" weight=\"bold\">+</span>" );
        _loadMapButton.set_label( "Add" );

        auto box1 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        auto NI   = Gtk::Image( );
        auto NL   = Gtk::Label( );
        NI.set_from_icon_name( "document-new-symbolic" );
        NI.set_margin( MARGIN );
        box1.append( NI );
        box1.append( _nameEntry );
        box1.append( NL );
        _nameEntry.set_width_chars( 2 );
        _nameEntry.set_max_width_chars( 2 );
        _nameEntry.set_numeric( true );

        _entryBox.insert_child_at_start( box1 );
        setStatus( STATUS_NEW );
        setSizeX( MAX_MAPBANK_NAME );
        setSizeY( MAX_MAPBANK_NAME );
    }

    void mapBank::select( ) {
        _selected = true;
        _outerFrame.add_css_class( "mapbank-selected" );
    }
    void mapBank::unselect( ) {
        _selected = false;
        _outerFrame.remove_css_class( "mapbank-selected" );
    }

    editTileSet::editTileSet( ) : mapBank( "pan-up-symbolic", "pan-down-symbolic" ) {
        _nameLabel.set_markup( "<span size=\"x-large\" weight=\"bold\">TS</span>" );
        _loadMapButton.set_label( "Edit" );

        _mapXAdj->set_lower( 0 );
        _mapYAdj->set_lower( 0 );
        setSizeX( MAX_TILESET_NAME );
        setSizeY( MAX_TILESET_NAME );
    }

} // namespace UI
