#include "model.h"
#include "data/util.h"

bool model::loadNewFsRoot( const std::string& p_path ) {
    if( !DATA::checkOrCreatePath( p_path ) ) { return false; }
    if( !DATA::checkOrCreatePath( p_path + "/MAPS/" ) ) { return false; }

    m_fsdata.m_fsrootPath = p_path;

    return true;
}
