#include <gtkmm/application.h>

#include "data_bitmap.h"
#include "defines.h"
#include "ui_root.h"

std::string FSROOT_PATH;

int main( int p_argc, char* p_argv[] ) {
    auto app = Gtk::Application::create( "pneo.sachi" );
    return app->make_window_and_run<UI::root>( p_argc, p_argv );
}
