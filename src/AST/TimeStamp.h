/**
 * @file TimeStamp.h
 * @brief Time stamp and commands used for modeling time management in the compiler.
 *
 * @author Adrián Zamora Sánchez
 * @see Type.h
 * @see AST.h
 */

#pragma once
#include <iostream>
#include <string>

/// Time stamps for time type
enum TimeStamp { TYPE_TICK, TYPE_SEC, TYPE_MIN, TYPE_HR };

/// Time management commands
enum TimeCommand { TIME_EVERY, TIME_AT, TIME_AFTER };

/**
 * @brief Generates the string for the time stamp.
 * @param type Type object.
 * @return string representation of the type.
 *
 * @see Type.h
 */
inline std::string timeToString(TimeStamp type) {
    switch (type) {
    case TimeStamp::TYPE_TICK:
        return "tick";
    case TimeStamp::TYPE_SEC:
        return "sec";
    case TimeStamp::TYPE_MIN:
        return "min";
    case TimeStamp::TYPE_HR:
        return "hr";
    default:
        return "Unknown time type";
    }
}

/**
 * @brief Generates the string for the time command.
 * @param command TimeCommand object.
 * @return string representation of the command.
 */
inline std::string timeCommandToString(TimeCommand command) {
    switch (command) {
    case TimeCommand::TIME_EVERY:
        return "every";
    case TimeCommand::TIME_AT:
        return "at";
    default:
        return "Unknown time command";
    }
}