#include "log.h"
#include "defines.h"

void message_log( const std::string& p_context, const std::string& p_message, u8 ) {
    // TODO: proper implementation
    fprintf( stderr, "[%s] %s\n", p_context.c_str( ), p_message.c_str( ) );
}

void message_error( const std::string& p_context, const std::string& p_message ) {
    // TODO: proper implementation
    fprintf( stderr, "[%s] %s\n", p_context.c_str( ), p_message.c_str( ) );
}
