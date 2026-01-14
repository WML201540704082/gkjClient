//#pragma once
//#include <windows.h>
//#include <thread>
//#include <string>
//#include <mutex>
//
//class myProvider; // Ç°ÏòÉùÃ÷
//
//class PipeManager
//{
//public:
//	static PipeManager& GetInstance();
//
//	void Start(myProvider* pProvider);
//	void Stop();
//	bool IsRunning() const { return _bRunning; }
//
//	// ½ûÖ¹¿½±´
//	PipeManager(const PipeManager&) = delete;
//	PipeManager& operator=(const PipeManager&) = delete;
//
//private:
//	PipeManager();
//	~PipeManager();
//
//	static void ListenForPassword(PipeManager* pManager);
//
//	std::thread _pipeThread;
//	myProvider* _pProvider;
//	bool _bRunning;
//	bool _bStopRequested;
//	std::mutex _mutex;
//
//	static PipeManager* s_instance;
//	static std::mutex s_instanceMutex;
//};
//
