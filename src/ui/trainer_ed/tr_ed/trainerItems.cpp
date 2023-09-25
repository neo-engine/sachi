#include <gtkmm/frame.h>
#include <gtkmm/separator.h>

#include "../../root.h"
#include "trainerItems.h"

namespace UI::TRE {
    trainerItems::trainerItems( model& p_model, root& p_root )
        : _model{ p_model }, _rootWindow{ p_root } {

        _mainBox.set_margin_top( MARGIN );

        Gtk::Frame mf{ "Items" };
        mf.set_label_align( Gtk::Align::CENTER );
        _mainBox.append( mf );
        Gtk::Box mainBox{ Gtk::Orientation::HORIZONTAL };
        mf.set_child( mainBox );

        mainBox.set_margin( MARGIN );
        mainBox.set_expand( );

        for( int i = 0; i < 5; ++i ) {
            _items[ i ] = std::make_shared<itemSelector>( _model, Gtk::Orientation::VERTICAL );
            if( _items[ i ] ) {
                if( i ) {
                    Gtk::Separator s{ };
                    mainBox.append( s );
                    s.set_margin_start( MARGIN );
                    s.set_margin_end( MARGIN );
                }
                mainBox.append( *_items[ i ] );
                _items[ i ]->connect( [ this, i ]( u16 p_item ) {
                    _model.selectedTrainer( ).m_items[ i ] = p_item;
                    _model.markTrainersChanged( );
                    _rootWindow.redrawPanel( );
                    redraw( );
                } );
            }
        }
    }

    void trainerItems::redraw( ) {
        for( int i = 0; i < 5; ++i ) {
            if( _items[ i ] ) {
                _items[ i ]->refreshModel( );
                _items[ i ]->setData( _model.selectedTrainer( ).m_items[ i ] );
            }
        }
    }
} // namespace UI::TRE
