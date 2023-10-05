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
                _selectedTeamMember = p_newSelection;
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
                team.push_back( { pkmn[ i ].m_speciesId,
                                  pkmn[ i ].m_forme | ( ( !!pkmn[ i ].m_shiny ) << 5 ) } );
            }
            _teamPreview->updateChoices( team );
        }
        if( _selectedPoke ) {
            _selectedPoke->set( _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ] );
        }
        if( _teamPreview && _selectedTeamMember != _teamPreview->currentChoice( ) ) {
            _teamPreview->choose( _selectedTeamMember );
        }
    }

    void trainerTeam::performAction( u8 p_actionId ) {
        switch( p_actionId ) {
        case DEFAULT_MOVES_ACTION: {
            // read and set default moves for the currently selected pkmn and level.
            u16 tmp_moves[ 4 ] = { 0 };

            _model.m_fsdata.getLearnMoves(
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_speciesId,
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_forme & 31, 0,
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_level, 4, tmp_moves );

            for( u8 i = 0; i < 4; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_moves[ i ]
                    = tmp_moves[ i ];
            }

            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );
            break;
        }
        case SET_IV_0_ACTION: {
            // set all iv to 0
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 0;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_5_ACTION: {
            // set all iv to 5
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 5;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_10_ACTION: {
            // set all iv to 10
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 10;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_15_ACTION: {
            // set all iv to 0
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 15;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_20_ACTION: {
            // set all iv to 20
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 20;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_25_ACTION: {
            // set all iv to 25
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 25;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_30_ACTION: {
            // set all iv to 30
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 30;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case SET_IV_31_ACTION: {
            // set all iv to 31
            for( u8 i = 0; i < 6; ++i ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_iv[ i ] = 31;
            }
            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );

            break;
        }
        case DEFAULT_ABILITY: {
            // read and set default moves for the currently selected pkmn and level.
            DATA::pkmnData data = _model.m_fsdata.getPkmnData(
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_speciesId,
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_forme & 31 );

            if( !_model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_ability ) {
                _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_ability
                    = data.m_baseForme.m_abilities[ 0 ];
            } else {
                for( u8 i = 0; i < 4; ++i ) {
                    if( i < 3
                        && data.m_baseForme.m_abilities[ i ]
                               == data.m_baseForme.m_abilities[ ( i + 1 ) % 4 ] ) {
                        continue;
                    }
                    if( _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_ability
                        == data.m_baseForme.m_abilities[ i ] ) {
                        _model.selectedTrainer( ).m_pokemon[ _selectedTeamMember ].m_ability
                            = data.m_baseForme.m_abilities[ ( i + 1 ) % 4 ];
                        break;
                    }
                }
            }

            _model.markTrainersChanged( );
            _rootWindow.redrawPanel( );
            redraw( );
            break;
        }

        default: break;
        }
    }
} // namespace UI::TRE
