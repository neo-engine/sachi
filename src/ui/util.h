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

    /*
     * @brief: Walks through the specified directory and its subdirectory to explore
     * how large a map bank is and whether its maps are distributed over subfolders.
     */
    DATA::mapBankInfo exploreMapBank( const fs::path& p_path );

    /*
     * @brief: Checks if the specified path exists or creates it if it doesn't.
     */
    bool checkOrCreatePath( const std::string& p_path );
} // namespace UI
