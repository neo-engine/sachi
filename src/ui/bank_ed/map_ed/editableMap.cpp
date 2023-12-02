#include <queue>
#include "../../../defines.h"
#include "../../pure/mapSlice.h"
#include "../../root.h"
#include "editableMap.h"

namespace UI::MED {
    editableMap::editableMap( model& p_model, root& p_root, mapEditor& p_parent )
        : _model{ p_model }, _rootWindow{ p_root }, _mapEditor{ p_parent } {

        _mainWindow = Gtk::ScrolledWindow( );
        _mainWindow.set_child( _mapGrid );
        _mapGrid.set_halign( Gtk::Align::CENTER );
        _mapGrid.set_valign( Gtk::Align::CENTER );

        for( u8 x{ 0 }; x < 3; ++x ) {
            _currentMap.push_back( std::vector<lookupMapSlice>( 3 ) );
            for( u8 y{ 0 }; y < 3; ++y ) {
                if( x == 1 && y == 1 ) {
                    // use overlay for center map to attach sprites, etc
                    _centerMapOverlay.set_child( _currentMap[ x ][ y ] );
                    _mapGrid.attach( _centerMapOverlay, x, y );
                } else {
                    _mapGrid.attach( _currentMap[ x ][ y ], x, y );
                }
                _currentMap[ x ][ y ].connectClick(
                    [ this, x, y ]( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapClicked( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                      x == 1 && y == 1 );
                    } );
                _currentMap[ x ][ y ].connectDrag(
                    [ this, x, y ]( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapDragStart( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                        x == 1 && y == 1 );
                    },
                    [ this, x, y ]( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapDragUpdate( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                         x == 1 && y == 1 );
                    },
                    [ this, x, y ]( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY ) {
                        onMapDragEnd( p_button, p_blockX, p_blockY, s8( x ) - 1, s8( y ) - 1,
                                      x == 1 && y == 1 );
                    } );
            }
        }

        _mapGrid.set_row_spacing( _model.m_settings.m_neighborSpacing );
        _mapGrid.set_column_spacing( _model.m_settings.m_neighborSpacing );

        _mainWindow.set_expand( );

        _blockStamp = std::make_shared<blockStamp>( p_model, p_root, p_parent );

        _centerMapOverlay.add_overlay( _locationGrid );
        _locationGrid.set_column_homogeneous( );
        _locationGrid.set_expand( );
        _locationGrid.get_style_context( )->add_class( "linked" );

        size_t scale = DATA::SIZE / DATA::MAP_LOCATION_RES;
        // init locations
        for( u8 x{ 0 }; x < scale; ++x ) {
            _locations.push_back( { } );
            for( u8 y{ 0 }; y < scale; ++y ) {
                auto l = std::make_shared<numberedStringCacheDropDown>( );
                l->connect( [ this, x, y ]( u64 p_newChoice ) {
                    if( _model.mapData( ).m_locationIds[ y ][ x ] == p_newChoice ) { return; }
                    _model.mapData( ).m_locationIds[ y ][ x ] = p_newChoice;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( false );
                } );
                _locations[ x ].push_back( l );

                _locationGrid.attach( *l, x, y );
            }
        }
    }

    void editableMap::setNewMapEditMode( mapEditor::mapDisplayMode p_newMode ) {
        _currentMapDisplayMode = p_newMode;
        redraw( );
        if( _blockStamp ) { _blockStamp->setNewMapEditMode( p_newMode ); }
    }

    void editableMap::updateSelectedBlock( DATA::mapBlockAtom p_block ) {
        _model.updateSelectedBlock( p_block );
        _mapEditor.updateSelectedBlock( );
    }

    void editableMap::redraw( bool p_full ) {
        if( _model.selectedBank( ) == -1 ) { return; }

        _mapGrid.set_row_spacing( _model.m_settings.m_neighborSpacing );
        _mapGrid.set_column_spacing( _model.m_settings.m_neighborSpacing );

        const auto& mb    = _model.bank( );
        const auto& mbank = mb.m_bank.m_slices;

        auto mapX{ _model.selectedMapX( ) }, mapY{ _model.selectedMapY( ) };

        for( s8 x{ -1 }; x <= 1; ++x ) {
            for( s8 y{ -1 }; y <= 1; ++y ) {
                bool empty{ false };
                if( not( mapY + y >= 0 && mapY + y <= mb.getSizeY( ) && mapX + x >= 0
                         && mapX + x <= mb.getSizeX( ) ) ) {
                    empty = true;
                }

                u8 mapwd = DATA::SIZE, maphg = DATA::SIZE;
                if( x ) { mapwd = _model.m_settings.m_adjacentBlocks; }
                if( y ) { maphg = _model.m_settings.m_adjacentBlocks; }

                if( !mapwd || !maphg ) {
                    _currentMap[ x + 1 ][ y + 1 ].hide( );
                    continue;
                } else {
                    _currentMap[ x + 1 ][ y + 1 ].show( );
                }

                if( p_full ) {
                    auto filtered = std::vector<DATA::mapBlockAtom>( );
                    for( u8 y2{ 0 }; y2 < DATA::SIZE; ++y2 ) {
                        for( u8 x2{ 0 }; x2 < DATA::SIZE; ++x2 ) {
                            bool addx{ false }, addy{ false };
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
                                        mbank[ mapY + y ][ mapX + x ].m_data.m_blocks[ y2 ][ x2 ] );
                                }
                            }
                        }
                    }

                    _currentMap[ x + 1 ][ y + 1 ].set(
                        filtered,
                        [ this ]( DATA::mapBlockAtom p_block ) {
                            return _mapEditor.blockSetLookup( p_block.m_blockidx );
                        },
                        mapwd );
                }

                _currentMap[ x + 1 ][ y + 1 ].setSpacing( _model.m_settings.m_blockSpacing );
                _currentMap[ x + 1 ][ y + 1 ].setScale( _model.m_settings.m_blockScale );

                _currentMap[ x + 1 ][ y + 1 ].setOverlayHidden( _currentMapDisplayMode
                                                                != mapEditor::MODE_EDIT_MOVEMENT );
                if( p_full ) { _currentMap[ x + 1 ][ y + 1 ].draw( ); }
                _currentMap[ x + 1 ][ y + 1 ].queue_resize( );
            }
        }

        if( _currentMapDisplayMode == mapEditor::MODE_EDIT_LOCATIONS ) {
            _locationGrid.show( );
            // update model for locations
            size_t scale = DATA::SIZE / DATA::MAP_LOCATION_RES;
            for( u8 x{ 0 }; x < scale; ++x ) {
                for( u8 y{ 0 }; y < scale; ++y ) {
                    if( !_locations[ x ][ y ] ) { continue; }

                    _locations[ x ][ y ]->refreshModel( _model.locationNames( ) );
                    _locations[ x ][ y ]->choose( _model.mapData( ).m_locationIds[ y ][ x ] );
                    ( (Gtk::Widget&) *_locations[ x ][ y ] ).set_opacity( .65 );
                    ( (Gtk::Widget&) *_locations[ x ][ y ] ).set_expand( true );
                }
            }
        } else {
            _locationGrid.hide( );
        }

        if( _blockStamp ) { _blockStamp->redraw( ); }

        if( _currentMapDisplayMode == mapEditor::MODE_EDIT_EVENTS ) {
            // add selection box for currently selected event
            _currentMap[ 1 ][ 1 ].clearMarks( );
            _currentMap[ 1 ][ 1 ].setMarksHidden( false );
            _currentMap[ 1 ][ 1 ].setMarksOpacity( .7 );
            auto evt = _model.mapEvent( );
            if( evt.m_type ) {
                _currentMap[ 1 ][ 1 ].selectBlock( evt.m_posX, evt.m_posY );
            } else {
                _currentMap[ 1 ][ 1 ].selectBlock( -1 );
            }

            for( auto i : _eventWidgets ) {
                if( i && i->is_visible( ) ) {
                    _centerMapOverlay.remove_overlay( *i );
                    i->hide( );
                }
            }
            //            _eventWidgets.clear( );

            for( u16 i = 0; i < DATA::MAX_EVENTS_PER_SLICE; ++i ) {
                auto& evt = _model.mapData( ).m_events[ i ];

                auto tx = evt.m_posX * DATA::BLOCK_SIZE * _model.m_settings.m_blockScale
                          + evt.m_posX * _model.m_settings.m_blockSpacing;
                auto ty = evt.m_posY * DATA::BLOCK_SIZE * _model.m_settings.m_blockScale
                          + evt.m_posY * _model.m_settings.m_blockSpacing;

                switch( evt.m_type ) {
                case DATA::EVENT_MESSAGE: {
                    _currentMap[ 1 ][ 1 ].addMark( evt.m_posX, evt.m_posY, mapSlice::MARK_MESSAGE );
                    break;
                }
                case DATA::EVENT_ITEM: {
                    auto itm = std::make_shared<fsImage<imageType::IT_SPRITE_ANIMATED>>( );

                    itm->set_can_target( false );

                    itm->load( _model.m_fsdata.owSpritePath(
                                   256 | ( 248 + evt.m_data.m_item.m_itemType ) ),
                               0, 8, 16, 16, 16 );

                    itm->set_margin_top( ty );
                    itm->set_margin_start( tx );
                    itm->set_valign( Gtk::Align::START );
                    itm->set_halign( Gtk::Align::START );
                    itm->setScale( _model.m_settings.m_blockScale );
                    if( evt.m_data.m_item.m_itemType == 0 ) { itm->set_opacity( .3 ); }

                    _centerMapOverlay.add_overlay( *itm );
                    _eventWidgets.push_back( std::move( itm ) );

                    break;
                }
                case DATA::EVENT_TRAINER: {
                    auto itm = std::make_shared<fsImage<imageType::IT_SPRITE_ANIMATED>>( );
                    itm->set_can_target( false );

                    auto sid = evt.m_data.m_trainer.m_spriteId;
                    if( sid < DATA::PKMN_SPRITE ) { sid |= 256; }

                    itm->load( _model.m_fsdata.owSpritePath( sid ),
                               DATA::moveModeToFrame(
                                   DATA::moveMode( evt.m_data.m_trainer.m_movementType ),
                                   DATA::frameFuncionForIdx( sid ) ) );

                    itm->set_margin_top( ty - 16 * _model.m_settings.m_blockScale );
                    itm->set_margin_start( tx - 8 * _model.m_settings.m_blockScale );
                    itm->set_valign( Gtk::Align::START );
                    itm->set_halign( Gtk::Align::START );
                    itm->setScale( _model.m_settings.m_blockScale );

                    if( evt.m_activateFlag || evt.m_deactivateFlag ) { itm->set_opacity( 0.5 ); }

                    _centerMapOverlay.add_overlay( *itm );
                    _eventWidgets.push_back( std::move( itm ) );

                    // add movement
                    s8 sx = evt.m_posX;
                    s8 sy = evt.m_posY;
                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_MOVEMENT );
                    if( evt.m_data.m_trainer.m_movementType < 16 ) {
                        if( evt.m_data.m_trainer.m_movementType & DATA::LOOK_UP ) {
                            for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                sx = evt.m_posX + 0;
                                sy = evt.m_posY - 1 * st;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                        }
                        if( evt.m_data.m_trainer.m_movementType & DATA::LOOK_DOWN ) {
                            for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                sx = evt.m_posX + 0;
                                sy = evt.m_posY + 1 * st;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                        }
                        if( evt.m_data.m_trainer.m_movementType & DATA::LOOK_LEFT ) {
                            for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                sx = evt.m_posX - 1 * st;
                                sy = evt.m_posY + 0;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                        }
                        if( evt.m_data.m_trainer.m_movementType & DATA::LOOK_RIGHT ) {
                            for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                sx = evt.m_posX + 1 * st;
                                sy = evt.m_posY + 0;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                        }

                    } else {
                        switch( evt.m_data.m_trainer.m_movementType ) {
                        default: break;
                        case DATA::WALK_AROUND_SQUARE: {
                            for( s8 ssx{ -1 }; ssx <= 1; ++ssx ) {
                                for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                    sx = evt.m_posX + ssx;
                                    sy = evt.m_posY - 1 * st;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                                for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                    sx = evt.m_posX + ssx;
                                    sy = evt.m_posY + 1 * st;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                            }
                            for( s8 ssy{ -1 }; ssy <= 1; ++ssy ) {
                                for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                    sx = evt.m_posX - 1 * st;
                                    sy = evt.m_posY + ssy;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                                for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                    sx = evt.m_posX + 1 * st;
                                    sy = evt.m_posY + ssy;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                            }
                            for( s8 ssx{ -1 }; ssx <= 1; ++ssx ) {
                                for( s8 ssy{ -1 }; ssy <= 1; ++ssy ) {
                                    sx = evt.m_posX + ssx;
                                    sy = evt.m_posY + ssy;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_MOVEMENT );
                                    }
                                }
                            }

                            break;
                        }

                        case DATA::WALK_AROUND_LEFT_RIGHT: {
                            for( s8 ssx{ -1 }; ssx <= 1; ++ssx ) {
                                for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                    sx = evt.m_posX + ssx;
                                    sy = evt.m_posY - 1 * st;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                                for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                    sx = evt.m_posX + ssx;
                                    sy = evt.m_posY + 1 * st;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                            }

                            [[fallthrough]];
                        }
                        case DATA::WALK_LEFT_RIGHT:
                        case DATA::WALK_CONT_LEFT_RIGHT: {
                            // pos left
                            sx = evt.m_posX - 1;
                            sy = evt.m_posY + 0;
                            if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_MOVEMENT );
                            }
                            for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                sx = evt.m_posX - 1 * st;
                                sy = evt.m_posY + 0;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                            // pos right
                            sx = evt.m_posX + 1;
                            sy = evt.m_posY + 0;
                            if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_MOVEMENT );
                            }
                            for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                sx = evt.m_posX + 1 * st;
                                sy = evt.m_posY + 0;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                            break;
                        }
                        case DATA::WALK_AROUND_UP_DOWN: {
                            for( s8 ssy{ -1 }; ssy <= 1; ++ssy ) {
                                for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                    sx = evt.m_posX - 1 * st;
                                    sy = evt.m_posY + ssy;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                                for( u8 st{ 1 }; st <= evt.m_data.m_trainer.m_sight; ++st ) {
                                    sx = evt.m_posX + 1 * st;
                                    sy = evt.m_posY + ssy;
                                    if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                        _currentMap[ 1 ][ 1 ].addMark( sx, sy,
                                                                       mapSlice::MARK_SIGHT );
                                    }
                                }
                            }

                            [[fallthrough]];
                        }
                        case DATA::WALK_UP_DOWN:
                        case DATA::WALK_CONT_UP_DOWN: {
                            // pos up
                            sx = evt.m_posX + 0;
                            sy = evt.m_posY - 1;
                            if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_MOVEMENT );
                            }
                            for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                sx = evt.m_posX + 0;
                                sy = evt.m_posY - 1 * st;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                            // pos down
                            sx = evt.m_posX + 0;
                            sy = evt.m_posY + 1;
                            if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_MOVEMENT );
                            }
                            for( u8 st{ 2 }; st <= evt.m_data.m_trainer.m_sight + 1; ++st ) {
                                sx = evt.m_posX + 0;
                                sy = evt.m_posY + 1 * st;
                                if( sx >= 0 && sy >= 0 && sx < DATA::SIZE && sy < DATA::SIZE ) {
                                    _currentMap[ 1 ][ 1 ].addMark( sx, sy, mapSlice::MARK_SIGHT );
                                }
                            }
                            break;
                        }
                        }
                    }
                    break;
                }
                case DATA::EVENT_OW_PKMN: {
                    auto itm = std::make_shared<fsImage<imageType::IT_SPRITE_ANIMATED>>( );
                    itm->set_can_target( false );

                    itm->load( _model.m_fsdata.owSpritePath(
                                   DATA::PKMN_SPRITE + evt.m_data.m_owPkmn.m_speciesId,
                                   evt.m_data.m_owPkmn.m_forme & ~( 1 << 6 | 1 << 7 ),
                                   ( evt.m_data.m_owPkmn.m_shiny & 0x3f ) == 2 ),
                               0 );

                    if( itm->getWidth( ) < 64 ) {
                        itm->set_margin_top( ty - 16 * _model.m_settings.m_blockScale );
                        itm->set_margin_start( tx - 8 * _model.m_settings.m_blockScale );
                    } else {
                        itm->set_margin_top( ty - 48 * _model.m_settings.m_blockScale );
                        itm->set_margin_start( tx - 24 * _model.m_settings.m_blockScale );
                    }
                    itm->set_valign( Gtk::Align::START );
                    itm->set_halign( Gtk::Align::START );
                    itm->setScale( _model.m_settings.m_blockScale );

                    _centerMapOverlay.add_overlay( *itm );
                    _eventWidgets.push_back( std::move( itm ) );

                    break;
                }
                case DATA::EVENT_NPC_MESSAGE:
                case DATA::EVENT_NPC: {
                    auto itm = std::make_shared<fsImage<imageType::IT_SPRITE_ANIMATED>>( );
                    itm->set_can_target( false );

                    auto sid = evt.m_data.m_npc.m_spriteId;
                    if( sid < DATA::PKMN_SPRITE ) { sid |= 256; }
                    itm->load(
                        _model.m_fsdata.owSpritePath( sid ),
                        DATA::moveModeToFrame( DATA::moveMode( evt.m_data.m_npc.m_movementType ),
                                               DATA::frameFuncionForIdx( sid ) ) );

                    itm->set_margin_top( ty - 16 * _model.m_settings.m_blockScale );
                    itm->set_margin_start( tx - 8 * _model.m_settings.m_blockScale );
                    itm->set_valign( Gtk::Align::START );
                    itm->set_halign( Gtk::Align::START );
                    itm->setScale( _model.m_settings.m_blockScale );

                    if( evt.m_activateFlag || evt.m_deactivateFlag ) { itm->set_opacity( 0.5 ); }

                    _centerMapOverlay.add_overlay( *itm );
                    _eventWidgets.push_back( std::move( itm ) );

                    break;
                }
                case DATA::EVENT_WARP: {
                    _currentMap[ 1 ][ 1 ].addMark( evt.m_posX, evt.m_posY, mapSlice::MARK_WARP );
                    break;
                }
                case DATA::EVENT_GENERIC: {
                    if( evt.m_trigger && evt.m_trigger != DATA::TRIGGER_ON_MAP_ENTER ) {
                        _currentMap[ 1 ][ 1 ].addMark( evt.m_posX, evt.m_posY,
                                                       mapSlice::MARK_SCRIPT );
                    }
                    break;
                }
                case DATA::EVENT_HMOBJECT: {
                    if( evt.m_data.m_hmObject.m_hmType == 0 ) {
                        break; // none
                    }
                    auto itm = std::make_shared<fsImage<imageType::IT_SPRITE_ANIMATED>>( );
                    itm->set_can_target( false );

                    itm->load( _model.m_fsdata.owSpritePath(
                                   256 | ( 248 + evt.m_data.m_hmObject.m_hmType ) ),
                               0, 8, 16, 16, 16 );

                    itm->set_margin_top( ty );
                    itm->set_margin_start( tx );
                    itm->set_valign( Gtk::Align::START );
                    itm->set_halign( Gtk::Align::START );
                    itm->setScale( _model.m_settings.m_blockScale );

                    _centerMapOverlay.add_overlay( *itm );
                    _eventWidgets.push_back( std::move( itm ) );

                    break;
                }
                case DATA::EVENT_BERRYTREE: {
                    _currentMap[ 1 ][ 1 ].addMark( evt.m_posX, evt.m_posY, mapSlice::MARK_BERRY );
                    break;
                }
                case DATA::EVENT_FLY_POS: {
                    _currentMap[ 1 ][ 1 ].addMark( evt.m_posX, evt.m_posY, mapSlice::MARK_FLYPOS );
                    break;
                }
                default: break;
                }
            }
        } else {
            _currentMap[ 1 ][ 1 ].selectBlock( -1 );
            for( auto& i : _eventWidgets ) { i->hide( ); }
            _currentMap[ 1 ][ 1 ].setMarksHidden( true );
        }
    }

    void editableMap::onMapDragStart( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                                      s8 p_mapX, s8 p_mapY, bool ) {
        _dragStart = { p_blockX, p_blockY, p_mapX, p_mapY };
        _dragLast  = { p_blockX, p_blockY };
        if( p_button == mapSlice::clickType::RIGHT ) {
            // reset blockStamp
            if( _blockStamp ) { _blockStamp->reset( ); }
        }
    }

    void editableMap::onMapDragUpdate( mapSlice::clickType p_button, s16 p_dX, s16 p_dY, s8 p_mapX,
                                       s8 p_mapY, bool p_allowEdit ) {

        auto [ sx, sy, _1, _2 ] = _dragStart;
        auto [ lx, ly ]         = _dragLast;
        (void) _1;
        (void) _2;

        auto blockwd
            = _model.m_settings.m_blockScale * DATA::BLOCK_SIZE + _model.m_settings.m_blockSpacing;
        auto nx = sx + ( p_dX / blockwd );
        auto ny = sy + ( p_dY / blockwd );

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
            if( _currentMapDisplayMode == mapEditor::MODE_EDIT_TILES
                || _currentMapDisplayMode == mapEditor::MODE_EDIT_MOVEMENT ) {

                // update and show the stamp box

                if( _blockStamp ) { _blockStamp->create( ); }

                // Check if the stamp should go across map borders
                if( p_mapX == 1 ) {
                    if( nx < -DATA::SIZE - _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( nx < -DATA::SIZE ) {
                        // crossing two borders
                        nx = sx
                             + ( p_dX + 2 * _model.m_settings.m_neighborSpacing
                                 + 2 * _model.m_settings.m_blockSpacing )
                                   / blockwd
                             - 1;
                    } else if( nx < 0 ) {
                        // crossing one border
                        nx = sx
                             + ( p_dX + _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd
                             - 1;
                    } else if( nx >= _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else if( p_mapX == 0 ) {
                    if( nx < -_model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( nx < 0 ) {
                        // crossing one border
                        nx = sx
                             + ( p_dX + _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd
                             - 1;
                    } else if( nx >= DATA::SIZE ) {
                        // crossing one border
                        nx = sx
                             + ( p_dX - _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd;
                    } else if( nx >= DATA::SIZE + _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else {
                    if( nx < 0 ) {
                        // out of bounds
                        return;
                    } else if( nx >= _model.m_settings.m_adjacentBlocks ) {
                        // crossing one border
                        nx = sx
                             + ( p_dX - _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd;
                    } else if( nx >= DATA::SIZE + _model.m_settings.m_adjacentBlocks ) {
                        // crossing two borders
                        nx = sx
                             + ( p_dX - 2 * _model.m_settings.m_neighborSpacing
                                 + 2 * _model.m_settings.m_blockSpacing )
                                   / blockwd;
                    } else if( nx >= DATA::SIZE + 2 * _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                }
                if( p_mapY == 1 ) {
                    if( ny < -DATA::SIZE - _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( ny < -DATA::SIZE ) {
                        // crossing two borders
                        ny = sy
                             + ( p_dY + 2 * _model.m_settings.m_neighborSpacing
                                 - 2 * _model.m_settings.m_blockSpacing )
                                   / blockwd
                             - 1;
                    } else if( ny < 0 ) {
                        // crossing one border
                        ny = sy
                             + ( p_dY + _model.m_settings.m_neighborSpacing
                                 - _model.m_settings.m_blockSpacing )
                                   / blockwd
                             - 1;
                    } else if( ny >= _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else if( p_mapY == 0 ) {
                    if( ny < -_model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    } else if( ny < 0 ) {
                        // crossing one border
                        ny = sy
                             + ( p_dY + _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd
                             - 1;
                    } else if( ny >= DATA::SIZE ) {
                        // crossing one border
                        ny = sy
                             + ( p_dY - _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd;
                    } else if( ny >= DATA::SIZE + _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                } else {
                    if( ny < 0 ) {
                        // out of bounds
                        return;
                    } else if( ny >= _model.m_settings.m_adjacentBlocks ) {
                        // crossing one border
                        ny = sy
                             + ( p_dY - _model.m_settings.m_neighborSpacing
                                 + _model.m_settings.m_blockSpacing )
                                   / blockwd;
                    } else if( ny >= DATA::SIZE + _model.m_settings.m_adjacentBlocks ) {
                        // crossing two borders
                        ny = sy
                             + ( p_dY - 2 * _model.m_settings.m_neighborSpacing
                                 + 2 * _model.m_settings.m_blockSpacing )
                                   / blockwd;
                    } else if( ny >= DATA::SIZE + 2 * _model.m_settings.m_adjacentBlocks ) {
                        // out of bounds
                        return;
                    }
                }

                _dragLast = { nx, ny };
                if( nx == lx && ny == ly ) { return; }

                bool revx{ nx < sx };
                bool revy{ ny < sy };
                s16  posx = sx, posy = sy;

                auto tmpMap = std::deque<std::deque<DATA::mapBlockAtom>>( );
                auto bswd{ 0 };

                while( 1 ) {
                    if( revy && posy < ny ) { break; }
                    if( !revy && posy > ny ) { break; }

                    // build a new row
                    auto row = std::deque<DATA::mapBlockAtom>( );
                    bswd     = 0;
                    posx     = sx;
                    while( 1 ) {
                        if( revx && posx < nx ) { break; }
                        if( !revx && posx > nx ) { break; }

                        s8  mx{ p_mapX }, my{ p_mapY };
                        s16 remx{ posx }, remy{ posy };

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
                                    remx += _model.m_settings.m_adjacentBlocks;
                                    mx--;
                                } else {
                                    remx -= DATA::SIZE;
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
                                    remy += _model.m_settings.m_adjacentBlocks;
                                    my--;
                                } else {
                                    remy -= DATA::SIZE;
                                    my++;
                                }
                            } else {
                                if( remy < 0 ) [[unlikely]] {
                                    break;
                                } else {
                                    remy -= _model.m_settings.m_adjacentBlocks;
                                    my++;
                                }
                            }
                        }
                        // compute block

                        u16 xcorr = 0, ycorr = 0;
                        if( mx < 0 ) { xcorr = DATA::SIZE - _model.m_settings.m_adjacentBlocks; }
                        if( my < 0 ) { ycorr = DATA::SIZE - _model.m_settings.m_adjacentBlocks; }

                        DATA::mapBlockAtom currentBlock;
                        if( _model.selectedMapY( ) + mx < 0 || _model.selectedMapX( ) + my < 0
                            || _model.selectedMapX( ) + mx > _model.selectedSizeX( )
                            || _model.selectedMapY( ) + my > _model.selectedSizeY( ) ) {
                            // out of map bank bounds, add a blank block
                            currentBlock = { 0, 1 };
                        } else {
                            auto& mp     = _model.slice( );
                            currentBlock = mp.m_data.m_blocks[ remy + ycorr ][ remx + xcorr ];
                        }

                        if( revx ) {
                            row.push_front( currentBlock );
                            posx--;
                        } else {
                            row.push_back( currentBlock );
                            posx++;
                        }
                        bswd++;
                    }

                    if( revy ) {
                        tmpMap.push_front( row );
                        posy--;
                    } else {
                        tmpMap.push_back( row );
                        posy++;
                    }
                }

                auto tmpData = std::vector<DATA::mapBlockAtom>( );
                for( auto row : tmpMap ) {
                    for( auto block : row ) { tmpData.push_back( block ); }
                }

                if( _blockStamp ) { _blockStamp->update( tmpData, bswd ); }
            }
        }
    }

    void editableMap::onMapDragEnd( mapSlice::clickType /* p_button */, s16 /* p_dX */,
                                    s16 /* p_dY */, s8 /* p_mapX */, s8 /* p_mapY */,
                                    bool /* p_allowEdit */ ) {
    }

    void editableMap::onMapClicked( mapSlice::clickType p_button, u16 p_blockX, u16 p_blockY,
                                    s8 p_mapX, s8 p_mapY, bool p_allowEdit ) {

        // fprintf( stderr, "onMapClicked butto %hu bx: %hu by: %hu, %hhi %hhi %hhu\n", p_button,
        //         p_blockX, p_blockY, p_mapX, p_mapY, p_allowEdit );

        // compute block
        u16 xcorr = 0, ycorr = 0;
        if( p_mapX < 0 ) { xcorr = DATA::SIZE - _model.m_settings.m_adjacentBlocks; }
        if( p_mapY < 0 ) { ycorr = DATA::SIZE - _model.m_settings.m_adjacentBlocks; }

        if( _model.selectedMapY( ) + p_mapY < 0 || _model.selectedMapX( ) + p_mapX < 0
            || _model.selectedMapX( ) + p_mapX > _model.selectedSizeX( )
            || _model.selectedMapY( ) + p_mapY > _model.selectedSizeY( ) ) {
            if( p_button == mapSlice::clickType::RIGHT ) { updateSelectedBlock( { 0, 1 } ); }
            return;
        }

        auto& mp    = _model.slice( _model.selectedBank( ), _model.selectedMapY( ) + p_mapY,
                                    _model.selectedMapX( ) + p_mapX );
        auto& block = mp.m_data.m_blocks[ p_blockY + ycorr ][ p_blockX + xcorr ];

        switch( p_button ) {
        case mapSlice::clickType::LEFT:
            if( p_allowEdit ) {
                if( _currentMapDisplayMode == mapEditor::MODE_EDIT_TILES ) {
                    // check if there is a valid block stamp
                    if( _blockStamp && _blockStamp->isValid( ) ) {
                        // paste the block stamp
                        auto bx{ p_blockX + xcorr }, by{ p_blockY + ycorr };
                        for( size_t y{ 0 }; y < _blockStamp->sizeY( ); ++y ) {
                            for( size_t x{ 0 }; x < _blockStamp->sizeX( ); ++x ) {
                                if( bx + x < DATA::SIZE && by + y < DATA::SIZE ) {
                                    mp.m_data.m_blocks[ by + y ][ bx + x ]
                                        = _blockStamp->at( x, y );
                                    _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock(
                                        _blockStamp->at( x, y ), bx + x, by + y );
                                    _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                                        _blockStamp->at( x, y ).m_movedata, bx + x, by + y );
                                }
                            }
                        }
                    } else {
                        block.m_blockidx = _model.m_settings.m_currentlySelectedBlock.m_blockidx;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock( block, p_blockX,
                                                                             p_blockY );
                    }
                } else if( _currentMapDisplayMode == mapEditor::MODE_EDIT_MOVEMENT ) {
                    if( _blockStamp && _blockStamp->isValid( ) ) {
                        // paste the block stamp
                        auto bx{ p_blockX + xcorr }, by{ p_blockY + ycorr };
                        for( size_t y{ 0 }; y < _blockStamp->sizeY( ); ++y ) {
                            for( size_t x{ 0 }; x < _blockStamp->sizeX( ); ++x ) {
                                if( bx + x < DATA::SIZE && by + y < DATA::SIZE ) {
                                    mp.m_data.m_blocks[ by + y ][ bx + x ].m_movedata
                                        = _blockStamp->at( x, y ).m_movedata;
                                    _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                                        _blockStamp->at( x, y ).m_movedata, bx + x, by + y );
                                }
                            }
                        }
                    } else {
                        block.m_movedata = _model.m_settings.m_currentlySelectedBlock.m_movedata;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                            block.m_movedata, p_blockX, p_blockY );
                    }
                } else if( _currentMapDisplayMode == mapEditor::MODE_EDIT_EVENTS ) {
                    // move currently selected event to position that was clicked on
                    auto  bx{ p_blockX + xcorr }, by{ p_blockY + ycorr };
                    auto& evt = _model.mapEvent( );
                    if( evt.m_type ) {
                        evt.m_posX = bx;
                        evt.m_posY = by;
                        evt.m_posZ = block.m_movedata / 4;
                        if( !evt.m_posZ ) { evt.m_posZ = 3; }
                        _rootWindow.redraw( );
                    }
                }
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
            } else {
                updateSelectedBlock( block );
            }
            break;
        case mapSlice::clickType::RIGHT:
            if( _currentMapDisplayMode == mapEditor::MODE_EDIT_TILES
                || _currentMapDisplayMode == mapEditor::MODE_EDIT_MOVEMENT ) {
                updateSelectedBlock( block );
            } else if( _currentMapDisplayMode == mapEditor::MODE_EDIT_EVENTS ) {
                // select event at current position
                auto bx{ p_blockX + xcorr }, by{ p_blockY + ycorr };

                for( u8 ev{ 0 }; ev < DATA::MAX_EVENTS_PER_SLICE; ++ev ) {
                    if( _model.mapData( ).m_events[ ev ].m_posX == bx
                        && _model.mapData( ).m_events[ ev ].m_posY == by ) {
                        _model.selectEvent( ev );
                        _rootWindow.redraw( );
                        break;
                    }
                }
            }

            break;
        case mapSlice::clickType::MIDDLE:
            if( p_allowEdit ) {
                DATA::mapBlockAtom oldb{ block };
                // flood fill
                auto bqueue = std::queue<std::pair<s16, s16>>( );
                bqueue.push( { p_blockY, p_blockX } );
                while( !bqueue.empty( ) ) {
                    auto [ cy, cx ] = bqueue.front( );
                    bqueue.pop( );

                    if( cx < 0 || cx >= DATA::SIZE || cy < 0 || cy >= DATA::SIZE ) { continue; }

                    if( _currentMapDisplayMode == mapEditor::MODE_EDIT_TILES ) {
                        if( mp.m_data.m_blocks[ cy ][ cx ].m_blockidx != oldb.m_blockidx ) {
                            continue;
                        }
                        mp.m_data.m_blocks[ cy ][ cx ].m_blockidx
                            = _model.m_settings.m_currentlySelectedBlock.m_blockidx;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlock( block, cx, cy );
                    } else if( _currentMapDisplayMode == mapEditor::MODE_EDIT_MOVEMENT ) {
                        if( mp.m_data.m_blocks[ cy ][ cx ].m_movedata != oldb.m_movedata ) {
                            continue;
                        }
                        mp.m_data.m_blocks[ cy ][ cx ].m_movedata
                            = _model.m_settings.m_currentlySelectedBlock.m_movedata;
                        _currentMap[ p_mapX + 1 ][ p_mapY + 1 ].updateBlockMovement(
                            block.m_movedata, cx, cy );
                    }

                    for( s8 i{ -1 }; i <= 1; ++i ) {
                        for( s8 j{ -1 }; j <= 1; ++j ) {
                            if( cy + i < 0 || cy + i >= DATA::SIZE || cx + j < 0
                                || cx + j >= DATA::SIZE ) {
                                continue;
                            }
                            bqueue.push( { cy + i, cx + j } );
                        }
                    }
                }
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
            } else {
                updateSelectedBlock( block );
            }
            break;
        default: break;
        }
    }

} // namespace UI::MED
