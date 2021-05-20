#include <gtkmm/application.h>

#include "data_bitmap.h"
#include "defines.h"
#include "sachi.h"
#include "ui_root.h"

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
    return app->run( p_argc, p_argv );
}
