/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */


#include "Global.h"
#include "win32_handmade.h"
#include "Game.h"




// TODO(casey): This is a global for now.

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return(Result);
}
//
//internal void
//RenderWeirdGradient(win32_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
//{
//    // TODO(casey): Let's see what the optimizer does
//
//	SendStringToDebug("RenderWeirdGradient");
//
//    uint8 *Row = (uint8 *)Buffer->Memory;    
//    for(int Y = 0;
//        Y < Buffer->Height;
//        ++Y)
//    {
//        uint32 *Pixel = (uint32 *)Row;
//        for(int X = 0;
//            X < Buffer->Width;
//            ++X)
//        {
//            uint8 Blue = (X + BlueOffset);
//            uint8 Green = (Y + GreenOffset);
//
//            *Pixel++ = ((Green << 8) | Blue);
//        }
//        
//        Row += Buffer->Pitch;
//    }
//}

//internal void ClearBuffer(win32_offscreen_buffer *Buffer)
//{
//	int32_t *Pixel = (int32_t*)Buffer->Memory;
//
//	for (int i = 0; i < Buffer->Width * Buffer->Height; i++)
//	{
//		//int32_t Red = RGB(255,0,0);
//		//int32_t Green = RGB(0, 255, 0);
//		//int32_t Blue = RGB(0, 0, 255);
//
//		Pixel[i] = 0;   //RGB(0,0,0);
//
//	}
//}
//
//internal void DrawRectangle(win32_offscreen_buffer *Buffer, int X, int Y, int Width, int Height, int32_t Color)
//{
//	
//	int32_t* Pixel = (int32_t*)Buffer->Memory;
//	for (int j = max(Y, 0); j < Y + Height && j < Buffer->Height; j++)
//	{
//		for (int i = max(X, 0); i < X + Width && i < Buffer->Width; i++)
//		{
//			Pixel[(j-1)*Buffer->Width + i] = Color;
//		}
//	}
//}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	// Initialize the buffer.

	// Free Buffer.Memory if it is not already empty.
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;

    int BytesPerPixel = 4;

    // NOTE(casey): When the biHeight field is negative, this is the clue to
    // Windows to treat this bitmap as top-down, not bottom-up, meaning that
    // the first three bytes of the image are the color for the top left pixel
    // in the bitmap, not the bottom left!
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    Buffer->Pitch = Width*BytesPerPixel;
    // TODO(casey): Probably clear this to black
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
	SendStringToDebug("Win32DisplayBufferInWindow");

	//OutputDebugStringA("Win32DisplayBufferInWindow");
	// TODO(casey): Aspect ratio correction
    // TODO(casey): Play with stretch modes
    StretchDIBits(DeviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  */
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Memory,
                  &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{       
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_CLOSE:
        {
            // TODO(casey): Handle this with a message to the user?
            GlobalRunning = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            // TODO(casey): Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;

		case WM_KEYDOWN:
		{
			uint32 VKCode = WParam;
			if (VKCode == 'W')
			{
				KeysDown.Up = 1;
			}
			else if (VKCode == 'A')
			{
				KeysDown.Left = 1;
			}
			else if (VKCode == 'S')
			{
				KeysDown.Down = 1;
			}
			else if (VKCode == 'D')
			{
				KeysDown.Right = 1;
			}
			else if (VKCode == VK_SPACE)
			{
				KeysDown.Space = 1;
			}
		} break;

		case WM_KEYUP:
		{
			uint32 VKCode = WParam;
			if (VKCode == 'W')
			{
				KeysDown.Up = 0;
			}
			else if (VKCode == 'A')
			{
				KeysDown.Left = 0;
			}
			else if (VKCode == 'S')
			{
				KeysDown.Down = 0;
			}
			else if (VKCode == 'D')
			{
				KeysDown.Right = 0;
			}
			else if (VKCode == VK_SPACE)
			{
				KeysDown.Space = 0;
			}
		} break;

        //case WM_SYSKEYDOWN:
        //case WM_SYSKEYUP:
        //case WM_KEYDOWN:
        //case WM_KEYUP:
        //{
        //    uint32 VKCode = WParam;
        //    bool WasDown = ((LParam & (1 << 30)) != 0);
        //    bool IsDown = ((LParam & (1 << 31)) == 0);
        //    if(WasDown != IsDown)
        //    {
        //        if(VKCode == 'W')
        //        {
        //        }
        //        else if(VKCode == 'A')
        //        {
        //        }
        //        else if(VKCode == 'S')
        //        {
        //        }
        //        else if(VKCode == 'D')
        //        {
        //        }
        //        else if(VKCode == 'Q')
        //        {
        //        }
        //        else if(VKCode == 'E')
        //        {
        //        }
        //        else if(VKCode == VK_UP)
        //        {
        //        }
        //        else if(VKCode == VK_LEFT)
        //        {
        //        }
        //        else if(VKCode == VK_DOWN)
        //        {
        //        }
        //        else if(VKCode == VK_RIGHT)
        //        {
        //        }
        //        else if(VKCode == VK_ESCAPE)
        //        {
        //            OutputDebugStringA("ESCAPE: ");
        //            if(IsDown)
        //            {
        //                OutputDebugStringA("IsDown ");
        //            }
        //            if(WasDown)
        //            {
        //                OutputDebugStringA("WasDown");
        //            }
        //            OutputDebugStringA("\n");
        //        }
        //        else if(VKCode == VK_SPACE)
        //        {
        //        }
        //    }
        //} break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext,
                                       Dimension.Width, Dimension.Height);
            EndPaint(Window, &Paint);
        } break;

        default:
        {
//            OutputDebugStringA("default\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

void SendStringToDebug(char *Message)
{
	LARGE_INTEGER Counts;
	QueryPerformanceCounter(&Counts);
	char MsgBuffer[256];
	wsprintf(MsgBuffer, "%d\n", (int)Counts.QuadPart);
	OutputDebugStringA(Message);
	OutputDebugStringA(MsgBuffer);
}


int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
	// QueryPerformanceCounter() returns clock time in "counts". We can convert this to seconds with QueryPerformanceFrequency,
	// which returns counts per second (constant and determined at boot).
	// (counts/sec) / (frames/sec) = (count/frame)
	LARGE_INTEGER CountsPerSecond;
	QueryPerformanceFrequency(&CountsPerSecond);
	int CountsPerFrame = (int)CountsPerSecond.QuadPart / TargetFPS;

	// Initialize the back buffer.
    Win32ResizeDIBSection(&GlobalBackBuffer, GameWindowWidth, GameWindowHeight);
    
	// Define the WindowClass
	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "Handmade Hero", WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);
        if(Window)
        {
            // NOTE(casey): Since we specified CS_OWNDC, we can just get one device context and use it forever because we
            // are not sharing it with anyone.
            HDC DeviceContext = GetDC(Window);

			game_offscreen_buffer GameBuffer = {};
			GameBuffer.Height = GlobalBackBuffer.Height;
			GameBuffer.Width = GlobalBackBuffer.Width;
			GameBuffer.Pitch = GlobalBackBuffer.Pitch;
			GameBuffer.Memory = GlobalBackBuffer.Memory;

			// Make this static inside Game.cpp?
			game_state GameState = {};
			GameStateInitialize(&GameState);

			GlobalRunning = true;
			LARGE_INTEGER LastCycleStart;
			QueryPerformanceCounter(&LastCycleStart);

			// **** The Main game loop
            while(GlobalRunning)
            {
				// Loop for as long as we have messages.
				MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                
				LARGE_INTEGER CurrentTime;
				QueryPerformanceCounter(&CurrentTime);
				
				int CountsSinceLastCycle = (int32)(CurrentTime.QuadPart - LastCycleStart.QuadPart);
				if (CountsSinceLastCycle >= CountsPerFrame)
				{
					GameStateProcess(&GameState, &KeysDown, &GameBuffer);
					//int32 MSeconds = (int32)(1000 * (CurrentTime.QuadPart - LastCycleStart.QuadPart) / CountsPerSecond.QuadPart);
					//char MsgBuffer[256];
					//wsprintf(MsgBuffer, "Milliseconds: %dms\n", MSeconds);
					//OutputDebugStringA(MsgBuffer);

					RenderBuffer(&GameState, &GameBuffer);
					//RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
					LastCycleStart = CurrentTime;
				}


                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext,
                                           Dimension.Width, Dimension.Height);
				//LastCycleStart = CurrentTime;
			}
			
        }
        else
        {
            // TODO(casey): Logging
        }
    }
    else
    {
        // TODO(casey): Logging
    }
    
    return(0);
}

