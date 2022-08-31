#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>

#include "../../pure/util.h"
#include "../../root.h"
#include "eventSelector.h"

namespace UI::MED {
    u8 moveModeFromChoice( u32 p_choice ) {
        if( !p_choice ) { return 0; }

        u8 res = 0;

        for( u8 i{ 0 }; i < 4; ++i ) {
            if( p_choice & ( 1 << i ) ) { res |= ( 1 << i ); }
        }

        if( res ) { return res; }

        for( size_t i{ 4 }; i < DATA::MOVE_MODE_NAMES.size( ); ++i ) {
            if( p_choice & ( 1 << i ) ) { return 16 + i - 4; }
        }

        return 0;
    }

    u32 choiceFromMoveMode( u8 p_moveMode ) {
        if( p_moveMode < 16 ) { return p_moveMode; }

        return 1 << ( 4 + p_moveMode - 16 );
    }

    u32 moveModeTransition( u32 p_state, u8 p_choice ) {
        if( p_state < 16 && p_choice < 4 ) { return p_state ^ ( 1 << p_choice ); }
        return ( p_state & ( 1 << p_choice ) ) ^ ( 1 << p_choice );
    }

    eventSelector::eventSelector( model& p_model, mapEditor& p_parent, root& p_root )
        : _model{ p_model }, _parent{ p_parent }, _rootWindow{ p_root },
          _selectedEventA{ Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_EVENTS_PER_SLICE - 1.0, 1.0,
                                                    1.0, 0.0 ) },
          _aFlagA{ Gtk::Adjustment::create( 0.0, 0.0, 16 * DATA::MAX_FLAG, 1.0, 1.0, 0.0 ) },
          _dFlagA{ Gtk::Adjustment::create( 0.0, 0.0, 16 * DATA::MAX_FLAG, 1.0, 1.0, 0.0 ) },
          _messageIdx1A{ Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) },
          _messageIdx2A{ Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) },
          _warpScriptIdxA{ Gtk::Adjustment::create( 0.0, 0.0, (u8) -1, 1.0, 1.0, 0.0 ) },
          _scriptIdx1A{ Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) },
          _scriptIdx2A{ Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) },
          _flyLocationIdxA{ Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) },
          _berryTreeIdxA{ Gtk::Adjustment::create( 0.0, 0.0, (u8) -1, 1.0, 1.0, 0.0 ) },
          _trainerIdxA{ Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) },
          _trainerSightA{ Gtk::Adjustment::create( 0.0, 0.0, 31, 1.0, 1.0, 0.0 ) },
          _owPkmnLevelA{ Gtk::Adjustment::create( 0.0, 1, 100, 1.0, 1.0, 0.0 ) },
          _selectedEventE{ _selectedEventA }, _aFlagE{ _aFlagA }, _dFlagE{ _dFlagA },
          _messageIdx1E{ _messageIdx1A }, _messageIdx2E{ _messageIdx2A },
          _warpScriptIdxE{ _warpScriptIdxA }, _scriptIdx1E{ _scriptIdx1A },
          _scriptIdx2E{ _scriptIdx2A }, _berryTreeIdxE{ _berryTreeIdxA },
          _trainerIdxE{ _trainerIdxA }, _trainerSightE{ _trainerSightA }, _owPkmnLevelE{
                                                                              _owPkmnLevelA } {
        _mainFrame = Gtk::Frame{ "Event Data" };

        Gtk::Box mainBox{ Gtk::Orientation::VERTICAL };
        auto     swindow = Gtk::ScrolledWindow{ };
        swindow.set_child( mainBox );
        swindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _mainFrame.set_child( swindow );
        _mainFrame.set_margin_start( MARGIN );
        _mainFrame.set_label_align( Gtk::Align::CENTER );

        mainBox.set_vexpand( );
        mainBox.set_margin( MARGIN );

        // general event data (idx number, position, activate, deactivate flag)

        Gtk::Box idxBox{ Gtk::Orientation::HORIZONTAL };
        idxBox.get_style_context( )->add_class( "linked" );

        // evt idx
        idxBox.append( _selectedEventE );
        _selectedEventE.signal_value_changed( ).connect( [ this ]( ) {
            if( _disableRedraw ) { return; }
            _model.selectEvent( _selectedEventE.get_value_as_int( ) );
            redraw( );
            _parent.redrawMap( false );
        } );

        // evt type

        _eventType = std::make_shared<dropDown>( DATA::EVENT_TYPE_NAMES );
        if( _eventType ) {
            idxBox.append( *_eventType );
            ( (Gtk::Widget&) ( *_eventType ) ).set_hexpand( true );

            _eventType->connect( [ this ]( u64 p_newChoice ) {
                if( _disableRedraw ) { return; }
                _model.mapEvent( ).m_type = p_newChoice;
                _model.markSelectedBankChanged( );
                redraw( );
                _parent.redrawMap( false );
                _rootWindow.redrawPanel( );
            } );
        }
        idxBox.set_hexpand( false );

        mainBox.append( idxBox );

        // position widget
        mainBox.append( _generalData );

        Gtk::Frame epos{ "Position" };
        epos.set_margin_top( MARGIN );
        epos.set_label_align( Gtk::Align::CENTER );
        _generalData.append( epos );
        _eventPosition = std::make_shared<mapPosition>( false );
        if( _eventPosition ) {
            epos.set_child( *_eventPosition );
            _eventPosition->connect( [ this ]( ) {
                if( _disableRedraw ) { return; }
                auto& evt  = _model.mapEvent( );
                auto  pos  = _eventPosition->getPosition( ).second;
                evt.m_posX = pos.localX( );
                evt.m_posY = pos.localY( );
                evt.m_posZ = pos.m_posZ;
                _model.markSelectedBankChanged( );
                redraw( );
                _parent.redrawMap( false );
                _rootWindow.redrawPanel( );
            } );
        }

        // activate / deactivated flags
        Gtk::Frame ea{ "Activation" };
        ea.set_margin_top( MARGIN );
        ea.set_label_align( Gtk::Align::CENTER );
        _generalData.append( ea );

        Gtk::Grid g1{ };
        ea.set_child( g1 );
        Gtk::Label afL{ "Activation Flag" };
        g1.attach( afL, 0, 0 );
        afL.set_margin_end( MARGIN );
        afL.set_margin_start( MARGIN );
        afL.set_hexpand( );
        afL.set_halign( Gtk::Align::START );
        _aFlagE.set_margin_end( MARGIN );
        g1.attach( _aFlagE, 1, 0 );

        _aFlagE.signal_value_changed( ).connect( [ this ]( ) {
            if( _disableRedraw
                || _model.mapEvent( ).m_activateFlag == _aFlagE.get_value_as_int( ) ) {
                return;
            }
            _disableAF = true;
            _aFlagE.update( );
            _model.mapEvent( ).m_activateFlag = _aFlagE.get_value_as_int( );
            _model.markSelectedBankChanged( );
            _rootWindow.redrawPanel( );
            _parent.redrawMap( false );
            redraw( );
            _disableAF = false;
        } );

        Gtk::Label dfL{ "Deactivation Flag" };
        g1.attach( dfL, 0, 1 );
        dfL.set_margin_end( MARGIN );
        dfL.set_margin_start( MARGIN );
        dfL.set_hexpand( );
        dfL.set_halign( Gtk::Align::START );
        _dFlagE.set_margin_end( MARGIN );
        g1.attach( _dFlagE, 1, 1 );

        _dFlagE.signal_value_changed( ).connect( [ this ]( ) {
            if( _disableRedraw
                || _model.mapEvent( ).m_deactivateFlag == _dFlagE.get_value_as_int( ) ) {
                return;
            }
            _disableDF = true;
            _dFlagE.update( );
            _model.mapEvent( ).m_deactivateFlag = _dFlagE.get_value_as_int( );
            _model.markSelectedBankChanged( );
            _rootWindow.redrawPanel( );
            _parent.redrawMap( false );
            redraw( );
            _disableDF = false;
        } );

        _eventTrigger = std::make_shared<multiButton>(
            DATA::EVENT_TRIGGER_NAMES,
            []( u32 p_state, u8 p_choice ) { return p_state ^ ( 1 << p_choice ); }, 0,
            Gtk::Orientation::VERTICAL );
        if( _eventTrigger ) {
            g1.attach( *_eventTrigger, 0, 2, 2 );
            _eventTrigger->connect( [ this ]( u32 p_newChoice ) {
                if( _disableRedraw ) { return; }
                auto& evt     = _model.mapEvent( );
                evt.m_trigger = p_newChoice;
                _model.markSelectedBankChanged( );
                redraw( );
                _parent.redrawMap( false );
                _rootWindow.redrawPanel( );
            } );
        }

        g1.set_hexpand( false );

        // event type specific widgets
        {
            Gtk::Frame frame{ "No Event" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            frame.hide( );
            _detailFrames.push_back( std::move( frame ) );
        }

        // message
        // message type, message id
        // message preview
        {
            Gtk::Frame frame{ "Message Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _messageType1 = std::make_shared<dropDown>( DATA::MESSAGE_TYPE_NAMES );
            if( _messageType1 ) {
                fbox.append( *_messageType1 );
                ( (Gtk::Widget&) ( *_messageType1 ) ).set_hexpand( true );

                _messageType1->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_message.m_msgType = p_newChoice;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Message Idx" };
            ilabel.set_halign( Gtk::Align::START );
            ilabel.set_hexpand( );

            fbox.append( ibox );
            ibox.append( ilabel );
            ibox.append( _messageIdx1E );
            _messageIdx1E.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_message.m_msgId
                           == _messageIdx1E.get_value_as_int( ) ) {
                    return;
                }
                _disableMI1E = true;
                _messageIdx1E.update( );
                _model.mapEvent( ).m_data.m_message.m_msgId = _messageIdx1E.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableMI1E = false;
            } );

            _messageLabel1.set_hexpand( );
            _messageLabel1.set_wrap( );
            fbox.append( _messageLabel1 );

            _messageLabel1.set_margin_top( MARGIN );

            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // item
        // item type, item id
        // item name, sprite
        {
            Gtk::Frame frame{ "Item Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _itemType = std::make_shared<dropDown>( DATA::ITEM_TYPE_NAMES );
            if( _itemType ) {
                fbox.append( *_itemType );
                ( (Gtk::Widget&) ( *_itemType ) ).set_hexpand( true );

                _itemType->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_item.m_itemType = p_newChoice;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    _parent.redrawMap( false );
                    redraw( );
                } );
            }

            _item = std::make_shared<itemSelector>( _model );
            if( _item ) {
                fbox.append( *_item );

                _item->connect( [ this ]( u16 p_item ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_item.m_itemId = p_item;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );

                ( (Gtk::Widget&) ( *_item ) ).set_hexpand( true );
                ( (Gtk::Widget&) ( *_item ) ).set_margin_top( MARGIN );
            }
            fbox.set_hexpand( false );

            _detailFrames.push_back( std::move( frame ) );
        }

        // trainer
        {
            Gtk::Frame frame{ "Trainer Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _trainerOWSprite = std::make_shared<owSpriteSelector>( _model, false );
            if( _trainerOWSprite ) {
                fbox.append( *_trainerOWSprite );

                _trainerOWSprite->connect( [ this ]( std::pair<u16, u8> p_sprite ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_trainer.m_spriteId = p_sprite.first;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    _parent.redrawMap( false );
                    redraw( );
                } );

                ( (Gtk::Widget&) ( *_trainerOWSprite ) ).set_hexpand( true );
            }

            _trainerMove = std::make_shared<multiButton>( DATA::MOVE_MODE_NAMES, moveModeTransition,
                                                          0, Gtk::Orientation::VERTICAL );
            if( _trainerMove ) {
                fbox.append( *_trainerMove );
                _trainerMove->connect( [ this ]( u32 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    auto& evt                           = _model.mapEvent( );
                    evt.m_data.m_trainer.m_movementType = moveModeFromChoice( p_newChoice );
                    _model.markSelectedBankChanged( );
                    redraw( );
                    _parent.redrawMap( false );
                    _rootWindow.redrawPanel( );
                } );
            }

            Gtk::Grid  g2{ };
            Gtk::Label tsl{ "Trainer Sight" };
            g2.attach( tsl, 0, 0 );
            tsl.set_margin_end( MARGIN );
            tsl.set_halign( Gtk::Align::START );
            tsl.set_hexpand( );
            g2.attach( _trainerSightE, 1, 0 );

            _trainerSightE.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_trainer.m_sight
                           == _trainerSightE.get_value_as_int( ) ) {
                    return;
                }
                _disableTS = true;
                _trainerSightE.update( );
                _model.mapEvent( ).m_data.m_trainer.m_sight = _trainerSightE.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                _parent.redrawMap( false );
                redraw( );
                _disableTS = false;
            } );

            Gtk::Label til{ "Trainer Idx" };
            g2.attach( til, 0, 1 );
            til.set_margin_end( MARGIN );
            til.set_halign( Gtk::Align::START );
            til.set_hexpand( );
            g2.attach( _trainerIdxE, 1, 1 );

            _trainerIdxE.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_trainer.m_trainerId
                           == _trainerIdxE.get_value_as_int( ) ) {
                    return;
                }
                _disableTI = true;
                _trainerIdxE.update( );
                _model.mapEvent( ).m_data.m_trainer.m_trainerId = _trainerIdxE.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableTI = false;
            } );

            fbox.append( g2 );
            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // OW pkmn
        {
            Gtk::Frame frame{ "Pkmn Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _pkmn = std::make_shared<pokeSelector>( _model );
            if( _pkmn ) {
                fbox.append( *_pkmn );

                _pkmn->connect( [ this ]( std::pair<u16, u8> p_pkmn ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_owPkmn.m_speciesId = p_pkmn.first;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    _parent.redrawMap( false );
                    redraw( );
                } );

                ( (Gtk::Widget&) ( *_pkmn ) ).set_hexpand( true );
            }

            Gtk::Grid  g2{ };
            Gtk::Label til{ "Level" };
            g2.attach( til, 0, 0 );
            til.set_margin_end( MARGIN );
            til.set_halign( Gtk::Align::START );
            til.set_hexpand( );
            g2.attach( _owPkmnLevelE, 1, 0 );

            _owPkmnLevelE.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_owPkmn.m_level
                           == _owPkmnLevelE.get_value_as_int( ) ) {
                    return;
                }
                _disablePL = true;
                _owPkmnLevelE.update( );
                _model.mapEvent( ).m_data.m_owPkmn.m_level = _owPkmnLevelE.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disablePL = false;
            } );

            Gtk::Label tsl{ "Shininess" };
            g2.attach( tsl, 0, 1 );
            tsl.set_halign( Gtk::Align::START );
            tsl.set_margin_end( MARGIN );
            tsl.set_hexpand( );

            _owPkmnShininess = std::make_shared<dropDown>( std::vector<std::string>{
                "Random", "Never", "Forced", "1 Extra Roll", "2 Extra Rolls", "3 Extra Rolls",
                // technically more possible
            } );
            if( _owPkmnShininess ) {
                g2.attach( *_owPkmnShininess, 1, 1 );
                _owPkmnShininess->connect( [ this ]( u8 p_newValue ) {
                    if( _disableRedraw
                        || ( _model.mapEvent( ).m_data.m_owPkmn.m_shiny & ~( 1 << 6 | 1 << 7 ) )
                               == p_newValue ) {
                        return;
                    }
                    _model.mapEvent( ).m_data.m_owPkmn.m_shiny
                        = ( _model.mapEvent( ).m_data.m_owPkmn.m_shiny & ( 1 << 6 | 1 << 7 ) )
                          | p_newValue;
                    _parent.redrawMap( false );
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
                ( (Gtk::Widget&) ( *_owPkmnShininess ) ).set_hexpand( true );
                ( (Gtk::Widget&) ( *_owPkmnShininess ) ).set_margin_top( MARGIN );
            }

            Gtk::Label l3{ "Hidden Ability" };
            g2.attach( l3, 0, 2 );
            l3.set_margin_end( MARGIN );
            l3.set_halign( Gtk::Align::START );
            l3.set_hexpand( );

            _owPkmnHA = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
            if( _owPkmnHA ) {
                g2.attach( *_owPkmnHA, 1, 2 );
                _owPkmnHA->connect( [ this ]( u8 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_owPkmn.m_shiny
                        = ( _model.mapEvent( ).m_data.m_owPkmn.m_shiny & ~( 1 << 6 ) )
                          | ( p_newChoice << 6 );
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
                ( (Gtk::Widget&) ( *_owPkmnHA ) ).set_hexpand( true );
                ( (Gtk::Widget&) ( *_owPkmnHA ) ).set_halign( Gtk::Align::END );
                ( (Gtk::Widget&) ( *_owPkmnHA ) ).set_margin( 0 );
                ( (Gtk::Widget&) ( *_owPkmnHA ) ).set_margin_top( MARGIN );
            }

            Gtk::Label l4{ "Fateful Enc." };
            g2.attach( l4, 0, 3 );
            l4.set_margin_end( MARGIN );
            l4.set_halign( Gtk::Align::START );
            l4.set_hexpand( );

            _owPkmnFE = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
            if( _owPkmnFE ) {
                g2.attach( *_owPkmnFE, 1, 3 );
                _owPkmnFE->connect( [ this ]( u8 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_owPkmn.m_shiny
                        = ( _model.mapEvent( ).m_data.m_owPkmn.m_shiny & ~( 1 << 7 ) )
                          | ( p_newChoice << 7 );
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
                ( (Gtk::Widget&) ( *_owPkmnFE ) ).set_hexpand( true );
                ( (Gtk::Widget&) ( *_owPkmnFE ) ).set_halign( Gtk::Align::END );
                ( (Gtk::Widget&) ( *_owPkmnFE ) ).set_margin( 0 );
                ( (Gtk::Widget&) ( *_owPkmnFE ) ).set_margin_top( MARGIN );
            }

            fbox.append( g2 );
            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // NPC
        {
            Gtk::Frame frame{ "NPC Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _scriptType2 = std::make_shared<dropDown>( DATA::SCRIPT_TYPE_NAMES );
            if( _scriptType2 ) {
                fbox.append( *_scriptType2 );
                ( (Gtk::Widget&) ( *_scriptType2 ) ).set_hexpand( true );

                _scriptType2->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    if( p_newChoice ) {
                        _model.mapEvent( ).m_data.m_npc.m_scriptType = 9 + p_newChoice;
                    } else {
                        _model.mapEvent( ).m_data.m_npc.m_scriptType = 0;
                    }
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }

            _npcOWSprite = std::make_shared<owSpriteSelector>( _model, false );
            if( _npcOWSprite ) {
                fbox.append( *_npcOWSprite );

                _npcOWSprite->connect( [ this ]( std::pair<u16, u8> p_sprite ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_npc.m_spriteId = p_sprite.first;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    _parent.redrawMap( false );
                    redraw( );
                } );

                ( (Gtk::Widget&) ( *_npcOWSprite ) ).set_hexpand( true );
                ( (Gtk::Widget&) ( *_npcOWSprite ) ).set_margin_top( MARGIN );
            }

            _npcMove = std::make_shared<multiButton>( DATA::MOVE_MODE_NAMES, moveModeTransition, 0,
                                                      Gtk::Orientation::VERTICAL );
            if( _npcMove ) {
                fbox.append( *_npcMove );
                _npcMove->connect( [ this ]( u32 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    auto& evt                       = _model.mapEvent( );
                    evt.m_data.m_npc.m_movementType = moveModeFromChoice( p_newChoice );
                    _model.markSelectedBankChanged( );
                    redraw( );
                    _parent.redrawMap( false );
                    _rootWindow.redrawPanel( );
                } );
            }

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Script Idx" };
            ilabel.set_hexpand( );
            ilabel.set_halign( Gtk::Align::START );

            fbox.append( ibox );
            ibox.append( ilabel );
            ibox.append( _scriptIdx1E );
            _scriptIdx1E.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_npc.m_scriptId
                           == _scriptIdx1E.get_value_as_int( ) ) {
                    return;
                }
                _disableSI1E = true;
                _scriptIdx1E.update( );
                _model.mapEvent( ).m_data.m_npc.m_scriptId = _scriptIdx1E.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableSI1E = false;
            } );

            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // warp
        // warp type, target bank, target mx, my, lx, ly, z
        // "go to" button
        {
            Gtk::Frame frame{ "Warp Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _warpType = std::make_shared<dropDown>( DATA::WARP_TYPE_NAMES );
            if( _warpType ) {
                fbox.append( *_warpType );
                ( (Gtk::Widget&) ( *_warpType ) ).set_hexpand( true );

                _warpType->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_warp.m_warpType = p_newChoice;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }
            fbox.set_hexpand( false );

            Gtk::Frame wpos{ "Target" };
            wpos.set_margin_top( MARGIN );
            wpos.set_label_align( Gtk::Align::CENTER );
            fbox.append( wpos );

            Gtk::Box f2box{ Gtk::Orientation::VERTICAL };
            wpos.set_child( f2box );

            _warpScriptIdxBox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Script Idx" };
            ilabel.set_hexpand( );
            ilabel.set_halign( Gtk::Align::START );

            _warpScriptIdxE.set_hexpand( );
            _warpScriptIdxE.set_margin_end( MARGIN );
            _warpScriptIdxE.set_margin_bottom( MARGIN );

            f2box.append( _warpScriptIdxBox );
            _warpScriptIdxBox.append( ilabel );
            _warpScriptIdxBox.append( _warpScriptIdxE );
            _warpScriptIdxE.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_warp.m_posZ
                           == _warpScriptIdxE.get_value_as_int( ) ) {
                    return;
                }
                _disableWSI                             = true;
                _model.mapEvent( ).m_data.m_warp.m_posZ = _warpScriptIdxE.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableWSI = false;
            } );

            _warpTarget = std::make_shared<mapPosition>( true );
            if( _warpTarget ) {
                f2box.append( *_warpTarget );
                _warpTarget->connect( [ this ]( ) {
                    if( _disableRedraw ) { return; }
                    auto& evt                = _model.mapEvent( );
                    auto  pos                = _warpTarget->getPosition( ).second;
                    evt.m_data.m_warp.m_bank = _warpTarget->getPosition( ).first;
                    evt.m_data.m_warp.m_mapX = pos.mapX( );
                    evt.m_data.m_warp.m_mapY = pos.mapY( );

                    evt.m_data.m_warp.m_posX = pos.localX( );
                    evt.m_data.m_warp.m_posY = pos.localY( );
                    evt.m_data.m_warp.m_posZ = pos.m_posZ;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }
            _warpJumpTo = createButton( "", "_Follow Warp", [ this ]( ) {
                if( _warpTarget ) {
                    _rootWindow.loadMap( _warpTarget->getPosition( ).first,
                                         _warpTarget->getPosition( ).second.mapY( ),
                                         _warpTarget->getPosition( ).second.mapX( ) );
                }
            } );
            if( _warpJumpTo ) {
                _warpJumpTo->set_margin_top( MARGIN );
                _warpJumpTo->set_vexpand( false );
                _warpJumpTo->set_hexpand( false );
            }
            fbox.append( *_warpJumpTo );

            _detailFrames.push_back( std::move( frame ) );
        }

        // generic
        // script id, script type
        {
            Gtk::Frame frame{ "Script Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _scriptType1 = std::make_shared<dropDown>( DATA::SCRIPT_TYPE_NAMES );
            if( _scriptType1 ) {
                fbox.append( *_scriptType1 );
                ( (Gtk::Widget&) ( *_scriptType1 ) ).set_hexpand( true );

                _scriptType1->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    if( p_newChoice ) {
                        _model.mapEvent( ).m_data.m_generic.m_scriptType = 9 + p_newChoice;
                    } else {
                        _model.mapEvent( ).m_data.m_generic.m_scriptType = 0;
                    }
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Script Idx" };
            ilabel.set_hexpand( );
            ilabel.set_halign( Gtk::Align::START );

            fbox.append( ibox );
            ibox.append( ilabel );
            ibox.append( _scriptIdx2E );
            _scriptIdx2E.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_generic.m_scriptId
                           == _scriptIdx2E.get_value_as_int( ) ) {
                    return;
                }
                _disableSI2E = true;
                _scriptIdx2E.update( );
                _model.mapEvent( ).m_data.m_generic.m_scriptId = _scriptIdx2E.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableSI2E = false;
            } );

            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // hm object
        // type
        // preview image
        {
            Gtk::Frame frame{ "HM Object Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _hmType = std::make_shared<dropDown>(
                std::vector<std::string>{ "None", "Strength", "Rock Smash", "Cut" } );
            if( _hmType ) {
                fbox.append( *_hmType );
                ( (Gtk::Widget&) ( *_hmType ) ).set_hexpand( true );

                _hmType->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    if( p_newChoice ) {
                        _model.mapEvent( ).m_data.m_hmObject.m_hmType = 2 + p_newChoice;
                    } else {
                        _model.mapEvent( ).m_data.m_hmObject.m_hmType = 0;
                    }
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    _parent.redrawMap( false );
                    redraw( );
                } );
            }

            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // berry tree
        // tree idx
        // (default berry?)
        {
            Gtk::Frame frame{ "Berry Tree Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Internal Tree No." };
            ilabel.set_hexpand( );
            ilabel.set_halign( Gtk::Align::START );

            fbox.append( ibox );
            ibox.append( ilabel );
            ibox.append( _berryTreeIdxE );
            _berryTreeIdxE.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_berryTree.m_treeIdx
                           == _berryTreeIdxE.get_value_as_int( ) ) {
                    return;
                }
                _disableBTI = true;
                _berryTreeIdxE.update( );
                _model.mapEvent( ).m_data.m_berryTree.m_treeIdx
                    = _berryTreeIdxE.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableBTI = false;
            } );

            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // NPC Message
        {
            Gtk::Frame frame{ "NPC/Message Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            _messageType2 = std::make_shared<dropDown>( DATA::MESSAGE_TYPE_NAMES );
            if( _messageType2 ) {
                fbox.append( *_messageType2 );
                ( (Gtk::Widget&) ( *_messageType2 ) ).set_hexpand( true );

                _messageType2->connect( [ this ]( u64 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_npc.m_scriptType = p_newChoice;
                    if( _npcMessageAutoDeact ) {
                        _model.mapEvent( ).m_data.m_npc.m_scriptType
                            |= _npcMessageAutoDeact->currentChoice( ) << 7;
                    }
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }

            _npcMessageAutoDeact = std::make_shared<switchButton>(
                std::vector<std::string>{ "Repeatable", "Auto-Destroy" } );
            if( _npcMessageAutoDeact ) {
                fbox.append( *_npcMessageAutoDeact );
                _npcMessageAutoDeact->connect( [ this ]( u8 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_npc.m_scriptType = p_newChoice << 7;
                    if( _messageType2 ) {
                        _model.mapEvent( ).m_data.m_npc.m_scriptType
                            |= _messageType2->currentChoice( );
                    }
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }

            _npcMessageOWSprite = std::make_shared<owSpriteSelector>( _model, false );
            if( _npcMessageOWSprite ) {
                fbox.append( *_npcMessageOWSprite );

                _npcMessageOWSprite->connect( [ this ]( std::pair<u16, u8> p_sprite ) {
                    if( _disableRedraw ) { return; }
                    _model.mapEvent( ).m_data.m_npc.m_spriteId = p_sprite.first;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    _parent.redrawMap( false );
                    redraw( );
                } );

                ( (Gtk::Widget&) ( *_npcMessageOWSprite ) ).set_hexpand( true );
                ( (Gtk::Widget&) ( *_npcMessageOWSprite ) ).set_margin_top( MARGIN );
            }

            _npcMsgMove = std::make_shared<multiButton>( DATA::MOVE_MODE_NAMES, moveModeTransition,
                                                         0, Gtk::Orientation::VERTICAL );
            if( _npcMsgMove ) {
                fbox.append( *_npcMsgMove );
                _npcMsgMove->connect( [ this ]( u32 p_newChoice ) {
                    if( _disableRedraw ) { return; }
                    auto& evt                       = _model.mapEvent( );
                    evt.m_data.m_npc.m_movementType = moveModeFromChoice( p_newChoice );
                    _model.markSelectedBankChanged( );
                    redraw( );
                    _parent.redrawMap( false );
                    _rootWindow.redrawPanel( );
                } );
            }

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Message Idx" };
            ilabel.set_hexpand( );
            ilabel.set_halign( Gtk::Align::START );

            fbox.append( ibox );
            ibox.append( ilabel );
            ibox.append( _messageIdx2E );
            _messageIdx2E.signal_value_changed( ).connect( [ this ]( ) {
                if( _disableRedraw
                    || _model.mapEvent( ).m_data.m_npc.m_scriptId
                           == _messageIdx2E.get_value_as_int( ) ) {
                    return;
                }
                _disableMI2E = true;
                _messageIdx2E.update( );
                _model.mapEvent( ).m_data.m_npc.m_scriptId = _messageIdx2E.get_value_as_int( );
                _model.markSelectedBankChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
                _disableMI2E = false;
            } );

            _messageLabel2.set_hexpand( );
            _messageLabel2.set_wrap( );
            fbox.append( _messageLabel2 );

            _messageLabel2.set_margin_top( MARGIN );

            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }

        // fly position
        {
            Gtk::Frame frame{ "FlyPos Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            Gtk::Box fbox{ Gtk::Orientation::VERTICAL };
            fbox.set_margin( MARGIN );
            frame.set_child( fbox );

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Fly Location" };
            ilabel.set_hexpand( );
            ilabel.set_halign( Gtk::Align::START );

            fbox.append( ibox );
            ibox.append( ilabel );

            _flyLocation = std::make_shared<locationDropDown>( );
            if( _flyLocation ) {
                ( (Gtk::Widget&) *_flyLocation ).set_hexpand( true );
                ( (Gtk::Widget&) *_flyLocation ).set_vexpand( false );
                ibox.append( *_flyLocation );
                _flyLocation->connect( [ this ]( u64 p_newChoice ) {
                    if( _model.mapEvent( ).m_data.m_flyPos.m_location == p_newChoice ) { return; }
                    _model.mapEvent( ).m_data.m_flyPos.m_location = p_newChoice;
                    _model.markSelectedBankChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }
            fbox.set_hexpand( false );
            _detailFrames.push_back( std::move( frame ) );
        }
    }

    void eventSelector::redraw( ) {
        _disableRedraw = true;
        auto evt       = _model.mapEvent( );

        _selectedEventE.set_value( _model.selectedMapEvent( ) );

        if( _eventType ) { _eventType->choose( (u8) evt.m_type ); }
        for( auto& f : _detailFrames ) { f.hide( ); }

        if( !_eventType || !_eventType->currentChoice( ) ) {
            // not a real event, hide everything
            _generalData.hide( );
        } else {
            _generalData.show( );
            _detailFrames[ evt.m_type ].show( );
        }

        switch( evt.m_type ) {
        case DATA::EVENT_MESSAGE: {
            if( _messageType1 ) { _messageType1->choose( evt.m_data.m_message.m_msgType ); }

            if( !_disableMI1E ) { _messageIdx1E.set_value( evt.m_data.m_message.m_msgId ); }
            _messageLabel1.set_text( _model.getMapString( evt.m_data.m_message.m_msgId ) );
            break;
        }
        case DATA::EVENT_ITEM: {
            if( _itemType ) { _itemType->choose( evt.m_data.m_item.m_itemType ); }
            if( _item ) {
                _item->refreshModel( );
                _item->setData( evt.m_data.m_item.m_itemId );
            }
            break;
        }
        case DATA::EVENT_TRAINER: {
            if( !_disableTI ) { _trainerIdxE.set_value( evt.m_data.m_trainer.m_trainerId ); }
            if( !_disableTS ) { _trainerSightE.set_value( evt.m_data.m_trainer.m_sight ); }
            if( _trainerOWSprite ) {
                _trainerOWSprite->setData(
                    evt.m_data.m_trainer.m_spriteId,
                    DATA::moveModeToFrame(
                        DATA::moveMode( evt.m_data.m_trainer.m_movementType ),
                        DATA::frameFuncionForIdx( evt.m_data.m_trainer.m_spriteId ) ) );
            }

            if( _trainerMove ) {
                _trainerMove->choose( choiceFromMoveMode( evt.m_data.m_trainer.m_movementType ) );
            }
            break;
        }
        case DATA::EVENT_OW_PKMN: {
            if( _pkmn ) {
                _pkmn->refreshModel( );
                _pkmn->setData( { evt.m_data.m_owPkmn.m_speciesId,
                                  evt.m_data.m_owPkmn.m_forme & ~( 1 << 6 | 1 << 7 ) } );
            }
            if( !_disablePL ) {
                _owPkmnLevelE.set_value( _model.mapEvent( ).m_data.m_owPkmn.m_level );
            }
            if( _owPkmnShininess ) {
                _owPkmnShininess->choose( _model.mapEvent( ).m_data.m_owPkmn.m_shiny
                                          & ~( 1 << 6 | 1 << 7 ) );
            }
            if( _owPkmnHA ) {
                _owPkmnHA->choose( !!( _model.mapEvent( ).m_data.m_owPkmn.m_shiny & ( 1 << 6 ) ) );
            }
            if( _owPkmnFE ) {
                _owPkmnFE->choose( !!( _model.mapEvent( ).m_data.m_owPkmn.m_shiny & ( 1 << 7 ) ) );
            }
            break;
        }
        case DATA::EVENT_NPC: {
            if( _scriptType2 ) {
                if( !( evt.m_data.m_npc.m_scriptType & 127 ) ) {
                    _scriptType2->choose( 0 );
                } else {
                    _scriptType2->choose( ( evt.m_data.m_npc.m_scriptType & 127 ) - 9 );
                }
            }
            if( !_disableSI1E ) { _scriptIdx1E.set_value( evt.m_data.m_npc.m_scriptId ); }
            if( _npcOWSprite ) {
                _npcOWSprite->setData(
                    evt.m_data.m_npc.m_spriteId,
                    DATA::moveModeToFrame(
                        DATA::moveMode( evt.m_data.m_npc.m_movementType ),
                        DATA::frameFuncionForIdx( evt.m_data.m_npc.m_spriteId ) ) );
            }

            if( _npcMove ) {
                _npcMove->choose( choiceFromMoveMode( evt.m_data.m_npc.m_movementType ) );
            }

            break;
        }
        case DATA::EVENT_WARP: {
            if( _warpType ) { _warpType->choose( evt.m_data.m_warp.m_warpType ); }
            if( evt.m_data.m_warp.m_warpType == DATA::SCRIPT ) {
                if( _warpTarget ) { ( (Gtk::Widget&) *_warpTarget ).hide( ); }
                _warpJumpTo->hide( );
                _warpScriptIdxBox.show( );
            } else {
                if( _warpTarget ) { ( (Gtk::Widget&) *_warpTarget ).show( ); }
                _warpJumpTo->show( );
                _warpScriptIdxBox.hide( );
            }
            if( _warpTarget ) {
                auto tmpos = DATA::warpPos{
                    evt.m_data.m_warp.m_bank,
                    DATA::position::fromLocal( evt.m_data.m_warp.m_mapX, evt.m_data.m_warp.m_posX,
                                               evt.m_data.m_warp.m_mapY, evt.m_data.m_warp.m_posY,
                                               evt.m_data.m_warp.m_posZ ) };
                _warpTarget->setPosition( tmpos );
            }
            if( !_disableWSI ) { _warpScriptIdxE.set_value( evt.m_data.m_warp.m_posZ ); }
            break;
        }
        case DATA::EVENT_GENERIC: {
            if( _scriptType1 ) {
                if( !( evt.m_data.m_generic.m_scriptType & 127 ) ) {
                    _scriptType1->choose( 0 );
                } else {
                    _scriptType1->choose( ( evt.m_data.m_generic.m_scriptType & 127 ) - 9 );
                }
            }
            if( !_disableSI2E ) { _scriptIdx2E.set_value( evt.m_data.m_generic.m_scriptId ); }
            break;
        }
        case DATA::EVENT_HMOBJECT: {
            if( _hmType ) {
                switch( evt.m_data.m_hmObject.m_hmType ) {
                case 3: // strength
                    _hmType->choose( 1 );
                    break;
                case 4: // rock smash
                    _hmType->choose( 2 );
                    break;
                case 5: // cut
                    _hmType->choose( 3 );
                    break;
                default: _hmType->choose( 0 ); break;
                }
            }
            break;
        }
        case DATA::EVENT_BERRYTREE: {
            if( !_disableBTI ) { _berryTreeIdxE.set_value( evt.m_data.m_berryTree.m_treeIdx ); }
            break;
        }
        case DATA::EVENT_NPC_MESSAGE: {
            if( _messageType2 ) { _messageType2->choose( evt.m_data.m_npc.m_scriptType & 127 ); }

            if( !_disableMI2E ) { _messageIdx2E.set_value( evt.m_data.m_npc.m_scriptId ); }
            _messageLabel2.set_text( _model.getMapString( evt.m_data.m_npc.m_scriptId ) );

            if( _npcMessageAutoDeact ) {
                _npcMessageAutoDeact->choose( evt.m_data.m_npc.m_scriptType >> 7 );
            }

            if( _npcMessageOWSprite ) {
                _npcMessageOWSprite->setData(
                    evt.m_data.m_npc.m_spriteId,
                    DATA::moveModeToFrame(
                        DATA::moveMode( evt.m_data.m_npc.m_movementType ),
                        DATA::frameFuncionForIdx( evt.m_data.m_npc.m_spriteId ) ) );
            }

            if( _npcMsgMove ) {
                _npcMsgMove->choose( choiceFromMoveMode( evt.m_data.m_npc.m_movementType ) );
            }

            break;
        }
        case DATA::EVENT_FLY_POS: {
            if( _flyLocation ) {
                _flyLocation->refreshModel( _model );
                _flyLocation->choose( evt.m_data.m_flyPos.m_location );
            }
            break;
        }
        default: break;
        }

        if( _eventPosition ) { _eventPosition->setPosition( _model.mapEventPosition( ) ); }
        if( !_disableAF ) { _aFlagE.set_value( evt.m_activateFlag ); }
        if( !_disableDF ) { _dFlagE.set_value( evt.m_deactivateFlag ); }

        if( _eventTrigger ) { _eventTrigger->choose( evt.m_trigger ); }

        _disableRedraw = false;
    }
} // namespace UI::MED
