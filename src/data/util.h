#pragma once
#include <filesystem>
#include <functional>
#include <string>

#include "../defines.h"
#include "maprender.h"

namespace DATA {
    /*
     * @brief: Walks through the specified directory and its subdirectory to explore
     * how large a map bank is and whether its maps are distributed over subfolders.
     */
    mapBankInfo exploreMapBank( const fs::path& p_path );

    /*
     * @brief: Checks if the specified path exists or creates it if it doesn't.
     */
    bool checkOrCreatePath( const std::string& p_path );
} // namespace DATA
