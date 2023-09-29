#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>

#include "../../log.h"
#include "../root.h"
#include "tr_ed/trainerInfo.h"
#include "tr_ed/trainerItems.h"
#include "tr_ed/trainerTeam.h"
#include "trainerEditor.h"

namespace UI {
    trainerEditor::trainerEditor( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        // set up boxes
        _mainBox.set_margin( MARGIN );

        Gtk::Box diffBox{ Gtk::Orientation::VERTICAL };
        _mainBox.append( diffBox );
        _diffSelector = std::make_shared<switchButton>(
            std::vector<std::string>{ "_Easy", "_Normal", "_Hard" }, 1 );

        if( _diffSelector ) {
            diffBox.append( *_diffSelector );
            _diffSelector->connect( [ this ]( u8 p_newChoice ) {
                setDifficulty( difficulty( p_newChoice ) );
                redraw( );
            } );
        }

        auto diffEnL = Gtk::Label( "Separate Data for this Difficulty" );
        _diffEnabledBox.set_halign( Gtk::Align::CENTER );
        _diffEnabledBox.append( diffEnL );

        _diffEnabled = std::make_shared<switchButton>( std::vector<std::string>{ "_No", "_Yes" } );

        if( _diffEnabled ) {
            _diffEnabledBox.append( *_diffEnabled );
            _diffEnabled->connect( [ this ]( u8 p_newChoice ) {
                _model.markTrainersChanged( );
                _rootWindow.redrawPanel( );
                enableDifficulty( p_newChoice );
                redraw( );
            } );
        }

        diffBox.append( _diffEnabledBox );

        Gtk::ScrolledWindow sb{ };
        sb.set_child( _contentMainBox );
        sb.set_expand( );
        sb.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );

        _contentMainBox.set_margin_top( MARGIN );
        _mainBox.append( sb );

        _trainerInfo = std::make_shared<TRE::trainerInfo>( p_model, p_root );
        if( _trainerInfo ) { _contentMainBox.append( *_trainerInfo ); }

        _trainerTeam = std::make_shared<TRE::trainerTeam>( p_model, p_root );
        if( _trainerTeam ) { _contentMainBox.append( *_trainerTeam ); }

        _trainerItems = std::make_shared<TRE::trainerItems>( p_model, p_root );
        if( _trainerItems ) { _contentMainBox.append( *_trainerItems ); }

        setDifficulty( DIFF_NORMAL );
    }

    void trainerEditor::redraw( ) {
        if( _model.selectedBank( ) != -1 ) { return; }

        _contentMainBox.set_visible(
            _model.selectedTrainerInfo( ).exists( _model.selectedTrainerDifficulty( ) ) );

        if( _diffEnabled ) {
            _diffEnabled->choose(
                _model.selectedTrainerInfo( ).exists( _model.selectedTrainerDifficulty( ) ) );
        }
        if( _diffSelector ) {
            _diffSelector->choose( difficulty( _model.selectedTrainerDifficulty( ) ) );
        }

        if( _trainerInfo ) { _trainerInfo->redraw( ); }
        if( _trainerItems ) { _trainerItems->redraw( ); }
        if( _trainerTeam ) { _trainerTeam->redraw( ); }
    }

    void trainerEditor::setDifficulty( difficulty p_newDifficulty ) {
        if( p_newDifficulty != DIFF_NORMAL ) {
            // show enable button
            _diffEnabledBox.show( );

        } else {
            // hide enable button
            _diffEnabledBox.hide( );
        }

        _model.selectTrainerDifficulty( u8( p_newDifficulty ) );

        _selectedDifficulty = p_newDifficulty;
    }

    void trainerEditor::enableDifficulty( bool p_enabled ) {
        auto diff = _model.selectedTrainerDifficulty( );
        if( diff == 1 ) { return; }
        _model.selectedTrainerInfo( ).m_active[ diff ] = p_enabled;
    }
} // namespace UI
