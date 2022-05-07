#pragma once
#include <string>

#include <gdkmm/pixbuf.h>
#include <gtkmm/iconview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/scrolledwindow.h>

#include "../defines.h"

namespace UI {
    class root;

    class welcome {
        class recentFsRootModelColumn : public Gtk::TreeModel::ColumnRecord {
          public:
            recentFsRootModelColumn( ) {
                //                add( m_modified );
                add( m_path );
                add( m_pixbuf );
            }

            //            Gtk::TreeModelColumn<Glib::DateTime>               m_modified;
            Gtk::TreeModelColumn<std::string>                  m_path;
            Gtk::TreeModelColumn<std::shared_ptr<Gdk::Pixbuf>> m_pixbuf;
        };

        root* _rootWindow;

        recentFsRootModelColumn             _recentViewColumns;
        Gtk::IconView                       _recentFsRootIconView;
        Glib::RefPtr<Gtk::ListStore>        _recentFsRootListModel;
        std::shared_ptr<Gtk::RecentManager> _recentlyUsedFsRoots;
        Gtk::ScrolledWindow                 _ivScrolledWindow;

      public:
        welcome( root* p_parant );

        inline operator Gtk::Widget&( ) {
            return _ivScrolledWindow;
        }

        inline void connect( std::function<void( const std::string& )> p_loadFsroot ) {
            _recentFsRootIconView.signal_item_activated( ).connect(
                [ this, p_loadFsroot ]( const Gtk::TreeModel::Path& p_path ) {
                    auto iter = _recentFsRootListModel->get_iter( p_path );
                    auto row  = *iter;
                    p_loadFsroot( row[ _recentViewColumns.m_path ] );
                } );
        }

        /*
         * @brief: Adds the specified path to the list of recently used FSROOT paths.
         */
        void addFsRootToRecent( const std::string& p_path );

        /*
         * @brief: Removes the specified path from the recently used FSROOT paths.
         */
        void removeFsRootFromRecent( const std::string& p_path );

        /*
         * @brief: Returns a list of all recently used FSROOT paths.
         */
        auto getRecentFsRoots( );

        /*
         * @brief: Adds the recently used FSROOT paths for quick access to the IconView on
         * the start screen.
         */
        void populateRecentFsRootIconView( );

        inline void show( ) {
            _ivScrolledWindow.show( );
        }

        inline void hide( ) {
            _ivScrolledWindow.hide( );
        }
    };
} // namespace UI
