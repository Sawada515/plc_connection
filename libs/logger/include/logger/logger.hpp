/**
 * @file logger.hpp
 * @author Sawada
 * @brief syslog.hのラッパー
 * @note ヘッダオンリー
 * @version 0.1
 * @date 2026-11-08
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <syslog.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <format>
#include <mutex>
#include <memory>
#include <stdexcept>

namespace Logger {
    class Logger {
      private:
        std::string service_name_;
        int facility_;

        std::mutex mutex_;

        bool is_opened_;

        static inline int parse_facility(const std::string_view facility)
        {
            static const std::unordered_map<std::string_view, int> facility_map = {
                {"auth",        LOG_AUTH},
                {"authpriv",    LOG_AUTHPRIV},
                {"cron",        LOG_CRON},
                {"daemon",      LOG_DAEMON},
                {"ftp",         LOG_FTP},
                {"kern",        LOG_KERN},
                {"lpr",         LOG_LPR},
                {"mail",        LOG_MAIL},
                {"news",        LOG_NEWS},
                {"syslog",      LOG_SYSLOG},
                {"user",        LOG_USER},
                {"uucp",        LOG_UUCP},
                {"local0",      LOG_LOCAL0},
                {"local1",      LOG_LOCAL1},
                {"local2",      LOG_LOCAL2},
                {"local3",      LOG_LOCAL3},
                {"local4",      LOG_LOCAL4},
                {"local5",      LOG_LOCAL5},
                {"local6",      LOG_LOCAL6},
                {"local7",      LOG_LOCAL7}
            };

            std::string lower_str{facility};

            std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) { return std::tolower(c); });

            auto it = facility_map.find(lower_str);
            if (it != facility_map.end()) {
                return it->second;
            }

            return LOG_LOCAL0;
        }

        inline void log(int priority, const std::string& message)
        {
            if (!is_opened_) {
                return;
            }

            syslog(priority, "%s", message.c_str());
        }

      public:
        Logger(const std::string& service_name, const std::string& facility)
        : service_name_(service_name)
        , facility_(parse_facility(facility))
        , is_opened_(false)
        {
            ;
        }

        ~Logger()
        {
            if (is_opened_) {
                closelog();
            }
        }

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        inline void open()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!is_opened_) {
                openlog(service_name_.c_str(), LOG_PID | LOG_NDELAY, facility_);

                is_opened_ = true;
            }
        }

        inline void close()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (is_opened_) {
                closelog();

                is_opened_ = false;
            }
        }

        template <typename... Args>
        inline void info(std::format_string<Args...> fmt, Args&&... args) {
            log(LOG_INFO, std::format(fmt, std::forward<Args>(args)...));
        }
    
        template <typename... Args>
        inline void warning(std::format_string<Args...> fmt, Args&&... args) {
            log(LOG_WARNING, std::format(fmt, std::forward<Args>(args)...));
        }
    
        template <typename... Args>
        inline void error(std::format_string<Args...> fmt, Args&&... args) {
            log(LOG_ERR, std::format(fmt, std::forward<Args>(args)...));
        }
    
        template <typename... Args>
        inline void debug(std::format_string<Args...> fmt, Args&&... args) {
            log(LOG_DEBUG, std::format(fmt, std::forward<Args>(args)...));
        }
    };
}

#endif  // LOGGER_HPP_