#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Ember {
std::shared_ptr<spdlog::logger> Logger::sLogger;
void Logger::Init() { 
  Logger::sLogger = spdlog::stdout_color_mt("Engine");
  spdlog::set_level(spdlog::level::debug);
}
} // namespace Ember
