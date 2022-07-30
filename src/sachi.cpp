#include <gtkmm/application.h>

#include "data/bitmap.h"
#include "defines.h"
#include "sachi.h"
#include "ui/root.h"

std::string FSROOT_PATH;

sachi::sachi( ) : Gtk::Application( "pneo.sachi", Gio::Application::Flags::HANDLES_OPEN ) {
}

std::shared_ptr<sachi> sachi::create( ) {
    return std::shared_ptr<sachi>( new sachi( ) );
}

void sachi::on_activate( ) {
    auto window = createWindow( );
    window->present( );
}

void sachi::on_open( const Gio::Application::type_vec_files& p_files, const Glib::ustring& ) {
    UI::root* window  = nullptr;
    auto      windows = get_windows( );
    if( windows.size( ) > 0 ) { window = dynamic_cast<UI::root*>( windows[ 0 ] ); }

    if( !window ) { window = createWindow( ); }

    for( const auto& file : p_files ) {
        window->loadNewFsRoot( file->get_path( ) );
        break;
    }

    window->present( );
}

UI::root* sachi::createWindow( ) {
    auto window = new UI::root( );
    add_window( *window );

    window->signal_hide( ).connect( [ this, window ]( ) { onHideWindow( window ); } );
    return window;
}

void sachi::onHideWindow( Gtk::Window* p_window ) {
    delete p_window;
}

int main( int p_argc, char* p_argv[] ) {
    auto app = sachi::create( );

    app->set_accel_for_action( "load.fsroot", "<Primary>o" );
    app->set_accel_for_action( "load.reloadmap", "<Primary>r" );
    app->set_accel_for_action( "load.reloadmapbank", "<Primary><Shift>r" );
    app->set_accel_for_action( "load.importmap", "<Primary>m" );
    app->set_accel_for_action( "load.importlargemap", "<Primary><Shift>m" );
    app->set_accel_for_action( "load.importblocks1", "<Primary>b" );
    app->set_accel_for_action( "load.importblocks2", "<Primary><Shift>b" );
    app->set_accel_for_action( "load.importtiles1", "<Primary>t" );
    app->set_accel_for_action( "load.importtiles2", "<Primary><Shift>t" );

    app->set_accel_for_action( "save.fsroot", "<Primary>s" );
    app->set_accel_for_action( "save.map", "<Primary><Shift>s" );
    app->set_accel_for_action( "save.mapbank", "<Primary><Alt>s" );
    app->set_accel_for_action( "save.exportmap", "<Primary><Alt>m" );
    app->set_accel_for_action( "save.exportblocks1", "<Primary><Alt><Shift>b" );
    app->set_accel_for_action( "save.exportblocks2", "<Primary><Alt>b" );
    app->set_accel_for_action( "save.exporttiles1", "<Primary><Alt><Shift>t" );
    app->set_accel_for_action( "save.exporttiles2", "<Primary><Alt>t" );

    return app->run( p_argc, p_argv );
}
