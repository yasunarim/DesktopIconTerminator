// DesktopIconTerminator.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "resource.h"
#include "DesktopIconTerminator.h"
#include "darknet.h"

#define MAX_LOADSTRING 100
#define WM_TASKTRAY   WM_USER + 21
#define ID_TASKTRAY 0

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DESKTOPICONTERMINATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DESKTOPICONTERMINATOR));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DESKTOPICONTERMINATOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DESKTOPICONTERMINATOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

image Hwnd2Image(HWND hwnd, int nPosLeft, int nPosTop, int nDestWidth, int nDestHeight)
{
	int channels = 3;
	HDC hwindowDC = GetDC(hwnd);
	HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);

	HBITMAP           hbwindow = NULL;
	image           im;
	BITMAPINFOHEADER  bi;

	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;
	GetClientRect(hwnd, &windowsize);

	int srcheight = windowsize.bottom;
	int srcwidth = windowsize.right;

	im = make_image(nDestWidth, nDestHeight, channels);
	char* data = new char[nDestWidth * nDestHeight * channels];
	memset(data, 0, nDestWidth * nDestHeight * channels);
	
	hbwindow = CreateCompatibleBitmap(hwindowDC, nDestWidth, nDestHeight);
	if (hbwindow == NULL)
	{
		im.data = NULL;
		return im;
	}

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = nDestWidth;
	bi.biHeight = -nDestHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	SelectObject(hwindowCompatibleDC, hbwindow);

	StretchBlt(hwindowCompatibleDC, 0, 0, nDestWidth, nDestHeight, hwindowDC, nPosLeft, nPosTop, nDestWidth, nDestHeight, SRCCOPY);
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, nDestHeight, data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	if (hbwindow != NULL)
	{
		DeleteObject(hbwindow);
		hbwindow = NULL;
	}

	if (hwindowCompatibleDC != NULL)
	{
		DeleteDC(hwindowCompatibleDC);
		hwindowCompatibleDC = NULL;
	}

	if (hwindowDC != NULL)
	{
		ReleaseDC(hwnd, hwindowDC);
		hwindowDC = NULL;
	}

	copy_image_from_bytes(im, data);

	delete [] data;
	return im;
}

void DragAndDrop(float sx, float sy, float dx, float dy)
{
    INPUT inp[1] = { 0 };
    inp[0].type = INPUT_MOUSE;
    inp[0].mi.time = 0;
    inp[0].mi.dwExtraInfo = 0;
    inp[0].mi.dx = sx * (65535 / GetSystemMetrics(SM_CXSCREEN));
    inp[0].mi.dy = sy * (65535 / GetSystemMetrics(SM_CYSCREEN));
    inp[0].mi.mouseData = 0;
    inp[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    SendInput(1, inp, sizeof(INPUT));
    Sleep(100);

    inp[0].type = INPUT_MOUSE;
    inp[0].mi.time = 0;
    inp[0].mi.dwExtraInfo = 0;
    inp[0].mi.dx = 0;
    inp[0].mi.dy = 0;
    inp[0].mi.mouseData = 0;
    inp[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, inp, sizeof(INPUT));
    Sleep(100);

    float slidex = (dx - sx) / 3;
    float slidey = (dy - sy) / 3;

    for (int i = 1; i <= 3; i++)
    {
        inp[0].type = INPUT_MOUSE;
        inp[0].mi.time = 0;
        inp[0].mi.dwExtraInfo = 0;
        inp[0].mi.dx = (sx + slidex * i) * (65535 / GetSystemMetrics(SM_CXSCREEN));
        inp[0].mi.dy = (sy + slidey * i) * (65535 / GetSystemMetrics(SM_CYSCREEN));
        inp[0].mi.mouseData = 0;
        inp[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        SendInput(1, inp, sizeof(INPUT));
        Sleep(100);
    }

    inp[0].type = INPUT_MOUSE;
    inp[0].mi.time = 0;
    inp[0].mi.dwExtraInfo = 0;
    inp[0].mi.dx = 0;
    inp[0].mi.dy = 0;
    inp[0].mi.mouseData = 0;
    inp[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, inp, sizeof(INPUT));
    Sleep(100);

    SendInput(1, inp, sizeof(INPUT));

}

unsigned __stdcall desktopwatcher(void*)
{
    network* net = load_network_custom(const_cast<char*>("D:\\Projects\\python\\desktopyolov3\\cfg\\yolov3-voc.cfg"),
                                       const_cast<char*>("D:\\Projects\\python\\desktopyolov3\\data\\backup\\yolov3-voc_last.weights"), 0, 1);
	while (true)
	{

        image im = Hwnd2Image(GetDesktopWindow(), 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        
        float* p = network_predict_image(net, im);
        int num = 0;
        detection* dets = get_network_boxes(net, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0.5, 0.5, NULL, 0, &num, 0);

        do_nms_sort(dets, num, 2, .45);

        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetDC(hwnd);
        int count = 0;

        std::vector<box> iconboxes;
        box* trashbox = NULL;

        //識別確認用矩形描画コード
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        HPEN hNewPen = (HPEN)CreatePen(PS_INSIDEFRAME, 4, RGB(0x00, 0xff, 0x00));
        HPEN hNewPenTrash = (HPEN)CreatePen(PS_INSIDEFRAME, 4, RGB(0x00, 0x00, 0xff));


        for (int j = 0; j < num; j++)
        {
            for (int i = 0; i < 2; i++)
            {
                if (dets[j].prob[i] > 0)
                {
                    if (i == 0)
                    {
                        if (!trashbox)
                            trashbox = &dets[j].bbox;
                    }
                    else
                        iconboxes.push_back(dets[j].bbox);

                    HPEN hOldPen = (HPEN)SelectObject(hdc, i == 0 ? hNewPenTrash : hNewPen);
                    box b = dets[j].bbox;
                    Rectangle(hdc,
                        b.x - b.w / 2,
                        b.y - b.h / 2,
                        b.x + b.w / 2,
                        b.y + b.h / 2);
                    SelectObject(hdc, hOldPen);
                }
            }
        }

        SelectObject(hdc, hOldBrush);
        DeleteObject(hNewPen);
        ReleaseDC(hwnd, hdc);
        
        if (trashbox)
        {
            for (auto b : iconboxes)
            {
                DragAndDrop(b.x, b.y, trashbox->x, trashbox->y);
            }
        }

        free_detections(dets, num);
        free_image(im);
		Sleep(1000);
	}
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   NOTIFYICONDATA nif;
   // タスクトレイに登録
   nif.cbSize = sizeof(NOTIFYICONDATA);
   nif.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
   nif.hWnd = hWnd;
   nif.uCallbackMessage = WM_TASKTRAY;
   nif.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
   nif.uID = ID_TASKTRAY;
   wcscpy_s(nif.szTip, 128, L"デスクトップアイコンターミネーター");

   Shell_NotifyIcon(NIM_ADD, &nif);

   unsigned int thID;
   HANDLE hTh;

   hTh = (HANDLE)_beginthreadex(NULL, 0, desktopwatcher, NULL, 0, &thID);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_MENU_QUIT:
				NOTIFYICONDATA nif;
				// タスクトレイから削除
				nif.cbSize = sizeof(NOTIFYICONDATA);
				nif.hWnd = hWnd;
				nif.uID = ID_TASKTRAY;

				Shell_NotifyIcon(NIM_DELETE, &nif);
				DestroyWindow(hWnd);
				break;
			default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_TASKTRAY:
		if (wParam == ID_TASKTRAY)
		{
			switch (lParam)
			{
			case WM_RBUTTONDOWN:
				{
					POINT pt;
					GetCursorPos(&pt);
					HMENU menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1));
					TrackPopupMenu(GetSubMenu(menu, 0), TPM_LEFTALIGN, pt.x, pt.y, NULL, hWnd, NULL);
					DestroyMenu(menu);
				}
				break;
			}
			break;
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
