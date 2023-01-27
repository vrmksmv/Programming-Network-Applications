#pragma once
#include "Global.h"

DWORD WINAPI GarbageCleaner(LPVOID pPrm) // ������� �������� ��������
{
	cout << "GarbageCleaner start;\n" << endl;
	DWORD rc = 0;
	try
	{
		while (*((TalkersCmd*)pPrm) != Exit && &Work > 0)
		{
			//EnterCriticalSection(&scListContact);

			for (auto iterator = Contacts.begin(); iterator != Contacts.end();)
			{
				if (iterator->sthread == Contact::FINISH || iterator->sthread == Contact::TIMEOUT || iterator->sthread == Contact::ABORT /* || iterator->type == Contact::EMPTY  */)
				{
					EnterCriticalSection(&scListContact);

					printf("IP of deleted client:-%s", inet_ntoa(iterator->prms.sin_addr));
					cout << " with code " << iterator->sthread << ";" << endl;
					if (iterator->type == Contact::EMPTY) InterlockedIncrement(&Fail);
					else
					{
						if (iterator->sthread == Contact::FINISH) InterlockedIncrement(&Finished);
						if (iterator->sthread == Contact::TIMEOUT) InterlockedIncrement(&Fail);
						if (iterator->sthread == Contact::ABORT) InterlockedIncrement(&Fail);
						CloseHandle(iterator->hthread);
						CloseHandle(iterator->htimer);				
					}
					closesocket(iterator->s);
					iterator = Contacts.erase(iterator);
					InterlockedDecrement(&Work);

					LeaveCriticalSection(&scListContact);

					break; // for
				}

				else iterator++;
			}
			//LeaveCriticalSection(&scListContact);
			//Sleep(1000);
			Sleep(500);
		}
	}
	catch (string errorMsgText)
	{
		std::cout << errorMsgText << endl;
	}
	cout << "GarbageCleaner stopped;\n" << endl;
	ExitThread(rc);
}