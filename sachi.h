#pragma once
#include <memory>

#include <gtkmm/application.h>

#include "defines.h"
#include "ui_root.h"

class sachi : public Gtk::Application {
  protected:
    sachi( );

  public:
    static std::shared_ptr<sachi> create( );

  protected:
    void on_activate( ) override;
    void on_open( const Gio::Application::type_vec_files& p_files,
                  const Glib::ustring&                    p_hint ) override;

  private:
    UI::root* createWindow( );
    void      onHideWindow( Gtk::Window* p_window );
};
