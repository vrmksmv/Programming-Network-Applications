#include "stdafx.h"
#include "Global.h"
#include "AcceptServer.h"
#include "DispathServer.h"
#include "GarbageCleaner.h"
#include "ConsolePipe.h"
#include "ResponseServer.h"


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian"); // устанавлваем кодировку
	SetConsoleTitle("Concurrent Server"); // заголовок консольного окна
	cout << "Parallel server start:" << endl;


	if (argc > 1)
	{
		int tmp = atoi(argv[1]);
		if (tmp >= 0 && tmp <= 65535)
		{
			port = atoi(argv[1]);
			cout << "Usable TCP-port: " << port << ";" << endl;
		}
		else
		{
			cout << "Choose uncorrect TCP-PORT;" << endl;
		}
	}
	else
	{
		cout << "Usable TCP-port: " << port << "(default);" << endl;
	}


	if (argc > 2)
	{
		int tmp = atoi(argv[2]);
		if (tmp >= 0 && tmp <= 65535)
		{
			uport = atoi(argv[2]);
			cout << "Usable UDP-port: " << uport << ";" << endl;
		}
		else
		{
			cout << "Choose uncorrect UDP-port;" << endl;
		}
	}
	else
	{
		cout << "Usable UDP-port: " << uport << "(default);" << endl;
	}


	if (argc > 3)
	{
		dllname = argv[3];
	}


	if (argc > 4)
	{
		npname = argv[4];
		cout << "Name of NamedChannel: " << npname << ";" << endl;
	}
	else
	{
		cout << "Name of NamedChannel: " << npname << "(default);" << endl;
	}

	if (argc > 5)
	{
		ucall = argv[5];
		cout << "Nick:   " << ucall << ";" << endl;
	}
	else
	{
		cout << "Nick:        " << ucall << "  (default);" << endl;
	}
	srand((unsigned)time(NULL));

	volatile TalkersCmd  cmd = Getcommand; //команда управления сервером

	InitializeCriticalSection(&scListContact); //инициалзируем критическую секцию


	st1 = LoadLibrary(dllname); //загружаем dll
	ts1 = (HANDLE(*)(char*, LPVOID))GetProcAddress(st1, "SSS"); //импортируем функцию
	if (st1 == NULL) cout << "Error to load DLL;" << endl;
	else cout << "Loaded DLL: " << dllname << ";" << endl << endl;

	//потоки
	hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
	HANDLE hDispathServer = CreateThread(NULL, NULL, DispathServer, (LPVOID)&cmd, NULL, NULL);
	HANDLE hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
	HANDLE hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
	HANDLE hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

	//устанавливаем приоритеты
	SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriority(hDispathServer, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);


	//ждем завершения потоков
	WaitForSingleObject(hAcceptServer, INFINITE);
	WaitForSingleObject(hDispathServer, INFINITE);
	WaitForSingleObject(hGarbageCleaner, INFINITE);
	WaitForSingleObject(hConsolePipe, INFINITE);
	WaitForSingleObject(hResponseServer, INFINITE);

	//закрываем дескрипторы (освобождаем ресурсы)
	CloseHandle(hAcceptServer);
	CloseHandle(hDispathServer);
	CloseHandle(hGarbageCleaner);
	CloseHandle(hConsolePipe);
	CloseHandle(hResponseServer);

	DeleteCriticalSection(&scListContact); //удаляем критическую секцию

	FreeLibrary(st1); //выгружаем библиотеку

	system("pause");
	return 0;
}

