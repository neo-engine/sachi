#pragma once
#include <memory>

#include <gtkmm/box.h>
#include <gtkmm/frame.h>

#include "../../../model.h"
#include "../../pure/pokeSwitchButton.h"
#include "../../pure/trainerPoke.h"

namespace UI {
    class root;
}

namespace UI::TRE {
    /*
     * @brief: Widget to display/edit the pkmn of a trainer
     */
    class trainerTeam {
        model& _model;
        root&  _rootWindow;

        u8 _selectedTeamMember = 0;

        Gtk::Box _mainBox{ Gtk::Orientation::HORIZONTAL };

        // switchbox with 6 elements that displays sprites of pkmn/held items; clicking
        std::shared_ptr<pokeSwitchButton> _teamPreview;

        // entry selects and make editable a single pkmn
        std::shared_ptr<trainerPoke> _selectedPoke;

        DATA::trainerPokemon _cache;
        bool                 _hasCachedPkmn = false;

      public:
        static constexpr u8 DEFAULT_MOVES_ACTION = 0;
        static constexpr u8 SET_IV_0_ACTION      = 1;
        static constexpr u8 SET_IV_5_ACTION      = 2;
        static constexpr u8 SET_IV_10_ACTION     = 3;
        static constexpr u8 SET_IV_15_ACTION     = 4;
        static constexpr u8 SET_IV_20_ACTION     = 5;
        static constexpr u8 SET_IV_25_ACTION     = 6;
        static constexpr u8 SET_IV_30_ACTION     = 7;
        static constexpr u8 SET_IV_31_ACTION     = 8;
        static constexpr u8 DEFAULT_ABILITY      = 9;

        trainerTeam( model& p_model, root& p_root );

        inline operator Gtk::Widget&( ) {
            return _mainBox;
        }

        void redraw( );

        inline void show( ) {
            _mainBox.show( );
        }

        inline void hide( ) {
            _mainBox.hide( );
        }

        inline bool isVisible( ) {
            return _mainBox.is_visible( );
        }

        inline void copyAction( ) {
            if( _selectedPoke && _selectedPoke->get( ).m_speciesId ) {
                _hasCachedPkmn = true;
                _cache         = _selectedPoke->get( );
            } else {
                _hasCachedPkmn = false;
            }
        }

        inline void pasteAction( ) {
            if( _selectedPoke && _hasCachedPkmn ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ] = _cache;
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            }
        }

        inline void deleteAction( ) {
            if( _selectedPoke ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ]
                    = DATA::trainerPokemon{ };
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            }
        }

        inline void selectnewAction( ) {
            for( u8 i = 0; i < 6; ++i ) {
                if( !_model.selectedTrainer( ).m_pokemon[ i ].m_speciesId ) {
                    _selectedTeamMember = i;
                    break;
                }
            }
            redraw( );
        }

        void performAction( u8 p_actionId );
    };
} // namespace UI::TRE
