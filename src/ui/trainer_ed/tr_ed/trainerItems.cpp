#include "../../root.h"
#include "trainerItems.h"

namespace UI::TRE {
    trainerItems::trainerItems( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mainBox.set_margin( MARGIN );
        _mainBox.set_vexpand( );
    }

    void trainerItems::redraw( ) {
    }
} // namespace UI::TRE
