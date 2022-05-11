#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "util.h"

namespace UI {
    std::shared_ptr<Gtk::Button> createButton( const std::string&     p_iconName,
                                               const std::string&     p_labelText,
                                               std::function<void( )> p_callback ) {
        auto hbox = Gtk::Box( Gtk::Orientation::HORIZONTAL, 5 );
        if( p_iconName != "" ) {
            auto icon = Gtk::Image( );
            hbox.append( icon );
            icon.set_from_icon_name( p_iconName );
        }
        if( p_labelText != "" ) {
            auto label = Gtk::Label( p_labelText );
            label.set_expand( true );
            label.set_use_underline( );
            hbox.append( label );
        }

        auto resultButton = std::make_shared<Gtk::Button>( );
        resultButton->set_child( hbox );
        resultButton->signal_clicked( ).connect(
            [ = ]( ) {
                p_callback( );
                resultButton->grab_focus( );
            },
            false );
        return resultButton;
    }
} // namespace UI
