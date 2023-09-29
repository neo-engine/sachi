#include "../../root.h"
#include "trainerTeam.h"

namespace UI::TRE {
    trainerTeam::trainerTeam( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mainBox.set_margin_top( MARGIN );

        Gtk::Frame mf{ "Team" };
        mf.set_label_align( Gtk::Align::CENTER );
        _mainBox.append( mf );
        Gtk::Box mainBox{ Gtk::Orientation::VERTICAL };
        mf.set_child( mainBox );

        mainBox.set_margin( MARGIN );
        mainBox.set_expand( );

        _teamPreview = std::make_shared<pokeSwitchButton>(
            _model,
            std::vector<std::pair<u16, u8>>{
                { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
            0 );

        if( _teamPreview ) {
            mainBox.append( *_teamPreview );
            _teamPreview->connect( [ this ]( u8 p_newSelection ) {
                _selectedTeamMember = _selectedTeamMember;
                redraw( );
            } );
        }

        _selectedPoke = std::make_shared<trainerPoke>( _model );

        if( _selectedPoke ) {
            mainBox.append( *_selectedPoke );
            _selectedPoke->connect( [ this ]( ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ] = _selectedPoke->get( );
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                redraw( );
            } );
        }
    }

    void trainerTeam::redraw( ) {
        const auto& pkmn = _model.selectedTrainer( ).m_pokemon;
        if( _teamPreview ) {
            std::vector<std::pair<u16, u8>> team;
            for( u8 i = 0; i < 6; ++i ) {
                team.push_back( { pkmn[ i ].m_speciesId, pkmn[ i ].m_forme & 31 } );
            }
            _teamPreview->updateChoices( team );
        }
        if( _selectedPoke ) {
            _selectedPoke->set( _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ] );
        }
    }
} // namespace UI::TRE
