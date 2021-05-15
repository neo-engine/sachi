#pragma once
#include <memory>

#include <gtkmm/image.h>

#include "data_maprender.h"
#include "defines.h"

namespace UI {
    class block {
      public:
        static std::shared_ptr<Gtk::Image> createImage( const DATA::computedBlock& p_block,
                                                        const DATA::palette*       p_palette,
                                                        u8                         p_daytime = 0 );
    };
} // namespace UI
