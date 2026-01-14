#pragma once
#include <windows.h>
#include <string>
#include <mutex>

class Logger {
public:
	static void Log(const std::wstring& message);

private:
	static std::wstring GetLogFilePath();
	static std::mutex s_mutex; // 线程安全
};