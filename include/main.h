#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <climits>
#include <thread>
#include <atomic>
#include <stack>
#include <csignal>
#include <unordered_map>
#include <dirent.h>
#include <regex>
#include <mutex>
#include <iomanip>
#include <unordered_set>
#include <sys/utsname.h>
#include <sys/stat.h>
// #include <pcap.h>
#include <boost/format.hpp>
#include <librdkafka/rdkafkacpp.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <nlohmann/json.hpp>

#define NULLIFY(str) str = (str.length() == 0 ? "null" : str)

#endif