#pragma once
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
class Logger
{

public:
	enum LOGTYPE {
		WARN,
		ERRORZ,
		DEBUG
	};
	static Logger* log;
	Logger();
	~Logger();
	HANDLE procHandle;
	void LogHeapSize();
	Logger(char* logName);
	void OpenStream();
	void Create();
	int writeCount;
void	LogIt(LOGTYPE t, char*);
private:
	char* nameOfLog;
	FILE* logFile;
};

