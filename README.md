# sachi
An (experimental) map editor for neo. `sachi` aims to be the go-to tool when editing maps of neo projects.
While `sachi` is still in its early stages, it is a functional map editor for projects built
with the neo engine. Main features include

- Editing maps, including
    - blocks
    - movement data
    - events
    - encounter data
    - location data
    - meta data (battle bg, weather, etc.)
    - import from AdvanceMap maps (w/o event data)
- Editing and creating map banks, including auto-generated in-game maps
    - map bank overview feature for quick navigation
- Editing tilesets, blocksets, and corresponding palettes
- Editing trainer data

### Building

`sachi` is built using Gtk4 and its C++ interface gtkmm-4.0. On Linux, after having installed the necessary dependencies,
you can build `sachi` via a simple `make`. More detailed instructions and instructions for other OS follow once `sachi` becomes ready for use.

#### Requirements

- adwaita-icon-theme
- gtkmm-4.0
- libpng
- g++-13
