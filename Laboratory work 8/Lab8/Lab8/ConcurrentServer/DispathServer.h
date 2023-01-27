#pragma once
#include "Global.h"


DWORD WINAPI DispathServer(LPVOID pPrm)
{
	cout << "DispathServer start;\n" << endl;
	DWORD rc = 0;
	try
	{
		while (*((TalkersCmd*)pPrm) != Exit) //���� ���������
		{
			try
			{
				if (WaitForSingleObject(Event, 300) == WAIT_OBJECT_0) //���� �������
				{

					if (&Work > 0)
					{
						Contact *client = NULL;
						int libuf = 1;
						char CallBuf[50] = "", SendError[50] = "ErrorInquiry";
						EnterCriticalSection(&scListContact);
						//���� ������������ ������ ��������
						for (ListContact::iterator p = Contacts.begin(); p != Contacts.end(); p++)
						{
							if (p->type == Contact::ACCEPT) //���� ������ accept (���������)
							{
								client = &(*p);

								bool Check = false;
								while (Check == false)
								{
									if ((libuf = recv(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR)
									{
										switch (WSAGetLastError())
										{
										case WSAECONNRESET:
										case WSAECONNABORTED:
											p->sthread = Contact::FINISH;
										break;

										case WSAEWOULDBLOCK: Sleep(100); break;
										default: throw  SetErrorMsgText("Recv:", WSAGetLastError());
										}
									}
									else Check = true;
								}
								//������ �������
								//�������� check � ������ �� ������������
								if (strcmp(CallBuf, "Rand") == 0 || strcmp(CallBuf, "Echo") == 0 || strcmp(CallBuf, "Time") == 0)
									Check == true;
								else Check == false;

								if (Check == true)
								{
									client->type = Contact::CONTACT; //������ ������ ���������
									strcpy(client->srvname, CallBuf);
									////////
									client->htimer = CreateWaitableTimer(NULL, TRUE, NULL);
									LARGE_INTEGER li;
									unsigned int control_period = 45; // ������!!!!!!!!!!!! 
									const int nTimerUnitsPerSecond = 10000000;
									li.QuadPart = (LONGLONG)(-1)*control_period*nTimerUnitsPerSecond; 
									SetWaitableTimer(client->htimer, &li, control_period * 1000, (PTIMERAPCROUTINE)ASWTimer, client, FALSE);
									////////

									
									
									//client->htimer = CreateWaitableTimer(NULL, false, NULL); //�������� ���������� �������
									//_int64 time = -240000000; //��������� ������� (60 ������)
									//SetWaitableTimer(client->htimer, (LARGE_INTEGER*)&time, 0, ASWTimer, client, false);



									if ((libuf = send(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR)
										throw SetErrorMsgText("Send:", WSAGetLastError());
									client->hthread = ts1(CallBuf, client);
								}

								else //� ������ ���������� �������
								{
									if ((libuf = send(client->s, SendError, sizeof(SendError) + 1, NULL)) == SOCKET_ERROR)
										throw SetErrorMsgText("Send:", WSAGetLastError());
									closesocket(client->s);
									client->sthread = Contact::ABORT;
									CancelWaitableTimer(client->htimer);
									InterlockedIncrement(&Fail);
								}
							}
						}
						LeaveCriticalSection(&scListContact);
					}
					SleepEx(0, true);
				}
				SleepEx(0, true);
			}
			catch (string errorMsgText)
			{
				std::cout << errorMsgText << endl;
			}
		}
	}
	catch (string errorMsgText)
	{
		std::cout << errorMsgText << endl;
	}
	cout << "DispathServer stopped;\n" << endl;
	ExitThread(rc);
}