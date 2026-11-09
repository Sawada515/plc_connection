/**
 * @file logger_test.cpp
 * @brief Unit tests for the logger module.
 * @author Sawada
 * @date 2026-11-09
 */

#include <logger/logger.hpp>

int main()
{
    Logger::Logger logger("test_service", "local0");

    logger.open();

    logger.info("test message: {}", 20261109);

    logger.close();

    return 0;
}