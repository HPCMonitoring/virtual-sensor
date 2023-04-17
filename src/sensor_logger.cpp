#include "sensor_logger.h"


SensorLogger *SensorLogger::_singleton = nullptr;
std::mutex SensorLogger::_singletonMutex;
const std::string SensorLogger::ROOT_LOGGER_NAME = "ROOT_LOGGER";

SensorLogger::SensorLogger()
{
    // setup root logger
    auto rootLogger = this->getLogger(ROOT_LOGGER_NAME);
    spdlog::set_default_logger(rootLogger);
}

std::shared_ptr<spdlog::logger> SensorLogger::getLogger(std::string loggerName)
{
    if (loggerName.empty())
    {
        return spdlog::get(ROOT_LOGGER_NAME);
    }

    this->_setupLogger(loggerName);
    return spdlog::get(loggerName);
}

void SensorLogger::_setupLogger(const std::string loggerName)
{
    // TODO: Need to replace these hard-code variable by reading from config file

    const std::string LOG_PATTERN = "%Y-%m-%d %H:%M:%S %z - %-6l - [thread-%t@%s:%#] %v";
    const std::string LOG_DIR = "logs";
    const std::string DEBUG_FILE_NAME = "virtual_sensor_debug.log";
    const std::string ERROR_FILE_NAME = "virtual_sensor_error.log";

    std::lock_guard<std::mutex> lock(_registerLoggerMutex);
    if (this->_registerdLogger.count(loggerName))
    {
        return;
    }
    else
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern(LOG_PATTERN);

        auto debug_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LOG_DIR + "/" + DEBUG_FILE_NAME, 1024 * 1024, 5, false);
        debug_file_sink->set_level(spdlog::level::debug);

        auto error_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LOG_DIR + "/" + DEBUG_FILE_NAME, 1024 * 1024, 5, false);
        error_file_sink->set_level(spdlog::level::warn);

        spdlog::sinks_init_list sink_list = {debug_file_sink, console_sink};

        auto logger = std::make_shared<spdlog::logger>(loggerName, sink_list.begin(), sink_list.end());
        // or you can even set multi_sink logger as default logger
        spdlog::register_logger(logger);
        this->_registerdLogger.insert(loggerName);
    }
}

SensorLogger *SensorLogger::getInstance()
{
    std::lock_guard<std::mutex> lock(_singletonMutex);
    if (_singleton == nullptr)
    {
        _singleton = new SensorLogger();
    }
    return _singleton;
}
