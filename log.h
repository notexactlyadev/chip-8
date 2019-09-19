#pragma once
#include <memory>
#include "spdlog/spdlog.h"

class log_subsys {
public:
	static void Init();
	static std::shared_ptr<spdlog::logger>& main_logger();
private:
	static std::shared_ptr<spdlog::logger> logger;
};

// Core log macros
#define C8_TRACE(...)    log_subsys::main_logger()->trace(__VA_ARGS__)
#define C8_INFO(...)     log_subsys::main_logger()->info(__VA_ARGS__)
#define C8_WARN(...)     log_subsys::main_logger()->warn(__VA_ARGS__)
#define C8_ERROR(...)    log_subsys::main_logger()->error(__VA_ARGS__)
#define C8_CRITICAL(...) log_subsys::main_logger()->critical(__VA_ARGS__)
