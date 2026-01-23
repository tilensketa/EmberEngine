#pragma once

#include <spdlog/spdlog.h>

namespace Ember {
class Logger {
public:
  static void Init();

  inline static std::shared_ptr<spdlog::logger> &GetLogger() { return sLogger; }

private:
  static std::shared_ptr<spdlog::logger> sLogger;
};
} // namespace Ember

#define LOG_TRACE(...) Ember::Logger::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) Ember::Logger::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) Ember::Logger::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Ember::Logger::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Ember::Logger::GetLogger()->critical(__VA_ARGS__)
#define LOG_DEBUG(...) Ember::Logger::GetLogger()->debug(__VA_ARGS__)
