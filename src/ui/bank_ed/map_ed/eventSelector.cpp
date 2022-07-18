#include <gtkmm/grid.h>
#include <gtkmm/separator.h>

#include "../../root.h"
#include "eventSelector.h"

namespace UI::MED {
    eventSelector::eventSelector( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root },
          _selectedEventA( Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_EVENTS_PER_SLICE - 1.0, 1.0,
                                                    1.0, 0.0 ) ),
          _aFlagA( Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_FLAG, 1.0, 1.0, 0.0 ) ),
          _dFlagA( Gtk::Adjustment::create( 0.0, 0.0, DATA::MAX_FLAG, 1.0, 1.0, 0.0 ) ),
          _selectedEventE( _selectedEventA ), _aFlagE( _aFlagA ), _dFlagE( _dFlagA ) {
        _mainFrame = Gtk::Frame{ "Event Data" };

        Gtk::Box mainBox{ Gtk::Orientation::VERTICAL };
        _mainFrame.set_child( mainBox );
        _mainFrame.set_margin_start( MARGIN );
        _mainFrame.set_label_align( Gtk::Align::CENTER );

        mainBox.set_vexpand( );
        mainBox.set_margin( MARGIN );

        // general event data (idx number, position, activate, deactivate flag)

        Gtk::Box idxBox{ Gtk::Orientation::HORIZONTAL };
        idxBox.get_style_context( )->add_class( "linked" );

        // evt idx
        idxBox.append( _selectedEventE );
        _selectedEventE.signal_value_changed( ).connect( [ & ]( ) {
            _model.selectEvent( _selectedEventE.get_value_as_int( ) );
            _rootWindow.redraw( );
        } );

        // evt type

        _eventType = std::make_shared<dropDown>( DATA::EVENT_TYPE_NAMES );
        if( _eventType ) {
            idxBox.append( *_eventType );
            ( (Gtk::Widget&) ( *_eventType ) ).set_hexpand( true );
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
            []( u32 p_state, u8 p_choice ) { return p_state ^ p_choice; }, 0,
            Gtk::Orientation::VERTICAL );
        if( _eventTrigger ) { g1.attach( *_eventTrigger, 0, 2, 2 ); }

        g1.set_hexpand( false );

        // event type specific widgets
    }

    void eventSelector::redraw( ) {
        auto evt = _model.mapEvent( );
        if( _eventType ) { _eventType->choose( (u8) evt.m_type ); }

        if( !_eventType || !_eventType->currentChoice( ) ) {
            // not a real event, hide everything
            _generalData.hide( );
        } else {
            _generalData.show( );
        }

        if( _eventPosition ) { _eventPosition->setPosition( _model.mapEventPosition( ) ); }
    }
} // namespace UI::MED
