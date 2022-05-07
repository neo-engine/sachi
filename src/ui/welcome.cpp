#include "welcome.h"
#include "../log.h"
#include "root.h"

namespace UI {
    void welcome::addFsRootToRecent( const std::string& p_path ) {
        auto res       = Gtk::RecentManager::Data( );
        res.app_name   = APP_NAME;
        res.is_private = true;
        if( !_recentlyUsedFsRoots->add_item( "file://" + p_path, res ) ) {
            message_log( "addFsRootToRecent", "Adding \"" + p_path + "\" failed.", LOGLEVEL_DEBUG );
        }
    }

    void welcome::removeFsRootFromRecent( const std::string& p_path ) {
        try {
            _recentlyUsedFsRoots->remove_item( p_path );
        } catch( ... ) {}
    }

    auto welcome::getRecentFsRoots( ) {
        auto unfiltered = _recentlyUsedFsRoots->get_items( );

        auto res = std::vector<std::shared_ptr<Gtk::RecentInfo>>( );

        for( auto in : unfiltered ) {

            if( in->has_application( APP_NAME ) ) {
                if( in->exists( ) ) {
                    res.push_back( in );
                } else {
                    removeFsRootFromRecent( in->get_uri( ) );
                }
            }
        }

        return res;
    }

    void welcome::populateRecentFsRootIconView( ) {
        for( auto rf : getRecentFsRoots( ) ) {
            auto row                           = *( _recentFsRootListModel->append( ) );
            row[ _recentViewColumns.m_path ]   = rf->get_uri_display( );
            row[ _recentViewColumns.m_pixbuf ] = Gdk::Pixbuf::create_from_file( "./icon.bmp" );
            //            row[ _recentViewColumns.m_modified ] = rf->get_modified( );
        }
    }

    welcome::welcome( root* p_root ) {
        _rootWindow = p_root;

        _recentlyUsedFsRoots = Gtk::RecentManager::get_default( );

        // Main window on application start:
        // Show icon view of recently used FSROOT folders

        _ivScrolledWindow.set_policy( Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC );
        _ivScrolledWindow.set_expand( );
        _ivScrolledWindow.set_child( _recentFsRootIconView );

        _recentFsRootListModel = Gtk::ListStore::create( _recentViewColumns );
        //        _recentFsRootListModel->set_sort_column( _recentViewColumns.m_modified,
        //                                                 Gtk::SortType::DESCENDING );

        _recentFsRootIconView.set_model( _recentFsRootListModel );
        _recentFsRootIconView.set_pixbuf_column( _recentViewColumns.m_pixbuf );
        _recentFsRootIconView.set_text_column( _recentViewColumns.m_path );
        _recentFsRootIconView.set_activate_on_single_click( );

        populateRecentFsRootIconView( );
    }
} // namespace UI
