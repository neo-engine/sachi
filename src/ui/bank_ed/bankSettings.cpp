#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>

#include "../../log.h"
#include "../pure/util.h"
#include "../root.h"
#include "bankSettings.h"

namespace UI {
    bankSettings::bankSettings( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root },
          _shiftXA{ Gtk::Adjustment::create( 0.0, 0.0, 255.0, 1.0, 5.0, 0.0 ) },
          _shiftYA{ Gtk::Adjustment::create( 0.0, 0.0, 191.0, 1.0, 5.0, 0.0 ) },
          _wpshiftXA{ Gtk::Adjustment::create( 0.0, 0.0, 255.0, 1.0, 5.0, 0.0 ) },
          _wpshiftYA{ Gtk::Adjustment::create( 0.0, 0.0, 191.0, 1.0, 5.0, 0.0 ) },
          _shiftXE{ _shiftXA }, _shiftYE{ _shiftYA }, _wpshiftXE{ _wpshiftXA },
          _wpshiftYE{ _wpshiftYA } {

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

        _mapBankSettingsMapModeToggles = std::make_shared<switchButton>(
            std::vector<std::string>{ "_Simple", "Sc_attered", "_Combined" } );

        if( _mapBankSettingsMapModeToggles ) {
            ( (Gtk::Widget&) ( *_mapBankSettingsMapModeToggles ) ).set_margin_end( 0 );
            ( (Gtk::Widget&) ( *_mapBankSettingsMapModeToggles ) ).set_margin_top( 0 );
            shbox1.set_end_widget( *_mapBankSettingsMapModeToggles );
            _mapBankSettingsMapModeToggles->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _model.setCurrentBankOWStatus( p_newChoice );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( shbox1 );

        _shbox2.set_hexpand( true );
        auto shbox2l = Gtk::Label( "Bank is Overworld" );
        _shbox2.set_start_widget( shbox2l );

        _mapIsOW = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
        if( _mapIsOW ) {
            ( (Gtk::Widget&) ( *_mapIsOW ) ).set_margin_end( 0 );
            ( (Gtk::Widget&) ( *_mapIsOW ) ).set_margin_top( 0 );
            _shbox2.set_end_widget( *_mapIsOW );
            _mapIsOW->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                if( _model.bank( ).getOWStatus( ) == p_newChoice ) { return; }
                _model.bank( ).setOWStatus( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( _shbox2 );

        _shbox3.set_hexpand( true );
        auto shbox3l = Gtk::Label( "Default Overworld Location" );
        _shbox3.set_start_widget( shbox3l );
        _bankDefaultLocation
            = std::make_shared<numberedStringCacheDropDown>( Gtk::Orientation::HORIZONTAL );
        if( _bankDefaultLocation ) {
            ( (Gtk::Widget&) ( *_bankDefaultLocation ) ).set_halign( Gtk::Align::END );
            _shbox3.set_end_widget( *_bankDefaultLocation );
            _bankDefaultLocation->connect( [ this ]( u64 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                if( _model.bank( ).getDefaultLocation( ) == p_newChoice ) { return; }
                _model.bank( ).setDefaultLocation( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        sboxv1.append( _shbox3 );

        _shbox4.set_hexpand( true );
        auto shbox4l = Gtk::Label( "Bank has Underwater" );
        _shbox4.set_start_widget( shbox4l );

        _mapHasDive = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
        if( _mapHasDive ) {
            ( (Gtk::Widget&) ( *_mapHasDive ) ).set_margin_end( 0 );
            ( (Gtk::Widget&) ( *_mapHasDive ) ).set_margin_top( 0 );
            ( (Gtk::Widget&) ( *_mapHasDive ) ).set_margin_bottom( 0 );
            _shbox4.set_end_widget( *_mapHasDive );
            _mapHasDive->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                if( _model.bank( ).getDiveStatus( ) == p_newChoice ) { return; }
                _model.bank( ).setDiveStatus( p_newChoice );
                _model.markSelectedBankChanged( );
                _rootWindow.redraw( );
            } );
        }
        _shbox4.set_margin_top( MARGIN );
        sboxv1.append( _shbox4 );

        _mapSettingsBox.append( shbox1f1 );

        // ow map
        _owMapFrame = Gtk::Frame{ "Overworld Map" };
        _owMapFrame.set_label_align( Gtk::Align::CENTER );
        _owMapFrame.set_margin_top( MARGIN );
        Gtk::Box hboxn1{ Gtk::Orientation::HORIZONTAL };
        hboxn1.append( _owMapFrame );
        _mapSettingsBox.append( hboxn1 );

        auto sboxv2 = Gtk::Box( Gtk::Orientation::VERTICAL );
        _owMapFrame.set_child( sboxv2 );

        // map image
        Gtk::Overlay ov{ };
        ov.set_child( _owImage );
        ov.add_overlay( _navBorder );
        ov.add_overlay( _touchArea );
        ov.set_hexpand( );
        ov.set_valign( Gtk::Align::CENTER );
        sboxv2.append( ov );
        _owImage.setScale( 1 );
        _navBorder.setScale( 1 );
        _touchArea.get_style_context( )->add_class( "mapblock-selected" );
        ov.set_halign( Gtk::Align::CENTER );

        {
            auto sboxh2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            sboxv2.append( sboxh2 );
            sboxh2.set_expand( false );

            _recomputeMapBG = createButton( "", "Recompute OW Image", [ this ]( ) {
                _model.recomputeBankPic( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            } );
            if( _recomputeMapBG ) {
                sboxh2.append( *_recomputeMapBG );
                _recomputeMapBG->set_hexpand( true );
                _recomputeMapBG->set_vexpand( false );
                _recomputeMapBG->set_margin( MARGIN );
                _recomputeMapBG->set_margin_end( 0 );
                _recomputeMapBG->set_margin_top( 0 );
            }

            _recomputeOverlay = createButton( "", "Add Location Overlay", [ this ]( ) {
                _model.recomputeBankLocationOverlay( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            } );
            if( _recomputeOverlay ) {
                sboxh2.append( *_recomputeOverlay );
                _recomputeOverlay->set_hexpand( true );
                _recomputeOverlay->set_vexpand( false );
                _recomputeOverlay->set_margin( MARGIN );
                _recomputeOverlay->set_margin_top( 0 );
            }
        }

        _showNavBorder = std::make_shared<switchButton>(
            std::vector<std::string>{ "_Hide Border", "_Show Border" }, 1, true );
        if( _showNavBorder ) {
            ( (Gtk::Widget&) ( *_showNavBorder ) ).set_margin_top( 0 );
            ( (Gtk::Widget&) ( *_showNavBorder ) ).set_margin_bottom( MARGIN );
            sboxv2.append( *_showNavBorder );
            _showNavBorder->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _navBorder.set_visible( p_newChoice );
            } );
        }
        _showTouchArea = std::make_shared<switchButton>(
            std::vector<std::string>{ "_Hide Touch Area", "_Show Touch Area" }, 1, true );
        if( _showTouchArea ) {
            ( (Gtk::Widget&) ( *_showTouchArea ) ).set_margin_top( 0 );
            ( (Gtk::Widget&) ( *_showTouchArea ) ).set_margin_bottom( MARGIN );
            sboxv2.append( *_showTouchArea );
            _showTouchArea->connect( [ this ]( u8 p_newChoice ) {
                if( _model.selectedBank( ) == -1 ) { return; }
                _touchArea.set_visible( p_newChoice );
            } );
        }

        // xshift, yshift
        {
            auto sboxh2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            sboxv2.append( sboxh2 );

            {
                auto bx5 = Gtk::CenterBox( );
                bx5.set_hexpand( true );
                auto shbox1l = Gtk::Label( "Margin: Top " );
                bx5.set_margin_end( MARGIN );
                bx5.set_margin_bottom( MARGIN );
                shbox1l.set_margin_start( MARGIN );
                bx5.set_start_widget( shbox1l );

                bx5.set_end_widget( _shiftYE );
                _shiftYE.signal_changed( ).connect( [ this ]( ) {
                    if( _model.selectedBank( ) == -1
                        || _shiftYE.get_value( ) == _model.bank( ).m_mapImageShiftY ) {
                        return;
                    }
                    _disableSB                      = true;
                    _model.bank( ).m_mapImageShiftY = _shiftYE.get_value( );

                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                    _disableSB = false;
                } );

                sboxh2.append( bx5 );
            }

            {
                auto bx5 = Gtk::CenterBox( );
                bx5.set_hexpand( true );
                auto shbox1l = Gtk::Label( " Left " );
                bx5.set_margin_end( MARGIN );
                bx5.set_margin_bottom( MARGIN );
                shbox1l.set_margin_start( MARGIN );
                bx5.set_start_widget( shbox1l );

                bx5.set_end_widget( _shiftXE );
                _shiftXE.signal_changed( ).connect( [ this ]( ) {
                    if( _model.selectedBank( ) == -1
                        || _shiftXE.get_value( ) == _model.bank( ).m_mapImageShiftX ) {
                        return;
                    }
                    _disableSB                      = true;
                    _model.bank( ).m_mapImageShiftX = _shiftXE.get_value( );

                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                    _disableSB = false;
                } );

                sboxh2.append( bx5 );
            }
        }

        // location color selector

        // ow poke map
        _wpMapFrame = Gtk::Frame{ "Dex-Wild PKMN Map" };
        _wpMapFrame.set_label_align( Gtk::Align::CENTER );
        _wpMapFrame.set_margin_top( MARGIN );
        _wpMapFrame.set_margin_start( MARGIN );
        hboxn1.append( _wpMapFrame );

        auto sboxv3 = Gtk::Box( Gtk::Orientation::VERTICAL );
        _wpMapFrame.set_child( sboxv3 );

        // map image
        Gtk::Overlay ov2{ };
        ov2.set_child( _wpImage );
        ov2.add_overlay( _dexBorder );
        ov2.set_hexpand( );
        ov2.set_valign( Gtk::Align::CENTER );
        sboxv3.append( ov2 );
        _wpImage.setScale( 1 );
        _dexBorder.setScale( 1 );
        ov2.set_halign( Gtk::Align::CENTER );

        {
            auto sboxh2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            sboxv3.append( sboxh2 );
            sboxh2.set_expand( false );

            _recomputeWildPokeBG = createButton( "", "Recompute Dex Wild PKMN Image", [ this ]( ) {
                _model.recomputeDexWPPic( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            } );
            if( _recomputeWildPokeBG ) {
                sboxh2.append( *_recomputeWildPokeBG );
                _recomputeWildPokeBG->set_hexpand( true );
                _recomputeWildPokeBG->set_vexpand( false );
                _recomputeWildPokeBG->set_margin( MARGIN );
                _recomputeWildPokeBG->set_margin_top( 0 );
            }
        }

        {
            _showDexBorder = std::make_shared<switchButton>(
                std::vector<std::string>{ "_Hide Border", "_Show Border" }, 1, true );
            if( _showDexBorder ) {
                ( (Gtk::Widget&) ( *_showDexBorder ) ).set_margin_top( 0 );
                ( (Gtk::Widget&) ( *_showDexBorder ) ).set_margin_bottom( MARGIN );
                sboxv3.append( *_showDexBorder );
                _showDexBorder->connect( [ this ]( u8 p_newChoice ) {
                    if( _model.selectedBank( ) == -1 ) { return; }
                    _dexBorder.set_visible( p_newChoice );
                } );
            }
        }

        // xshift, yshift
        {
            auto sboxh2 = Gtk::Box( Gtk::Orientation::HORIZONTAL );
            sboxv3.append( sboxh2 );
            {
                auto bx5 = Gtk::CenterBox( );
                bx5.set_hexpand( true );
                auto shbox1l = Gtk::Label( "Margin: Top " );
                bx5.set_margin_end( MARGIN );
                bx5.set_margin_bottom( MARGIN );
                shbox1l.set_margin_start( MARGIN );
                bx5.set_start_widget( shbox1l );

                bx5.set_end_widget( _wpshiftYE );
                _wpshiftYE.signal_changed( ).connect( [ this ]( ) {
                    if( _model.selectedBank( ) == -1
                        || _wpshiftYE.get_value( ) == _model.bank( ).m_wildPokeMapShiftY ) {
                        return;
                    }
                    _disableSB                         = true;
                    _model.bank( ).m_wildPokeMapShiftY = _wpshiftYE.get_value( );

                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                    _disableSB = false;
                } );

                sboxh2.append( bx5 );
            }
            {
                auto bx5 = Gtk::CenterBox( );
                bx5.set_hexpand( true );
                auto shbox1l = Gtk::Label( " Left " );
                bx5.set_margin_end( MARGIN );
                bx5.set_margin_bottom( MARGIN );
                shbox1l.set_margin_start( MARGIN );
                bx5.set_start_widget( shbox1l );

                bx5.set_end_widget( _wpshiftXE );
                _wpshiftXE.signal_changed( ).connect( [ this ]( ) {
                    if( _model.selectedBank( ) == -1
                        || _wpshiftXE.get_value( ) == _model.bank( ).m_wildPokeMapShiftX ) {
                        return;
                    }
                    _disableSB                         = true;
                    _model.bank( ).m_wildPokeMapShiftX = _wpshiftXE.get_value( );

                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                    _disableSB = false;
                } );

                sboxh2.append( bx5 );
            }
        }
    }

    void bankSettings::redraw( ) {
        if( _model.selectedBank( ) == -1 ) { return; }
        _owMapFrame.hide( );
        _wpMapFrame.hide( );

        if( _mapBankSettingsMapModeToggles ) {
            _mapBankSettingsMapModeToggles->choose( _model.bank( ).getMapMode( ) );
        }

        if( _model.selectedBankIsDive( ) ) {
            _shbox2.hide( );
            _shbox3.hide( );
            _shbox4.hide( );
        } else {
            _shbox2.show( );
            _shbox4.show( );

            if( _mapIsOW ) { _mapIsOW->choose( _model.bank( ).getOWStatus( ) ); }
            if( _mapHasDive ) { _mapHasDive->choose( _model.bank( ).getDiveStatus( ) ); }
            if( _bankDefaultLocation ) {
                _bankDefaultLocation->refreshModel( _model.locationNames( ) );
                _bankDefaultLocation->choose( _model.bank( ).getDefaultLocation( ) );
            }
            if( _model.bank( ).getOWStatus( ) ) {
                // draw the ow map, default location selector
                _shbox3.show( );
                _owMapFrame.show( );
                _wpMapFrame.show( );
                _navBorder.load( _model.m_fsdata.navBorderPath( ).c_str( ) );
                _owImage.load( _model.bank( ).m_owMap );
                _dexBorder.load( _model.m_fsdata.dexBorderPath( ).c_str( ) );
                _wpImage.load( _model.bank( ).m_wpMap );
                _touchArea.set_valign( Gtk::Align::START );
                _touchArea.set_halign( Gtk::Align::START );
                _touchArea.set_size_request(
                    ( DATA::MAP_LOCATION_RES / _model.bank( ).m_mapImageRes )
                        * _model.bank( ).getSizeX( ) * 3,
                    ( DATA::MAP_LOCATION_RES / _model.bank( ).m_mapImageRes )
                        * _model.bank( ).getSizeY( ) * 3 );
                _touchArea.set_margin_top( 16 + _model.bank( ).m_mapImageShiftY * 1 );
                _touchArea.set_margin_start( 32 + _model.bank( ).m_mapImageShiftX * 1 );
                if( !_disableSB ) {
                    _shiftXE.set_value( _model.bank( ).m_mapImageShiftX );
                    _shiftYE.set_value( _model.bank( ).m_mapImageShiftY );
                    _wpshiftXE.set_value( _model.bank( ).m_wildPokeMapShiftX );
                    _wpshiftYE.set_value( _model.bank( ).m_wildPokeMapShiftY );
                }
            } else {
                _shbox3.hide( );
            }
        }
    }

} // namespace UI
