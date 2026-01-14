#include "Logger.h"
#include <fstream>
#include <sstream>
#include <shlobj.h>
#include <chrono>
#include <iomanip>
#include <windows.h> // 添加 Windows API 支持

std::mutex Logger::s_mutex;

// 宽字符转UTF-8
std::string WideToUTF8(const std::wstring& wstr) {
	if (wstr.empty()) return "";
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
	return str;
}

void Logger::Log(const std::wstring& message) {
	std::lock_guard<std::mutex> lock(s_mutex);

	// 获取当前时间
	auto now = std::chrono::system_clock::now();
	auto now_time = std::chrono::system_clock::to_time_t(now);
	std::tm tm_buf;
	localtime_s(&tm_buf, &now_time);
	std::wstringstream ss;
	ss << std::put_time(&tm_buf, L"%Y-%m-%d %H:%M:%S");

	// 构造日志内容
	std::wstring logEntry = L"[" + ss.str() + L"] " + message + L"\n";

	// 转换为UTF-8并写入文件
	std::string utf8LogEntry = WideToUTF8(logEntry);
	std::ofstream logFile(GetLogFilePath(), std::ios::app);
	if (logFile.is_open()) {
		logFile << utf8LogEntry;
		logFile.close();
	}
}

std::wstring Logger::GetLogFilePath() {
	wchar_t tempPath[MAX_PATH];
	GetTempPathW(MAX_PATH, tempPath);
	return std::wstring(tempPath) + L"MyCredentialProvider.log";
}