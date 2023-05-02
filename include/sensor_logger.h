#ifndef __SENSOR_LOGGER_H__
#define __SENSOR_LOGGER_H__

#include <mutex>
#include <string>
#include <unordered_set>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

class SensorLogger
{

protected:
    static SensorLogger* _singleton;
    static std::mutex _singletonMutex;
    static const std::string ROOT_LOGGER_NAME;
    std::unordered_set <std::string> _registerdLogger;
    std::mutex _registerLoggerMutex;
    std::shared_ptr<spdlog::logger> _ROOT_LOGGER;

protected:
    SensorLogger();
    void _setupLogger(const std::string loggerName);

public:
    static SensorLogger *getInstance();

    SensorLogger(SensorLogger &other) = delete;
    void operator=(const SensorLogger &) = delete;
    std::shared_ptr<spdlog::logger> getLogger(std::string loggerName = "");
    ~SensorLogger();
};

#endif