#define WIN32_LEAN_AND_MEAN

#include <malloc.h>
#include <stdlib.h>
#include <windows.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<tlhelp32.h>
#include<ctime>
#include<bits/stdc++.h>


using namespace std;

#define IDB_Focusing 1001		//Focus这个文本 
#define IDB_Focusing_t 1002		//Focus时长 
#define IDB_Resting 1003
#define IDB_Resting_t 1004
#define IDB_Button 1005			//按钮 
#define IDB_More 1006			//按钮 
#define IDB_Tot 1007			//按钮 
#define IDB_Focusing2 1008		//Focus这个文本 
#define IDB_Focusing2_t 1009		//Focus时长 
#define IDB_Resting2 1010
#define IDB_Resting2_t 1011
#define IDB_Save 1012


char szClassName[] = "Windows App w/controls";

HINSTANCE hApplicationInstance = NULL;

HWND hMainWindow = NULL;
int Focusing=-1;
int f1=0,f2=0,r1=0,r2=0;
int ftot,rtot;
int last_time;
int now_time;
int n=0;
HWND hFocusing = NULL;
HWND hFocusing_t = NULL;
HWND hResting= NULL;
HWND hResting_t= NULL;
HWND hButton = NULL;
HWND hMore = NULL;
HWND hTot = NULL;
HWND hFocusing2 = NULL;
HWND hFocusing2_t = NULL;
HWND hResting2= NULL;
HWND hResting2_t= NULL;
HWND hSave= NULL;

bool more = 0;		// 是否展开

const int Weigh=335,Height=225;

static HFONT hFont;

struct lmhyyds
{
	int year,month,day,total;
	int time,rest;
} sheet[1001];


LRESULT CALLBACK WindowProcedure( HWND, UINT, WPARAM, LPARAM ) ;
VOID CALLBACK TimerProc( HWND, UINT, UINT, DWORD ) ;

string zh(int sz)  	//函数，用于int转string
{
	stringstream ss;
	ss<<sz;
	return ss.str();
}
int zs(string sz)  	//函数，用于string转int
{
	stringstream ss;
	ss<<sz;
	int s2;
	ss>>s2;
	return s2;
}
double ss(string sz)  	//函数，用于string转double
{
	stringstream s_s;
	s_s<<sz;
	double s2;
	s_s>>s2;
	return s2;
}
string program_name()  	//函数，获取我叫什么名字
{
	char szFileFullPath[MAX_PATH],szProcessName[MAX_PATH];
	::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH);
	int length=::strlen(szFileFullPath);
	for (int i=length-1; i>=0; i--)
	{
		if (szFileFullPath[i]=='\\')
		{
			i++;
			for (int j=0; i<=length; j++)
			{
				szProcessName[j]=szFileFullPath[i++];
			}
			break;
		}
	}
	return szProcessName;
}
int hm(const char* ProcessName)  	//函数，获取某个名字的进程有多少个
{
	int a=0;
	HANDLE hSnapshot;
	PROCESSENTRY32 lppe;
	BOOL Found;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	lppe.dwSize=sizeof(PROCESSENTRY32);
	Found=Process32First(hSnapshot,&lppe);
	while(Found)
	{
		if(strcmpi(ProcessName,lppe.szExeFile)==0) a++;
		Found = Process32Next(hSnapshot,&lppe);
	}
	CloseHandle(hSnapshot);
	return a;
}
void rechecktime()
{
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);
	if(sheet[n].year==curTime.wYear&&sheet[n].month==curTime.wMonth&&sheet[n].day==curTime.wDay)
	{
		sheet[n].total++;
		sheet[n].time=ftot;
		sheet[n].rest=rtot;
	}
	else
	{

		n++;
		sheet[n].year=curTime.wYear,sheet[n].month=curTime.wMonth,sheet[n].day=curTime.wDay;
		sheet[n].total=1;
		sheet[n].time=ftot;
		sheet[n].rest=rtot;
	}
}
void gout()  	//函数，输出时间记录
{
	rechecktime();
	ofstream fout;
	fout.open("Time Recording.csv");
	fout<<"Date,Focus Time,Rest Time\n";
	for(int a=1; a<=n; a++)
	{

		fout<<sheet[a].year<<".";
		if(sheet[a].month<10) fout<<"0";
		fout<<sheet[a].month<<".";
		if(sheet[a].day<10) fout<<"0";
		fout<<sheet[a].day<<","/*<<sheet[a].total<<","*/;
		fout<<fixed<<setprecision(1)<<sheet[a].time<<",";
		fout<<fixed<<setprecision(1)<<sheet[a].rest<<endl;

	}
	fout.close();
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCommandLine, int nShowCommand)
{
	MSG messages;
	WNDCLASSEX wcex;

	hApplicationInstance = hInstance;

	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(hInstance, "A");
	wcex.hIconSm = LoadIcon(hInstance, "A");
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WindowProcedure;
	wcex.lpszClassName = szClassName;
	wcex.lpszMenuName = NULL;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	//wcex.hbrBackground = CreateSolidBrush(RGB(255,255,255)); //尝试改背景色当时失败了

	if (!RegisterClassEx(&wcex))
		return 0;

	int sx=GetSystemMetrics(SM_CXSCREEN);
	int sy=GetSystemMetrics(SM_CYSCREEN);


	//获取屏幕正中央坐标
	hMainWindow = CreateWindowEx(WS_EX_TOPMOST, szClassName, "Focusing Recorder", /*WS_OVERLAPPEDWINDOW*/WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, (sx-Weigh)/2, (sy-Height)/2, Weigh, Height, HWND_DESKTOP, NULL, hInstance, NULL);

	if(hm((program_name()).c_str())>=2) return 0; //限制只同时运行一个
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	string temp;
	ifstream fin;
	fin.open("Time Recording.csv");
	getline(fin,temp);
	while(getline(fin,temp))
	{
		n++;
		sheet[n].year=zs(temp.substr(0,4));
		sheet[n].month=zs(temp.substr(5,2));
		sheet[n].day=zs(temp.substr(8,2));
		temp.erase(0,11);
		sheet[n].time=zs(temp.substr(0,temp.find(",")));
		sheet[n].rest=zs(temp.substr(temp.find(",")+1,temp.size()-temp.find(",")-1));

	}


	fin.close();

	if(sheet[n].year==curTime.wYear&&sheet[n].month==curTime.wMonth&&sheet[n].day==curTime.wDay) rtot=sheet[n].rest,ftot=sheet[n].time;
	int t1=ftot/60,t2=ftot%60,t3=rtot/60,t4=rtot%60;
	temp="";
	if(t1<10) temp+="0";
	temp+=zh(t1);
	temp+="min ";
	if(t2<10) temp+="0";
	temp+=zh(t2);
	temp+="s";
	SetWindowText(hFocusing2_t,temp.c_str());
	temp="";
	if(t3<10) temp+="0";
	temp+=zh(t3);
	temp+="min ";
	if(t4<10) temp+="0";
	temp+=zh(t4);
	temp+="s";
	SetWindowText(hResting2_t,temp.c_str());


	ShowWindow(hMainWindow, nShowCommand ) ;
	UpdateWindow( hMainWindow ) ;

	while( GetMessage( &messages, NULL, 0, 0) )
	{
		TranslateMessage( &messages ) ;
		DispatchMessage( &messages ) ;
	}
	return messages.wParam ;
}


LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//static HBRUSH hBrush;
	switch(message)
	{
		case WM_CREATE:

			//hBrush = CreateSolidBrush( RGB(0x41, 0x96, 0x4F) );	//尝试改颜色但还是失败了
			SetTimer( hwnd, 2, 1000, NULL ) ;                    //设置一个ID为2, 时间间隔为1秒, 无回调函数的计时器

			hFont = CreateFont(
			            -24/*高度*/, -12/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			            FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			            DEFAULT_CHARSET,  //这里使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
			            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			            DEFAULT_QUALITY,  //默认输出质量
			            FF_DONTCARE,  //不指定字体族*/
			            "等线"  //字体名
			        );

			hButton = CreateWindow("button",  "Start To Focus" ,BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			                       40, 100, 250, 60, hwnd,
			                       (HMENU)IDB_Button,hApplicationInstance,NULL);
			SendMessage(hButton, WM_SETFONT, (WPARAM)hFont , MAKELPARAM(FALSE, 0));

			hMore = CreateWindow("button",  "More ﹀" ,BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			                     40, 165, 250, 30, hwnd,
			                     (HMENU)IDB_More,hApplicationInstance,NULL);
			SendMessage(hMore, WM_SETFONT, (WPARAM)hFont , MAKELPARAM(FALSE, 0));

			hSave = CreateWindow("button",  "&Save" ,BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			                     40, 325, 250, 30, hwnd,
			                     (HMENU)IDB_Save,hApplicationInstance,NULL);
			SendMessage(hSave, WM_SETFONT, (WPARAM)hFont , MAKELPARAM(FALSE, 0));

			hFocusing = CreateWindow("STATIC",  "Focusing", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                         40, 15, 120, 35, hwnd,
			                         (HMENU)IDB_Focusing, hApplicationInstance,NULL);
			SendMessage(hFocusing, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			hFocusing_t = CreateWindow("STATIC",  "--min --s",BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                           170, 15, 170, 35,  hwnd,
			                           (HMENU)IDB_Focusing_t, hApplicationInstance, NULL);
			SendMessage(hFocusing_t, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));


			hResting = CreateWindow("STATIC", "Resting" ,BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                        40, 50, 120, 35,  hwnd,
			                        (HMENU)IDB_Resting, hApplicationInstance,NULL);
			SendMessage(hResting, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			hResting_t = CreateWindow("STATIC",  "--min --s", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                          170, 50, 170, 35, hwnd,
			                          (HMENU)IDB_Resting_t,  hApplicationInstance , NULL);
			SendMessage(hResting_t, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			hTot = CreateWindow("STATIC",  "Tot", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                    150, 210, 170, 35, hwnd,
			                    (HMENU)IDB_Tot,  hApplicationInstance , NULL);
			SendMessage(hTot, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			hFocusing2 = CreateWindow("STATIC",  "Focusing", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                          40, 245, 120, 35, hwnd,
			                          (HMENU)IDB_Focusing2, hApplicationInstance,NULL);
			SendMessage(hFocusing2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			hFocusing2_t = CreateWindow("STATIC",  "--min --s",BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                            170, 245, 170, 35,  hwnd,
			                            (HMENU)IDB_Focusing2_t, hApplicationInstance, NULL);
			SendMessage(hFocusing2_t, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));


			hResting2 = CreateWindow("STATIC", "Resting" ,BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                         40, 290, 120, 35,  hwnd,
			                         (HMENU)IDB_Resting2, hApplicationInstance,NULL);
			SendMessage(hResting2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			hResting2_t = CreateWindow("STATIC",  "--min --s", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
			                           170, 290, 170, 35, hwnd,
			                           (HMENU)IDB_Resting2_t,  hApplicationInstance , NULL);
			SendMessage(hResting2_t, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
			return 0 ;

			//下面这里设置背景颜色失败×3
			/*case WM_CTLCOLORSTATIC://拦截WM_CTLCOLORSTATIC消息
			{
			    if ((HWND)lParam == GetDlgItem(hwnd,IDB_Resting_t))//获得指定标签句柄用来对比
			    {
			        SetTextColor((HDC)wParam,RGB(255,0,0));//设置文本颜色
			        SetBkMode((HDC)wParam,TRANSPARENT);//设置背景透明
			    }

			    return (INT_PTR)GetStockObject((NULL_BRUSH));//返回一个空画刷(必须)
			}*/

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDB_Button:
				{
					if(Focusing==1)
					{

						Focusing=0;
						SetWindowText(hButton,"Start To Focus");

						SYSTEMTIME curTime;
						GetLocalTime(&curTime);

						if(sheet[n].year==curTime.wYear&&sheet[n].month==curTime.wMonth&&sheet[n].day==curTime.wDay)
						{
							sheet[n].total++;
							sheet[n].time=ftot;
							sheet[n].rest=rtot;
						}
						else
						{

							n++;
							sheet[n].year=curTime.wYear,sheet[n].month=curTime.wMonth,sheet[n].day=curTime.wDay;
							sheet[n].total=1;
							sheet[n].time=ftot;
							sheet[n].rest=rtot;
						}
						gout();
						int rt=(f1*60+f2)*0.15;
						r1=rt/60,r2=rt%60;
						string temp="";
						if(r1<10) temp+="0";
						temp+=zh(r1);
						temp+="min ";
						if(r2<10) temp+="0";
						temp+=zh(r2);
						temp+="s";
						SetWindowText(hResting_t,temp.c_str());
						SetWindowText(hFocusing_t,"--min --s");
						f1=0,f2=0;



					}
					else
					{
						f1=0,f2=0,r1=0,r2=0;
						Focusing=1;
						SetWindowText(hButton,"Stop Focusing");
						SetWindowText(hFocusing_t,"00min 00s");
						SetWindowText(hResting_t,"--min --s");
						last_time=time(NULL);

					}
					break;
				}
				case IDB_More:
				{

					if(more==0) SetWindowPos(hMainWindow,NULL,NULL,NULL,Weigh,Height+175,SWP_NOMOVE|SWP_NOZORDER),more=1,SetWindowText(hMore,"Less ︿");
					else SetWindowPos(hMainWindow,NULL,NULL,NULL,Weigh, Height,SWP_NOMOVE|SWP_NOZORDER),more=0,SetWindowText(hMore,"More ﹀");

					break;
				}
				case IDB_Save:
				{

					gout();
					MessageBox(GetForegroundWindow(),"Saved your Excel. But I can't tell if it save successfully.","Tips",0+MB_ICONINFORMATION);
					break;
				}
			}
		}
		case WM_TIMER:        //处理WM_TIMER消息
			switch(wParam)
			{
				case 2:
					if(Focusing==-1)
					{
						rtot++;
						sheet[n].rest=rtot;
						int t3=rtot/60,t4=rtot%60;

						string temp="";
						if(t3<10) temp+="0";
						temp+=zh(t3);
						temp+="min ";
						if(t4<10) temp+="0";
						temp+=zh(t4);
						temp+="s";
						SetWindowText(hResting2_t,temp.c_str());
					}
					else if(Focusing==1)
					{
						ftot++;
						sheet[n].time=ftot;
						f2++;
						if(f2>=60) f1++,f2=0;
						string temp="";
						if(f1<10) temp+="0";
						temp+=zh(f1);
						temp+="min ";
						if(f2<10) temp+="0";
						temp+=zh(f2);
						temp+="s";
						SetWindowText(hFocusing_t,temp.c_str());

						int t1=ftot/60,t2=ftot%60;
						temp="";
						if(t1<10) temp+="0";
						temp+=zh(t1);
						temp+="min ";
						if(t2<10) temp+="0";
						temp+=zh(t2);
						temp+="s";
						SetWindowText(hFocusing2_t,temp.c_str());
						temp="";
					}
					else if(Focusing==0)
					{
						rtot++;
						sheet[n].rest=rtot;
						r2--;
						if(r2<0)
						{
							if(r1<=0)
							{
								Focusing=-1;
								SetWindowText(hResting_t,"--min --s");
								MessageBox(GetForegroundWindow(),"Don't rest for too long! Start your work at once！","Break time is over.",0+MB_ICONWARNING);
								break;
							}
							else r1--,r2=59;

						}
						string temp="";
						if(r1<10) temp+="0";
						temp+=zh(r1);
						temp+="min ";
						if(r2<10) temp+="0";
						temp+=zh(r2);
						temp+="s";
						SetWindowText(hResting_t,temp.c_str());

						int t3=rtot/60,t4=rtot%60;

						temp="";
						if(t3<10) temp+="0";
						temp+=zh(t3);
						temp+="min ";
						if(t4<10) temp+="0";
						temp+=zh(t4);
						temp+="s";
						SetWindowText(hResting2_t,temp.c_str());
					}
					break ;

			}
			return 0 ;

		case WM_DESTROY:
			KillTimer( hwnd, 2 ) ;                //销毁ID为2的计时器
			PostQuitMessage( 0 ) ;
			return 0 ;
	}
	return DefWindowProc( hwnd, message, wParam, lParam ) ;
}

