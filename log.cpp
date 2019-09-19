#include "log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> log_subsys::logger;

std::shared_ptr<spdlog::logger>& log_subsys::main_logger() { return logger; }

void log_subsys::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	logger = spdlog::stdout_color_mt("CHIP8");
	logger->set_level(spdlog::level::trace);
}
