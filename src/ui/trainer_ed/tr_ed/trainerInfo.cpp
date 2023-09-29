#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>

#include "../../root.h"
#include "trainerInfo.h"

namespace UI::TRE {
    trainerInfo::trainerInfo( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root },
          _money{ Gtk::Adjustment::create( 0.0, 0.0, 65535.0, 10.0, 100.0, 0.0 ) },
          _moneyE{ _money }, _ai{ Gtk::Adjustment::create( 0.0, 0.0, 9.0, 1.0, 1.0, 0.0 ) },
          _aiE{ _ai } {

        Gtk::Frame mainF{ };
        Gtk::Box   mainBox{ Gtk::Orientation::HORIZONTAL };

        _mainBox.append( mainF );
        mainF.set_child( mainBox );

        mainBox.set_margin( MARGIN );
        mainBox.set_vexpand( );

        // left column
        Gtk::Box leftBox{ Gtk::Orientation::VERTICAL };
        mainBox.append( leftBox );

        leftBox.append( _nameL );
        _nameL.set_halign( Gtk::Align::CENTER );

        // trainer class / name
        auto tclassbox = Gtk::CenterBox( );
        tclassbox.set_margin_top( MARGIN );
        auto tclassl = Gtk::Label( "Trainer Class" ); // TODO: replace with combo box
        tclassbox.set_start_widget( tclassl );
        _tclassChooser = std::make_shared<stringCacheDropDown>( );
        if( _tclassChooser ) {
            tclassbox.set_end_widget( *_tclassChooser );
            _tclassChooser->connect( [ this ]( u64 p_newChoice ) {
                _model.selectedTrainer( ).m_trainerClass = p_newChoice;
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            } );
        }

        leftBox.append( tclassbox );

        // money earned
        auto tmoneybox = Gtk::CenterBox( );
        tmoneybox.set_margin_top( MARGIN );
        auto tmoneyl = Gtk::Label( "Money Yield" );
        tmoneybox.set_start_widget( tmoneyl );
        tmoneybox.set_end_widget( _moneyE );
        leftBox.append( tmoneybox );
        _moneyE.set_numeric( true );
        _money->signal_value_changed( ).connect( [ this ]( ) {
            if( _lock ) { return; }
            _model.selectedTrainer( ).m_moneyMultiplier = _moneyE.get_value( );
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );
        } );

        // AI level
        auto taibox = Gtk::CenterBox( );
        taibox.set_margin_top( MARGIN );
        auto tail = Gtk::Label( "AI Level" );
        taibox.set_start_widget( tail );
        taibox.set_end_widget( _aiE );
        leftBox.append( taibox );
        _aiE.set_numeric( true );
        _ai->signal_value_changed( ).connect( [ this ]( ) {
            if( _lock ) { return; }
            _model.selectedTrainer( ).m_AILevel = _aiE.get_value( );
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );
        } );

        // double battle
        auto dbbox = Gtk::CenterBox( );
        auto dbl   = Gtk::Label( "Is Double Battle?" );
        dbbox.set_margin_top( MARGIN );
        dbbox.set_start_widget( dbl );
        leftBox.append( dbbox );
        _doubleBattle = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );
        if( _doubleBattle ) {
            dbbox.set_end_widget( *_doubleBattle );
            ( (Gtk::Widget&) ( *_doubleBattle ) ).set_margin( 0 );
            _doubleBattle->connect( [ this ]( u8 p_newChoice ) {
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                _model.selectedTrainer( ).m_forceDoubleBattle = p_newChoice;
                redraw( );
            } );
        }

        // messages

        Gtk::Separator sl1{ };
        leftBox.append( sl1 );
        sl1.set_margin_top( MARGIN );
        sl1.set_margin_bottom( MARGIN );

        Gtk::Box            leftiBox{ Gtk::Orientation::VERTICAL };
        Gtk::ScrolledWindow swl{ };
        swl.set_child( leftiBox );
        swl.set_expand( );

        leftBox.append( swl );

        // intro message
        leftiBox.append( _msg1 );

        Gtk::Separator sl2{ };
        leftiBox.append( sl2 );
        sl2.set_margin_top( MARGIN );
        sl2.set_margin_bottom( MARGIN );

        // win message
        leftiBox.append( _msg2 );

        Gtk::Separator sl3{ };
        leftiBox.append( sl3 );
        sl3.set_margin_top( MARGIN );
        sl3.set_margin_bottom( MARGIN );

        // lose message
        leftiBox.append( _msg3 );

        Gtk::Separator s1{ };
        mainBox.append( s1 );
        s1.set_margin_start( MARGIN );
        s1.set_margin_end( MARGIN );

        // right column
        Gtk::Box rightBox{ Gtk::Orientation::VERTICAL };
        mainBox.append( rightBox );

        _battleBG = std::make_shared<battleBG>( "Battle BG" );
        if( _battleBG ) {
            ( (Gtk::Widget&) ( *_battleBG ) ).set_hexpand( );
            rightBox.append( *_battleBG );
            _battleBG->connect2( [ this ]( u8 p_bg, u8 p_p1, u8 p_p2, u8 p_sid ) {
                _model.selectedTrainer( ).m_trainerBG   = p_sid;
                _model.selectedTrainer( ).m_battlePlat1 = p_p1;
                _model.selectedTrainer( ).m_battlePlat2 = p_p2;
                _model.selectedTrainer( ).m_battleBG    = p_bg;
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            } );
        }
    }

    void trainerInfo::redraw( ) {
        _lock = true;

        _aiE.set_value( _model.selectedTrainer( ).m_AILevel );
        _moneyE.set_value( _model.selectedTrainer( ).m_moneyMultiplier );

        const auto& tnames = _model.trainerNames( );
        if( tnames.m_valid && _model.selectedTrainerId( ) < tnames.m_strings.size( ) ) {
            _nameL.set_text( tnames.m_strings[ _model.selectedTrainerId( ) ] );
        } else {
            _nameL.set_text( "(trainer name undefined)" );
        }

        const auto& msg1 = _model.trainerMessage( 0 );
        if( msg1.m_valid && _model.selectedTrainerId( ) < msg1.m_strings.size( ) ) {
            _msg1.set_text( msg1.m_strings[ _model.selectedTrainerId( ) ] );
        } else {
            _msg1.set_text( "(message 1 undefined)" );
        }

        const auto& msg2 = _model.trainerMessage( 1 );
        if( msg2.m_valid && _model.selectedTrainerId( ) < msg2.m_strings.size( ) ) {
            _msg2.set_text( msg2.m_strings[ _model.selectedTrainerId( ) ] );
        } else {
            _msg2.set_text( "(message 2 undefined)" );
        }

        const auto& msg3 = _model.trainerMessage( 2 );
        if( msg3.m_valid && _model.selectedTrainerId( ) < msg3.m_strings.size( ) ) {
            _msg3.set_text( msg3.m_strings[ _model.selectedTrainerId( ) ] );
        } else {
            _msg3.set_text( "(message 3 undefined)" );
        }

        if( _tclassChooser ) {
            _tclassChooser->refreshModel( _model.trainerClasses( ) );
            _tclassChooser->choose( _model.selectedTrainer( ).m_trainerClass );
        }

        if( _battleBG ) {
            _battleBG->set( _model.m_fsdata.battleBGPath( ), _model.m_fsdata.battlePlatPath( ),
                            _model.selectedTrainer( ).m_battleBG,
                            _model.selectedTrainer( ).m_battlePlat1,
                            _model.selectedTrainer( ).m_battlePlat2 );
            _battleBG->setSprite(
                _model.m_fsdata.trainerSpritePath( _model.selectedTrainer( ).m_trainerBG ),
                _model.selectedTrainer( ).m_trainerBG );
        }

        if( _doubleBattle ) {
            _doubleBattle->choose( _model.selectedTrainer( ).m_forceDoubleBattle );
        }

        _lock = false;
    }
} // namespace UI::TRE
