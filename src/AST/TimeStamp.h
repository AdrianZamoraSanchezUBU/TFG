#pragma once
#include <iostream>
#include <string>

/**
 * @brief Time stamps
 */
enum TimeStamp { TYPE_TICK, TYPE_SEC, TYPE_MIN, TYPE_HR };

/**
 * @brief Time management commands
 */
enum TimeCommand { TIME_EVERY, TIME_AT };

/**
 * @brief Generates the string for the time stamp
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
 * @brief Generates the string for the time command
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