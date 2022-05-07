#pragma once
#include <filesystem>
#include <functional>
#include <string>

#include <gtkmm/button.h>

#include "../data/maprender.h"
#include "../defines.h"

namespace UI {
    /*
     * @brief: Creates a new Gtk::Button with an icon and a string and the given
     * handler for the clicked signal.
     */
    std::shared_ptr<Gtk::Button> createButton(
        const std::string& p_iconName, const std::string& p_labelText,
        std::function<void( )> p_callBack = []( ) {} );
} // namespace UI
