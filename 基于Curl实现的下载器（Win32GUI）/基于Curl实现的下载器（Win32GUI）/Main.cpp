//�˴�����Դ�ڣ�http://blog.csdn.net/mfcing/article/details/43051865
//�ܺõ�ڹ����libcurl�����UIʵ�ֽ���

//��VS 2013 ѧϰ���У�����_SH_DENYNO��û�ж��壬�����ֶ����������ȥ
#ifndef _SH_DENYNO 
#define _SH_DENYNO 0x40
#endif


#ifdef __cplusplus  
extern "C" {
#endif  
#include "libcurl\include\curl\curl.h"
#ifdef __cplusplus  
}
#endif  


#include <Windows.h>  
#include <tchar.h>
#include <CommCtrl.h>  
#include "resource.h"


//�����������Ի���Ĵ��ھ������Ϊ����Ҫ��������ڷ���Ϣ������֪���䴰�ھ��  
HWND    g_hDlgWnd = NULL;
//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڱ������ص����ݵ��ļ�  
static size_t   DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڼ������ؽ���֪ͨ����  
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
//���ǶԻ������Ϣѭ�����ڿ���̨�������洴��GUI��������Ϊ�˸��õ�չ�����ػص��������  
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��������ص��̺߳�����Ϊ�˲��ѶԻ��������濨�����϶���Ҫ�Լ����߳������ص���  
DWORD WINAPI DownloadThread(LPVOID lpParam);

int _tmain(int argc, _TCHAR* argv[])
{
	//�����Ի���֪���Ի���رղŻ�ִ���˳�  
	DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	return 0;
}

static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//�����ص���������׷�ӵķ�ʽд���ļ�(һ��Ҫ��a������ǰ��д������ݾͻᱻ������)  
	FILE* fp = NULL;
	fopen_s(&fp, "curl-7.46.0.zip", "ab+");
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
	fclose(fp);
	return nWrite;
}

static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	printf(
		"dltotalΪ��  %d%%\n"
		"dlnowΪ��%d%%\n"
		"ultotalΪ��%d%%\n"
		"ulnowΪ��%d%%\n"
		, dltotal, dlnow, ultotal, ulnow
		);


	if (dltotal > -0.1 && dltotal < 0.1)
		return 0;
	int nPos = (int)((dlnow / dltotal) * 100);
	//֪ͨ�������������ؽ���  
	::PostMessage(g_hDlgWnd, WM_USER + 110, nPos, 0);
	//::Sleep(10);  
	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
						  g_hDlgWnd = hWnd;
						  HWND hProgress = GetDlgItem(hWnd, IDC_PROGRESS1);
						  SendMessage(hProgress, PBM_SETRANGE32, (WPARAM)0, (LPARAM)100);
						  //�Ի����ʼ��ʱ���������߳�  
						  HANDLE hThread = CreateThread(NULL, 0, DownloadThread, 0, 0, NULL);
						  CloseHandle(hThread);
						  ::SetWindowText(hWnd, L"ʹ��CURL�����ļ�ʾ����");
						  return TRUE;
	}
	case WM_COMMAND:
	{
					   WORD  msg = HIWORD(wParam);
					   WORD  id = LOWORD(wParam);
					   if (id == IDOK || id == IDCANCEL)
						   EndDialog(hWnd, id);
					   break;
	}
	case WM_ERASEBKGND:
		return TRUE;
	case WM_CTLCOLORSTATIC:
		return (INT_PTR)(HBRUSH)::GetStockObject(WHITE_BRUSH);
	case WM_USER + 110:
	{//���յ����ý��ȵ���Ϣ  
						  HWND    hProgress = GetDlgItem(hWnd, IDC_PROGRESS1);
						  HWND    hStatus = GetDlgItem(hWnd, IDC_EDIT1/*IDC_STATIC*/);
						  if (hProgress)
						  {
							  SendMessage(hProgress, PBM_SETPOS, wParam, 0L);
							  printf("��ǰ������%d%%\n", wParam);
						  }
						  if (hStatus)
						  {
							  WCHAR szBuffer[100] = { 0 };
							  if (wParam < 100)
							  {
								  swprintf_s(szBuffer, L"���������ļ������ȣ�%d%%", wParam);
							  }
							  else
							  {
								  printf("�ļ��������!\n");
								  swprintf_s(szBuffer, L"�ļ�������ϣ�");
							  }
							  ::SetWindowText(hStatus, szBuffer);
						  }
						  return 0;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI DownloadThread(LPVOID lpParam)
{
	//��ʼ��curl������Ǳ����  
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "https://curl.haxx.se/download/curl-7.46.0.zip");
	//���ý������ݵĻص�  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
	//curl_easy_setopt(curl, CURLOPT_INFILESIZE, lFileSize);  
	//curl_easy_setopt(curl, CURLOPT_HEADER, 1);  
	//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);  
	//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
	// �����ض����������  
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// ����301��302��ת����location  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	//���ý��Ȼص�����  
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);  
	//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, g_hDlgWnd);  
	//��ʼִ������  
	CURLcode retcCode = curl_easy_perform(curl);
	//�鿴�Ƿ��г�����Ϣ  
	const char* pError = curl_easy_strerror(retcCode);
	//����curl����ǰ��ĳ�ʼ��ƥ��  
	curl_easy_cleanup(curl);
	return 0;
}
