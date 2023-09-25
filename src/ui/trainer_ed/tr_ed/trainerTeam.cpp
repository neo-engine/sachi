#include "../../root.h"
#include "trainerTeam.h"

namespace UI::TRE {
    trainerTeam::trainerTeam( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mainBox.set_margin( MARGIN );
        _mainBox.set_vexpand( );
    }

    void trainerTeam::redraw( ) {
    }
} // namespace UI::TRE
