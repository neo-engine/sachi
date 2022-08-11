#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>

#include "../../pure/util.h"
#include "../../root.h"
#include "eventSelector.h"

namespace UI::MED {
    eventSelector::eventSelector( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root },
          _selectedEventA( Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_EVENTS_PER_SLICE - 1.0, 1.0,
                                                    1.0, 0.0 ) ),
          _aFlagA( Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_FLAG, 1.0, 1.0, 0.0 ) ),
          _dFlagA( Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_FLAG, 1.0, 1.0, 0.0 ) ),
          _messageIdx1A( Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) ),
          _warpScriptIdxA( Gtk::Adjustment::create( 0.0, 0.0, (u8) -1, 1.0, 1.0, 0.0 ) ),
          _scriptIdx1A( Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) ),
          _scriptIdx2A( Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) ),
          _flyLocationIdxA( Gtk::Adjustment::create( 0.0, 0.0, (u16) -1, 1.0, 1.0, 0.0 ) ),
          _selectedEventE( _selectedEventA ), _aFlagE( _aFlagA ), _dFlagE( _dFlagA ),
          _messageIdx1E( _messageIdx1A ), _warpScriptIdxE( _warpScriptIdxA ),
          _scriptIdx1E( _scriptIdx1A ), _scriptIdx2E( _scriptIdx2A ) {
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
            _rootWindow.redraw( );
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
                _rootWindow.redraw( );
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
                _rootWindow.redraw( );
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
        _aFlagE.set_margin_end( MARGIN );
        g1.attach( _aFlagE, 1, 0 );

        Gtk::Label dfL{ "Deactivation Flag" };
        g1.attach( dfL, 0, 1 );
        dfL.set_margin_end( MARGIN );
        dfL.set_margin_start( MARGIN );
        dfL.set_hexpand( );
        _dFlagE.set_margin_end( MARGIN );
        g1.attach( _dFlagE, 1, 1 );

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
                _rootWindow.redraw( );
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
                    _rootWindow.redraw( );
                } );
            }
            fbox.set_hexpand( false );

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Message Idx" };
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

            _detailFrames.push_back( std::move( frame ) );
        }

        // trainer
        {
            Gtk::Frame frame{ "Trainer Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

            _detailFrames.push_back( std::move( frame ) );
        }

        // OW pkmn
        {
            Gtk::Frame frame{ "Pkmn Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );

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
            fbox.set_hexpand( false );

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Script Idx" };
            ilabel.set_hexpand( );

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
                    _rootWindow.redraw( );
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
                    _rootWindow.redraw( );
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
            fbox.set_hexpand( false );

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Script Idx" };
            ilabel.set_hexpand( );

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
            _detailFrames.push_back( std::move( frame ) );
        }

        // NPC Message
        {
            Gtk::Frame frame{ "NPC/Message Data" };
            frame.set_margin_top( MARGIN );
            frame.set_label_align( Gtk::Align::CENTER );
            _generalData.append( frame );
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
            fbox.set_hexpand( false );

            Gtk::Box ibox{ Gtk::Orientation::HORIZONTAL };
            ibox.set_margin_top( MARGIN );
            Gtk::Label ilabel{ "Fly Location" };
            ilabel.set_hexpand( );

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
            _detailFrames.push_back( std::move( frame ) );
        }
    }

    void eventSelector::redraw( ) {
        _disableRedraw = true;
        auto evt       = _model.mapEvent( );
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
        case DATA::EVENT_NPC: {
            if( !_disableSI1E ) { _scriptIdx1E.set_value( evt.m_data.m_npc.m_scriptId ); }
            break;
        }
        case DATA::EVENT_GENERIC: {
            if( !_disableSI2E ) { _scriptIdx2E.set_value( evt.m_data.m_generic.m_scriptId ); }
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

        _aFlagE.set_value( evt.m_activateFlag );
        _dFlagE.set_value( evt.m_deactivateFlag );

        if( _eventTrigger ) { _eventTrigger->choose( evt.m_trigger ); }

        _disableRedraw = false;
    }
} // namespace UI::MED
