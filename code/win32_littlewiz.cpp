#include <windows.h>
//#include <dsound.h> // ?

#if NOCRT
#include <intrin.h>
#include "nocrt/crt_float.cpp"
#include "nocrt/crt_math.cpp"
#include "nocrt/crt_memory.cpp"
#include "win32/win32_crt_fix.cpp"
#endif // NOCRT

#include "platform.h"
#include "lwd/littlewizarddata.h"
#include "win32/win32_helpers.cpp"
#include "win32/win32_input.cpp"

TCHAR MainWindowTitle[256] = T("Little Wiz: The Wiz Kid 0.0.04 - Data Edition");

int MainWindowWidth = 1280;
int MainWindowHeight = 720;

HWND MainWindowHandle;
WNDCLASSEX MainWindowClass;

struct win_backbuffer
{
	// windows pixels: 32-bit, upside-down, BB GG RR XX
	BITMAPINFO Info;
	texture buffer;
};

struct win_window_dimension
{
	int Width;
    int Height;
};

texture GlobalBackBuffer;
BITMAPINFO BackBufferInfo;

TCHAR GameCodeName[MAX_PATH];
TCHAR TempGameCodeName[MAX_PATH];

spritesheet sprites[MAX_SPRITES];
int sprites_loaded;

bool WindowFocused = true;
int GameRunning = false;

LARGE_INTEGER LastFrameTime;
LARGE_INTEGER PerformanceFrequency;
float TargetFPS;
#define PREVIOUSFRAMETIMES 10
real32 PreviousFrameTimes[PREVIOUSFRAMETIMES];

void* EndOfGlobalBackBuffer;

void win_updateInput()
{
	if(GetKeyState(VK_ESCAPE) & 0x8000
       && WindowFocused )
	{
		GameRunning = false;
		return;
	}
    
	// ALL buttons downprev
	for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
	{
		inputstate.button[ii].downprevious = inputstate.button[ii].down;
	}
    for(int ii = 0; ii < MAX_CONTROLLERS; ++ii)
    {
        for(int jj = 0; jj < CONTROLLER_BUTTONS; ++jj)
        {
            inputstate.controllers[ii].button[jj].down = false;
        }
    }
    
    // keyboard to virtualcontroller
	if(WindowFocused || INACTIVE_KEYBOARD_INPUT)
	{
		for(uint32 ii = 0; ii < DIRECTION_BUTTONS; ++ii)
		{
			int hits = 0;
			for(uint32 jj = 0; jj < directionmap[ii].totalbinds; ++jj)
			{
				if(GetKeyState(*(directionmap[ii].data + jj)) & 0x8000)
					hits++;
				inputstate.directions[ii].down = !!(GetKeyState(*(directionmap[ii].data + jj)) & 0x8000);
			}
			inputstate.directions[ii].down = !!hits;
		}
        
		for(uint32 ii = 0; ii < BUTTONS; ++ii)
		{
			int hits = 0;
			for(uint32 jj = 0; jj < buttonmap[ii].totalbinds; ++jj)
			{
				if(GetKeyState(*(buttonmap[ii].data + jj)) & 0x8000)
					hits++;
				inputstate.buttons[ii].down = !!(GetKeyState(*(buttonmap[ii].data + jj)) & 0x8000);
			}
			inputstate.buttons[ii].down += !!hits;
		}
        
	} else {
		for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
		{
			inputstate.button[ii].down = false;
		}
	}
    
    // xinput handling
    if(WindowFocused || INACTIVE_CONTROLLER_INPUT)
    {
        updateXInput();
    }
    
	// mouse
	inputstate.mousexprevious = inputstate.mousex;
	inputstate.mouseyprevious = inputstate.mousey;
    
	POINT mousepos;
	GetCursorPos(&mousepos);
	ScreenToClient(MainWindowHandle,&mousepos);
	inputstate.mousex = mousepos.x;
	inputstate.mousey = mousepos.y;
    
	// L, R, M, X1, X2
	// getting l and r through windows messaging for now
	if(WindowFocused) // || INACTIVE_INPUT)
	{
		inputstate.mousebutton[0].down = !!(GetKeyState(VK_LBUTTON) & 0x80);
		inputstate.mousebutton[1].down = !!(GetKeyState(VK_RBUTTON) & 0x80);
		inputstate.mousebutton[2].down = !!(GetKeyState(VK_MBUTTON) & 0x80);
		inputstate.mousebutton[3].down = !!(GetKeyState(VK_XBUTTON1) & 0x80);
		inputstate.mousebutton[4].down = !!(GetKeyState(VK_XBUTTON2) & 0x80);
	}
    
	for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
	{
		inputstate.button[ii].wentdown = (inputstate.button[ii].down > 0
                                          && inputstate.button[ii].downprevious == 0);
		inputstate.button[ii].wentup = (inputstate.button[ii].down == 0
                                        && inputstate.button[ii].downprevious > 0);
	}
}

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Return = 0;
    
	switch(uMsg)
	{
		case WM_QUIT:
		case WM_DESTROY:
		case WM_CLOSE:
        {
            GameRunning = false;
        } break;
        
        case WM_ACTIVATE:
        {
        	WindowFocused = !!wParam;
        }
        
		//WM_NCRBUTTONDOWN nHittest:HTCAPTION
		case WM_NCLBUTTONDBLCLK:	// doubleclick maximize
        case WM_NCRBUTTONDOWN:		// right click freezing
        case WM_CONTEXTMENU:		// right click menu
        {
        	// disable this stuff
        } break;
        
        case WM_SYSCOMMAND:
        case WM_COMMAND:
        {
        	WPARAM sysParam = wParam & 0xFFF0;
        	switch(sysParam)
        	{
        		case SC_MOUSEMENU:
        		case SC_KEYMENU:
        		{
        			//if(lParam)
        		} break;
        		default:
                Return = DefWindowProc(hwnd, uMsg, wParam, lParam);
                break;
        	}
            
        } break;
		default:
        Return = DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
	}
    
	return(Return);
}

win_window_dimension win_getWindowDimension(HWND Window)
{
    win_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return(Result);
}

// TODO: this is totally broken
void win_wait()
{
	LARGE_INTEGER currentframe;
	long long sleeptime;
    
	QueryPerformanceCounter(&currentframe);
    
    sleeptime = TargetFPS - ((1000.0f*((currentframe.QuadPart - LastFrameTime.QuadPart))/(float)PerformanceFrequency.QuadPart));
    
	if(sleeptime > 0)
	{
		if(sleeptime > 16)
		{
			sleeptime = 16;
		}
		Sleep((DWORD)sleeptime);
	}
    
    QueryPerformanceCounter(&LastFrameTime);
}

void
win_resizeDIBSection(texture *Buffer, int Width, int Height)
{
	Buffer->width = Width;
	Buffer->height = Height;
	Buffer->bpp = 4;
	Buffer->pitch = GlobalBackBuffer.bpp * GlobalBackBuffer.width;
}

void
win_clearBuffer(texture *Buffer)
{
	unsigned int* pixels = (uint32 *)Buffer->memory;
    
	unsigned int pixelssize = Buffer->width * Buffer->height;
    
	for(unsigned int ii = 0; ii < pixelssize; ++ii)
	{
		pixels[ii] = 0x00000000;
	}
}

void
win_swapBuffer(texture *Buffer, HDC DeviceContext)
{
	StretchDIBits( DeviceContext,
                  0, 0, Buffer->width, Buffer->height,
                  0, 0, Buffer->width, Buffer->height,
                  GlobalBackBuffer.memory, &BackBufferInfo,
                  DIB_RGB_COLORS, SRCCOPY );
}

void
win_processMessages()
{
	MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GameRunning = false;
            } break;
            
            case WM_INPUT:
            {
                updateRawInput(&Message); // needed this fast?
                UINT dwSize;
                
                GetRawInputData((HRAWINPUT)Message.lParam, RID_INPUT, NULL, &dwSize, 
                                sizeof(RAWINPUTHEADER));
                BYTE rawdata[256] = {};
                if (dwSize == NULL || dwSize > 256)
                {
                    // this looks bad??
                    return;
                } 
                
                if (GetRawInputData((HRAWINPUT)Message.lParam, RID_INPUT, rawdata, &dwSize, 
                                    sizeof(RAWINPUTHEADER)) != dwSize )
                {
                    OutputDebugString (TEXT("GetRawInputData does not return correct size !\n"));
                    return; // still looks bad
                }
                
                debug_ps4_controller* ps4data = (debug_ps4_controller*)&rawdata[31];
                
                
                RAWINPUTHEADER header = {0};
                UINT hSize = sizeof(RAWINPUTHEADER);
                GetRawInputData((HRAWINPUT)Message.lParam, RID_HEADER, &header, &hSize, 
                                sizeof(RAWINPUTHEADER));
                
                uint8 handleid = 0;
                

                
                
                if(ConnectedControllers[1].type == 0)
                {
                    handleid=1;
                }
                
                // match to controller
                if(header.hDevice == ConnectedControllers[handleid].handle)
                {
                    
                    inputstate.controllers[0].a.down = !!(rawdata[0x25] & 0x10);
                    inputstate.controllers[0].a.downprevious = inputstate.controllers[0].a.down;
                    
                    for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
                    {
                        inputstate.controllers[0].button[ii].wentdown = (inputstate.controllers[0].button[ii].down > 0
                                                                         && inputstate.button[ii].downprevious == 0);
                        inputstate.controllers[0].button[ii].wentup = (inputstate.button[ii].down == 0
                                                                       && inputstate.controllers[0].button[ii].downprevious > 0);
                    }
                }
                // do the input
                
                dwSize = 255;
                
            } break;
            
            case WM_SYSCOMMAND:
            case WM_COMMAND:
            {
                WPARAM sysParam = Message.wParam & 0xFFF0;
                switch(sysParam)
                {
	        		case SC_KEYMENU:
	        		{
                        //if(lParam)
	        		} break;
	        		case SC_MOUSEMENU:
	        		{
                        //
	        		} break;
	        		default:
                    TranslateMessage(&Message);
                    DispatchMessage(&Message); // A?
                    break;
                }
                
            } break;
            
            case WM_PAINT:
            {
                //ClearWindow;
                
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(MainWindowHandle, &Paint);
                win_window_dimension Dimension = win_getWindowDimension(MainWindowHandle);
                win_swapBuffer( &GlobalBackBuffer, DeviceContext );
                EndPaint(MainWindowHandle, &Paint);
            } break;
            case WM_MOUSEWHEEL:
            {
                if(WindowFocused)
                {
            		inputstate.mousewheel = GET_WHEEL_DELTA_WPARAM(Message.wParam);
                }
            }
#if 0
            case WM_LBUTTONDOWN:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[0].down = true;
                inputstate.mousebutton[0].wentdown = true;
                //inputstate.mousebutton[0].wentup = false;
            } break;
            
            case WM_LBUTTONUP:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[0].down = false;
                inputstate.mousebutton[0].wentup = true;
                //inputstate.mousebutton[0].wentdown = false;
            } break;
            
            case WM_RBUTTONDOWN:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[1].down = true;
                inputstate.mousebutton[1].wentdown = true;
                //inputstate.mousebutton[0].wentup = false;
            } break;
            
            case WM_RBUTTONUP:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[1].down = false;
                inputstate.mousebutton[1].wentup = true;
                //inputstate.mousebutton[0].wentdown = false;
            } break;
            
#endif
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message); // A?
            } break;
        }
    }
}

HWND
win_createwindow(HINSTANCE hInstance)
{
    HWND result = {0};
    win_resizeDIBSection(&GlobalBackBuffer,MainWindowWidth,MainWindowHeight);
    
    RECT lpRect = {100 , 100, (LONG)GlobalBackBuffer.width + 100,
        (LONG)GlobalBackBuffer.height + 100};
    DWORD dwStyle = (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX))
        | WS_VISIBLE;
    DWORD dwExStyle = 0;
    AdjustWindowRectEx( &lpRect, dwStyle, 0, dwExStyle);
    
    result = CreateWindowEx(dwExStyle,
                            MainWindowClass.lpszClassName,
                            MainWindowTitle,
                            dwStyle,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            lpRect.right - lpRect.left,
                            lpRect.bottom - lpRect.top,
                            0, 0, hInstance, 0);
    return(result);
}

int
win_init(HINSTANCE hInstance)
{
    // register window class
    MainWindowClass.style 			= CS_HREDRAW | CS_VREDRAW;
    MainWindowClass.lpfnWndProc 	= WindowProc;
    MainWindowClass.hInstance 		= hInstance;
    MainWindowClass.hCursor 		= LoadCursor(0, IDC_ARROW);
    MainWindowClass.hbrBackground 	= (HBRUSH)GetStockObject(BLACK_BRUSH);
    MainWindowClass.lpszClassName 	= TEXT("RandomWindowClass");
    
    // making sure everything is correct for ex
    MainWindowClass.lpszMenuName 	= NULL;
    MainWindowClass.hIcon 			= LoadIcon(NULL, IDI_APPLICATION);
    MainWindowClass.cbSize 		= sizeof(WNDCLASSEX);
    MainWindowClass.hIconSm 		= LoadIcon(NULL, IDI_APPLICATION);
    
    if(!RegisterClassEx(&MainWindowClass))
    {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"),
                   MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    
    // create the window
    MainWindowHandle = win_createwindow(hInstance);
    
    if(MainWindowHandle == NULL)
    {
        unsigned long error = GetLastError();
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
                   MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    
    //refresh rate stuff
    HDC hdc = GetDC(NULL);
    long long WinRefreshRate = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(NULL, hdc);
    TargetFPS = 1000.0f/WinRefreshRate;
    QueryPerformanceFrequency(&PerformanceFrequency);
    
    timeBeginPeriod(1);
    QueryPerformanceCounter(&LastFrameTime);
    
    int BitmapMemorySize = (GlobalBackBuffer.bpp * GlobalBackBuffer.width * GlobalBackBuffer.height);
    GlobalBackBuffer.memory = VirtualAlloc(0, BitmapMemorySize,                                           MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    BackBufferInfo.bmiHeader.biSize = sizeof(BackBufferInfo.bmiHeader);
    BackBufferInfo.bmiHeader.biWidth = GlobalBackBuffer.width;
    BackBufferInfo.bmiHeader.biHeight = -GlobalBackBuffer.height; // this seems to work fine
    BackBufferInfo.bmiHeader.biPlanes = 1;
    BackBufferInfo.bmiHeader.biBitCount = 32;
    BackBufferInfo.bmiHeader.biCompression = BI_RGB;
    
    // TODO: DSound disabled
    //win_initDSound(gWindowHandle);
    
#if _UNICODE
    TCHAR directory[MAX_PATH] = {0};
    TCHAR gamename[MAX_PATH] = {0};
    
    GetModuleFileName( NULL, directory, MAX_PATH );
    
    size_t stringlength = _tcslen(directory);
#if 0
    TCHAR* underscorelocation = _tcsrchr(directory,'_');
    TCHAR* periodlocation = _tcsrchr(directory,'.');
    TCHAR* slashlocation = _tcsrchr(directory,'\\');
    
    if(underscorelocation != 0)
    {
        _tcsncpy(gamename,underscorelocation+1,periodlocation-underscorelocation-1);
        // lstrcpyn(gamename,underscorelocation+1,periodlocation-underscorelocation-1);
    } else {
        _tcsncpy(gamename,slashlocation+1,periodlocation-slashlocation-1);
        // lstrcpyn(gamename,slashlocation+1,periodlocation-slashlocation-1);
    }
    *(slashlocation + 1) = '\0'; // leaving garbage after, gets cleaned on copy
    
#else
	// ghetto fucking assumption lol
    // TODO: fix dll renaming shit
    directory[stringlength-13] = 0;
    _tcscat(gamename, T("littlewiz"));
#endif
    _tcscpy(TempGameCodeName, directory);
    _tcscat(TempGameCodeName, gamename);
    _tcscat(TempGameCodeName, L".dll");
    
    _tcscpy(GameCodeName, directory);
    _tcscat(GameCodeName, L".");
    _tcscat(GameCodeName, gamename);
    _tcscat(GameCodeName, L"_");
    DWORD pid;
    GetWindowThreadProcessId(MainWindowHandle, &pid);
    TCHAR pids[8];
    
#ifndef NOCRT
    _itow((int)pid,pids,10);
#else
    wsprintf(pids, T("%u"), pid);
#endif
    
    _tcscat(GameCodeName, pids);
    _tcscat(GameCodeName, T(".dll"));
    
    // convert to 16-bit unicode
    //int widecharsize = MultiByteToWideChar(CP_ACP, 0, GameCodeName, -1, GameCodeNameW, 0);
    //MultiByteToWideChar(CP_ACP, 0, GameCodeName, -1, GameCodeNameW, widecharsize);
    
    //widecharsize = MultiByteToWideChar(CP_ACP, 0, TempGameCodeName, -1, TempGameCodeNameW, 0);
    //MultiByteToWideChar(CP_ACP, 0, TempGameCodeName, -1, TempGameCodeNameW, widecharsize);

#else
    
    // TODO: clean up game code name shit?
    // assuming it's something like win32_game.exe
    // and we're looking for game.dll
    char directory[MAX_PATH] = {0};
    char gamename[MAX_PATH] = {0};
    
    GetModuleFileName( NULL, directory, MAX_PATH );
    size_t stringlength = strlen(directory);
    char* underscorelocation = strrchr(directory,'_');
    char* periodlocation = strrchr(directory,'.');
    char* slashlocation = strrchr(directory,'\\');
    if(underscorelocation != 0)
    {
        strncpy(gamename,underscorelocation+1,periodlocation-underscorelocation-1);
    } else {
        strncpy(gamename,slashlocation+1,periodlocation-slashlocation-1);
    }
    *(slashlocation + 1) = '\0'; // leaving garbage after, gets cleaned on copy
    
    strcpy(TempGameCodeName, directory);
    strcat(TempGameCodeName, gamename);
    strcat(TempGameCodeName, ".dll");
    
    strcpy(GameCodeName, directory);
    strcat(GameCodeName, ".");
    strcat(GameCodeName, gamename);
    strcat(GameCodeName, "_");
    DWORD pid;
    GetWindowThreadProcessId(MainWindowHandle, &pid);
    char pids[8];
    
    itoa((int)pid,pids,10);
    
    strcat(GameCodeName, pids);
    strcat(GameCodeName, ".dll");
#endif
    
    //default keybinds
    // Arrow keys to virtual dpad
    win_bindKey(&directionmap[0], VK_UP);
    win_bindKey(&directionmap[1], VK_DOWN);
    win_bindKey(&directionmap[2], VK_LEFT);
    win_bindKey(&directionmap[3], VK_RIGHT);
    
    // WASD also to virtyal dpad
    win_bindKey(&directionmap[0], 'W'); // w
    win_bindKey(&directionmap[1], 'S'); // s
    win_bindKey(&directionmap[2], 'A'); // a
    win_bindKey(&directionmap[3], 'D'); // d
    
    win_bindKey(&buttonmap[0], ' '); // spacebar
    win_bindKey(&buttonmap[1], VK_LCONTROL); // ?
    win_bindKey(&buttonmap[2], '2');
    win_bindKey(&buttonmap[3], '3');
    win_bindKey(&buttonmap[4], 'Q'); // q
    win_bindKey(&buttonmap[5], 'E'); // e
    
    // xinput
    win_initXInput();
    win_initRawInput();
	
	// TODO: load files from gamecode?
	// TODO: make loading images less retarded
	// TODO: add proper fucking animation support
    #if 0
    loadspritesheet(T("font.png"),  32,  32, 11, 10, sprites, &sprites_loaded, 0); // 0
    loadspritesheet(T("brick.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 1);
    loadspritesheet(T("front.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("stairup.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 1);
    loadspritesheet(T("portraitholder.png"), 160, 160, 1, 1, sprites, &sprites_loaded, 2); // 4
    loadspritesheet(T("portrait.png"), 160, 160, 1, 1, sprites, &sprites_loaded, 2);
    loadspritesheet(T("neutral.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 2);
    loadspritesheet(T("heart0.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 2);
    loadspritesheet(T("staff.png"), 256, 256, 1, 1, sprites, &sprites_loaded, 2); // 8
    loadspritesheet(T("death6.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("fire1.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3); // 10
    loadspritesheet(T("fire2.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("fire3.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3); // 12
    loadspritesheet(T("firefull1.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("firefull2.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("firefull3.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("fireexp1.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3); // 16
    loadspritesheet(T("fireexp2.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("fireexp3.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("fireexp4.png"), 64, 64, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("front2.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3); // 20
    loadspritesheet(T("front3.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("death1.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("death2.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("death3.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3); // 24
    loadspritesheet(T("death4.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("death5.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("faces.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("column.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3); // 28
    loadspritesheet(T("nowizards.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("rockk.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    loadspritesheet(T("orb.png"), 128, 128, 1, 1, sprites, &sprites_loaded, 3);
    #endif
    
    // load the file
    uc8* lfp = 0;
    lfp = (uc8*)loadfile(T("littlewizard.lwd"));

    // get info from header
    littlewizarddataheader* lwdh = (littlewizarddataheader*)lfp;
    uint32 fileentries = lwdh->filesfound;//*(uint32*)(lfp+4);
    uint32 imagedataoffset = lwdh->imagedo;//*(uint32*)(lfp+8);
    uint32 spritesheetsoffset = lwdh->spritedo;//*(uint32*)(lfp+12);
    uint32 optimizationsoffset = lwdh->optimizationsdo;//*(uint32*)(lfp+16);
    
    // fixup the pointers
    spritesheet* loadedsprites = (spritesheet*)(lfp+spritesheetsoffset);
    uint8* imagedatapointer = (uint8*)(lfp+imagedataoffset);
    uint8* optimizationdatapointer = (uint8*)(lfp+imagedataoffset);
    for(uint32 ii = 0; ii < fileentries; ++ii)
    {
		loadedsprites[ii].tex.memory = imagedatapointer;
        imagedatapointer += loadedsprites[ii].tex.width*loadedsprites[ii].tex.height*loadedsprites[ii].tex.bpp;
        if(loadedsprites[ii].optimizations != 0)
        {
            loadedsprites[ii].optimizations = optimizationdatapointer;
            optimizationdatapointer += 2*sizeof(uint8)*loadedsprites[ii].tex.width;
        }
    }
    
    // TODO: make files order-independant
    // assign the order
    uint32 temporaryordering[] = {
        31, 0, 19, 30, 27, 26, 23, 22, 29, 7,
        9, 10, 11, 16, 17, 18, 12, 13, 14, 15,
        20, 21, 2, 3, 4, 5, 6, 8, 1, 24, 28, 25
    };
    for(uint32 ii = 0; ii < fileentries; ++ii)
    {
        sprites[ii] = loadedsprites[temporaryordering[ii]];
    }
    
    lfp = lfp;
    
    return 0;
}

void WindowsKill()
{
    timeEndPeriod(1);
}


struct win32_gamecode
{
    HMODULE gamecode;
    FILETIME writetime;
    
    game_update_and_render *UpdateAndRender;
};

unsigned int
scaleToWindow(unsigned int height, unsigned int border)
{
    unsigned int Result = (MainWindowHeight - 2 * border) / height;
    
    return(Result);
}

bool
LoadGameCode(win32_gamecode* GameCode)
{
    WIN32_FILE_ATTRIBUTE_DATA FileTimeData;
    // TODO: set a game code location variable?
    GetFileAttributesEx(TempGameCodeName, GetFileExInfoStandard, &FileTimeData);
    FILETIME newtime = FileTimeData.ftLastWriteTime;
    
    if(CompareFileTime(&newtime, &GameCode->writetime) != 0)
    {
        // unload
        FreeLibrary(GameCode->gamecode);
        GameCode->gamecode = 0;
        
        // load
        CopyFile(TempGameCodeName,GameCodeName,FALSE);
        GameCode->gamecode = LoadLibrary(GameCodeName);
        GameCode->UpdateAndRender = (game_update_and_render *)GetProcAddress(GameCode->gamecode, "GameUpdateAndRender");
        GameCode->writetime = newtime;
        
        return true;
    }
    return false;
}

int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    GameRunning = true;
    
    // Start up Windows Platform
    if(win_init(hInstance) != 0)
    {
        GameRunning = false;
    }
    
    // TODO: make this not garbage, just a hack for streaming on left mon for now
    //SetWindowPos(MainWindowHandle, HWND_TOP, -1600, 0, MainWindowWidth, MainWindowHeight, SWP_NOSIZE);
    
    game_memory GameMemory = {};
    win32_gamecode GameCode = {};
#if 1
    GameMemory.persistsize = GAME_MEMORY_SIZE;
    GameMemory.tempmemsize = TEMP_MEMORY_SIZE;
    
    uint64 persistsize = GameMemory.persistsize + GameMemory.tempmemsize;
    
    GameMemory.persist = (uint8*)VirtualAlloc(0, persistsize,
                                              MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    GameMemory.tempmem = GameMemory.persist + GameMemory.persistsize;
    
    GameMemory.SCREEN_WIDTH = MainWindowWidth;
    GameMemory.SCREEN_HEIGHT = MainWindowHeight;
    GameMemory.sprites = &sprites[0];
#endif
    
    while(GameRunning != 0)
    {
        
        // check game dll and swap if needed
        GameMemory.swappedgamecode = LoadGameCode(&GameCode);
        
        // input
        win_updateInput();
        win_processMessages();
        
        // debug
		LARGE_INTEGER debug_LastFrameTime;
        LARGE_INTEGER currentframe;
        
        // this is retarded
        float averagefps = 0;
        for(int ii = 1; ii < PREVIOUSFRAMETIMES; ++ii)
        {
            averagefps += PreviousFrameTimes[ii];
            PreviousFrameTimes[ii-1] = PreviousFrameTimes[ii];
        }
        
        QueryPerformanceCounter(&currentframe);
        
        PreviousFrameTimes[PREVIOUSFRAMETIMES-1] = 1000.0f/(1000.0f*((currentframe.QuadPart - debug_LastFrameTime.QuadPart))/(float)PerformanceFrequency.QuadPart);
        
        QueryPerformanceCounter(&debug_LastFrameTime);
        
        averagefps += PreviousFrameTimes[PREVIOUSFRAMETIMES-1];
        averagefps /= (float)PREVIOUSFRAMETIMES;
        
        //sprintf(GameMemory.debug_fps, "%#5.1f", averagefps);
        
        // update and render game
        GameCode.UpdateAndRender(&GameMemory, &inputstate, &GlobalBackBuffer);
        GameMemory.timer++;
        
        // finish rendering
        HDC DeviceContext = GetDC(MainWindowHandle);        
        win_swapBuffer( &GlobalBackBuffer, DeviceContext );
        //SwapBuffers(DeviceContext); // the actual swap?
        
        
        // clean up
        ReleaseDC(MainWindowHandle, DeviceContext);
        
        if(GameMemory.shuttingdown)
        {
        	GameRunning = 0;
        }
        
        // shitty vsync
        win_wait(); // this is still very bad
    }
    
    FreeLibrary(GameCode.gamecode);
    DeleteFile(GameCodeName);
    
    return 0;
}
