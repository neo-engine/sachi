#include <gtkmm/box.h>

#include "headerBar.h"
#include "root.h"
#include "util.h"

namespace UI {
    headerBar::headerBar( model& p_model, root* p_root ) : _model( p_model ) {
        _rootWindow = p_root;

        auto headerTitleBox = Gtk::Box( Gtk::Orientation::VERTICAL );
        headerTitleBox.append( _titleLabel );
        headerTitleBox.append( _subtitleLabel );

        _titleLabel.get_style_context( )->add_class( "title" );
        _subtitleLabel.get_style_context( )->add_class( "subtitle" );
        _headerBar.set_title_widget( headerTitleBox );
        headerTitleBox.set_valign( Gtk::Align::CENTER );

        _openButton = createButton( "", "_Load FSROOT…", [ & ]( ) {
            // this->onFsRootOpenClick( );
        } );
        _saveButton = createButton( "", "_Save Changes", [ & ]( ) {
            // this->onFsRootSaveClick( );
        } );

        _openMenu = Gio::Menu::create( );
        _saveMenu = Gio::Menu::create( );

        _openMenu->append( "Reload Map", "load.reloadmap" );
        _openMenu->append( "Reload Map Bank", "load.reloadmapbank" );
        _openMenu->append( "Import Map", "load.importmap" );

        _saveMenu->append( "Save Single Map", "save.map" );
        _saveMenu->append( "Save Single Mapbank", "save.mapbank" );
        _saveMenu->append( "Export Map", "save.exportmap" );

        _openMenuPopover = Gtk::PopoverMenu( _openMenu );
        _openMenuPopover.set_has_arrow( false );
        _saveMenuPopover = Gtk::PopoverMenu( _saveMenu );
        _saveMenuPopover.set_has_arrow( false );

        _openMenuButton = std::make_shared<Gtk::MenuButton>( );
        _openMenuButton->set_popover( _openMenuPopover );
        _saveMenuButton = std::make_shared<Gtk::MenuButton>( );
        _saveMenuButton->set_popover( _saveMenuPopover );

        auto openBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        openBox.get_style_context( )->add_class( "linked" );
        openBox.append( *_openButton );
        openBox.append( *_openMenuButton );

        auto saveBox = Gtk::Box( Gtk::Orientation::HORIZONTAL );
        saveBox.get_style_context( )->add_class( "linked" );
        saveBox.append( *_saveButton );
        saveBox.append( *_saveMenuButton );
        _headerBar.pack_start( openBox );
        _headerBar.pack_start( saveBox );
    }

    void headerBar::setTitle( const std::string& p_windowTitle, const std::string& p_mainTitle,
                              const std::string& p_subTitle, bool p_redraw ) {
        _model.m_settings.m_windowTitle = p_windowTitle;
        _model.m_settings.m_mainTitle   = p_mainTitle;
        _model.m_settings.m_subTitle    = p_subTitle;
        if( p_redraw ) { redraw( ); }
    }

    void headerBar::redraw( ) {
        // update button visibility

        if( _openMenuButton ) { _openMenuButton->hide( ); }
        if( _saveMenuButton ) { _saveMenuButton->hide( ); }
        if( _saveButton ) { _saveButton->hide( ); }

        switch( _context ) {
        case CONTEXT_FSROOT_NONE: setTitle( "", _model.m_fsdata.m_fsrootPath ); break;
        case CONTEXT_MAP_EDITOR:
            if( _saveButton ) { _saveButton->show( ); }
            // populate open menu buttons
            if( _openMenuButton ) { _openMenuButton->show( ); }
            if( _saveMenuButton ) { _saveMenuButton->show( ); }
            break;
        case CONTEXT_TILE_EDITOR:
            if( _saveButton ) { _saveButton->show( ); }
            break;
        default: setTitle( ); break;
        }

        if( _model.m_settings.m_windowTitle != "" ) {
            if( _rootWindow ) { _rootWindow->set_title( _model.m_settings.m_windowTitle ); }
        } else {
            if( _rootWindow ) { _rootWindow->set_title( TITLE_STRING ); }
        }

        if( _model.m_settings.m_mainTitle == "" ) {
            if( _model.m_settings.m_subTitle != "" ) {
                _titleLabel.set_text( _model.m_settings.m_subTitle );
            } else if( _rootWindow ) {
                _titleLabel.set_text( _rootWindow->get_title( ) );
            } else {
                _titleLabel.set_text( TITLE_STRING );
            }
            _subtitleLabel.hide( );
        } else {
            _titleLabel.set_text( _model.m_settings.m_mainTitle );
            if( _model.m_settings.m_subTitle != "" ) {
                _subtitleLabel.set_text( _model.m_settings.m_subTitle );
                _subtitleLabel.show( );
            } else {
                _subtitleLabel.hide( );
            }
        }
    }
} // namespace UI
