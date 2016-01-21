#include <stdafx.h>
#include "IEC104Extention.h"

iec104ex_class::iec104ex_class()
{
	mEnding = false;
	mAllowConnect = true;
	mLog.activateLog();
	mLog.dontLogTime();


	//�����¼������ڹ����߳����� 
	//�ֶ����ã���ʼFALSE
	hConnectEvt = CreateEvent(NULL,TRUE,FALSE,NULL);
	//�����˳�
	hAPPExit = CreateEvent(NULL,TRUE,FALSE,NULL);

	//ÿ��ִ�еļ�ʱ��
	AfxBeginThread( threadStateFunc, this );
	AfxBeginThread( threadListening, this );
}

void iec104ex_class::connectTCP()
{
	

	//��ʼ��Socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData);
	m_TCPSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( INVALID_SOCKET == m_TCPSocket )
	{
		char info[255];
		sprintf_s( info, "Error at socket(): %d\n", WSAGetLastError() );	
		mLog.pushMsg( info );
		WSACleanup();
		return;
	}


	SOCKADDR_IN addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons( getPortTCP() );
	addr.sin_addr.S_un.S_addr = inet_addr( getSecondaryIP() );
	int iError = connect(m_TCPSocket, (sockaddr *)&addr, sizeof(addr) );
	if( iError <0 )
	{
		char szInfo[255] = {0};
		sprintf_s(szInfo,"Error in connect() at line45 IEC104EX.CPP, ErrorCode: %d",WSAGetLastError());
		mLog.pushMsg(szInfo);
		return;
	}
	enable_connect();
	onConnectTCP();
	return;
}

void iec104ex_class::disconnectTCP()
{
	int nRet = shutdown(m_TCPSocket,SD_RECEIVE);
	if( SOCKET_ERROR == nRet )
	{
		char szText[255];
		sprintf_s( szText, "ERROR occurs in shutdown(),ERRORCODE: %d ",WSAGetLastError());
		mLog.pushMsg( szText );
		return;
	}
	disable_connect();
	closesocket(m_TCPSocket);
	WSACleanup();
	m_TCPSocket = NULL;
	onDisconnectTCP();
	return;
}

int iec104ex_class::readTCP( char * buf, int szmax )
{
	int nRet = recv( m_TCPSocket, buf, szmax, 0);
	return nRet;
}

void iec104ex_class::sendTCP( char * data, int sz )
{
	send( m_TCPSocket, data, sz, 0 );
}

void iec104ex_class::interrogationActConfIndication()
{

}

void iec104ex_class::interrogationActTermIndication()
{

}

void iec104ex_class::commandActConfIndication( iec_obj *obj )
{

}

void iec104ex_class::commandActTermIndication( iec_obj *obj )
{

}

void iec104ex_class::dataIndication( iec_obj *obj, int numpoints )
{
	HWND hMainWindow = AfxGetApp()->m_pMainWnd->m_hWnd;
	::SendMessage( hMainWindow, WM_INFONOTIFY, (WPARAM )obj, (LPARAM)numpoints);
	return;
}

void iec104ex_class::startListening()
{
	AfxBeginThread( threadListening, this );
}

UINT iec104ex_class::threadStateFunc( LPVOID lParam )
{
	iec104ex_class *pIECex = (iec104ex_class *)lParam;
	HANDLE h1SecondEvt = CreateEvent(NULL,FALSE,FALSE,NULL);
	HANDLE hAPPExit = pIECex->hAPPExit;
	HANDLE hWaitObjects[2] = { hAPPExit, h1SecondEvt };

	while( true)
	{
		DWORD dwReturns = WaitForMultipleObjects( 2, hWaitObjects, false, 1000 );
		if (dwReturns == WAIT_TIMEOUT)
		{
			pIECex->onTimerSecond();
		}
		else if( dwReturns == WAIT_OBJECT_0 )
		{
			break;
		}
		
	}
	return 0;
}

iec104ex_class::~iec104ex_class()
{
	mLog.deactivateLog();
}

UINT iec104ex_class::threadListening( LPVOID lParam )
{
	iec104ex_class *pIECex = (iec104ex_class *)lParam;
	HANDLE hAPPExit = pIECex->hAPPExit;
	HANDLE hWaitObjects[2] = { hAPPExit, pIECex->hConnectEvt };

	while( true )
	{
		DWORD dwReturn = WaitForMultipleObjects( 2, hWaitObjects, false, INFINITE ) ;
		if( dwReturn == WAIT_OBJECT_0 )
		{
			break;
		}else if(dwReturn == WAIT_OBJECT_0+1 )
		{
			char buf[1024];
			memset( buf, 0 ,1024);
			int BytesInQue = recv(pIECex->m_TCPSocket,(char*)buf, 1024, MSG_PEEK);
			if( BytesInQue <= 0 )
			{
				//do something to disconnect
				
			}
			pIECex->packetReadyTCP();
		}
	}
	return 0;
}


void iec104ex_class::enable_connect()
{
	SetEvent(hConnectEvt);
	return;
}

void iec104ex_class::disable_connect()
{
	ResetEvent(hConnectEvt);
	return;
}

void iec104ex_class::OnExit()
{
	SetEvent(hAPPExit);
}
