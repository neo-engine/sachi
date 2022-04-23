#pragma once
#include <string>

#include "defines.h"

constexpr u8 LOGLEVEL_MESSAGE = 0;
constexpr u8 LOGLEVEL_STATUS  = 5;
constexpr u8 LOGLEVEL_DEBUG   = 10;

/*
 * @brief: Prints a log message to stderr (default) or a log file (if desired; TODO) of
 * the form [p_context] p_message.
 * @param p_level: Log level, higher numbers correspond to more unimportant messages.
 */
void message_log( const std::string& p_context, const std::string& p_message, u8 p_level = 0 );

/*
 * @brief: Prints an error message to stderr (default) or a log file (if desired; TODO) of
 * the form [p_context] p_message.
 */
void message_error( const std::string& p_context, const std::string& p_message );
