#include <gtkmm/centerbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>

#include "../../root.h"
#include "trainerInfo.h"

namespace UI::TRE {
    trainerInfo::trainerInfo( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        Gtk::Frame mainF{ };
        Gtk::Box   mainBox{ Gtk::Orientation::HORIZONTAL };

        _mainBox.append( mainF );
        mainF.set_child( mainBox );

        mainBox.set_margin( MARGIN );
        mainBox.set_vexpand( );

        // left column
        Gtk::Box leftBox{ Gtk::Orientation::VERTICAL };
        mainBox.append( leftBox );

        // trainer class / name
        auto tclassbox = Gtk::CenterBox( );
        auto tclassl   = Gtk::Label( "Trainer Class" ); // TODO: replace with combo box
        _nameL.set_text( "Trainer Name" );
        tclassbox.set_start_widget( tclassl );
        tclassbox.set_end_widget( _nameL );
        leftBox.append( tclassbox );

        // money earned
        auto tmoneybox = Gtk::CenterBox( );
        auto tmoneyl   = Gtk::Label( "Money Yield" );
        tmoneybox.set_start_widget( tmoneyl );
        // TODO:        tmoneybox.set_end_widget(  );
        leftBox.append( tmoneybox );

        // AI level
        auto taibox = Gtk::CenterBox( );
        auto tail   = Gtk::Label( "AI Level" );
        taibox.set_start_widget( tail );
        // TODO:        taibox.set_end_widget(  );
        leftBox.append( taibox );

        // messages

        Gtk::Separator sl1{ };
        leftBox.append( sl1 );

        Gtk::Box            leftiBox{ Gtk::Orientation::VERTICAL };
        Gtk::ScrolledWindow swl{ };
        swl.set_child( leftiBox );
        swl.set_expand( );

        leftBox.append( swl );

        // intro message
        leftiBox.append( _msg1 );
        _msg1.set_text( "msg1\nLine 2" );

        Gtk::Separator sl2{ };
        leftiBox.append( sl2 );

        // win message
        leftiBox.append( _msg2 );
        _msg2.set_text( "msg2" );

        Gtk::Separator sl3{ };
        leftiBox.append( sl3 );

        // lose message
        leftiBox.append( _msg3 );
        _msg3.set_text( "msg3" );

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
        if( _battleBG ) {
            _battleBG->set( _model.m_fsdata.battleBGPath( ), _model.m_fsdata.battlePlatPath( ),
                            _model.selectedTrainer( ).m_battleBG,
                            _model.selectedTrainer( ).m_battlePlat1,
                            _model.selectedTrainer( ).m_battlePlat2 );
            _battleBG->setSprite( _model.m_fsdata.trainerSpritePath( _model.selectedTrainer( ).m_trainerBG ),
                                  _model.selectedTrainer( ).m_trainerBG );
        }
    }
} // namespace UI::TRE
