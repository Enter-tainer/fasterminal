#include <windows.h>
#include <iostream>
#include <functional>
#include <cstdlib>
using namespace std;
void GetDesktopResolution(int& horizontal, int& vertical)
{
  RECT desktop;
  // Get a handle to the desktop window
  const HWND hDesktop = GetDesktopWindow();
  // Get the size of screen to the variable desktop
  GetWindowRect(hDesktop, &desktop);
  // The top left corner will have coordinates (0,0)
  // and the bottom right corner will have coordinates
  // (horizontal, vertical)
  horizontal = desktop.right;
  vertical = desktop.bottom;
}
std::string GetLastErrorAsString()
{
  //Get the error message, if any.
  DWORD errorMessageID = ::GetLastError();
  if(errorMessageID == 0)
    return std::string(); //No error message has been recorded

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);

  //Free the buffer.
  LocalFree(messageBuffer);

  return message;
}

void startWT() {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  char cmdstr[] = "explorer.exe shell:AppsFolder\\Microsoft.WindowsTerminal_8wekyb3d8bbwe!App";
  auto res = CreateProcess( NULL,   // No module name (use command line)
      cmdstr,            // Command line
      NULL,              // Process handle not inheritable
      NULL,              // Thread handle not inheritable
      FALSE,             // Set handle inheritance to FALSE
      0,                 // No creation flags
      NULL,              // Use parent's environment block
      NULL,              // Use parent's starting directory
      &si,               // Pointer to STARTUPINFO structure
      &pi );
  if (!res) {
    cout << GetLastErrorAsString() << endl;
    exit(1);
  }
  Sleep(1000);
}

int main () {
  FreeConsole();
  startWT();
  auto hd = FindWindowA("CASCADIA_HOSTING_WINDOW_CLASS", nullptr);
  cout << hd << endl;
  RECT rec;
  auto err = RegisterHotKey(NULL, 0, MOD_NOREPEAT | MOD_ALT | MOD_WIN, VK_RETURN);
  if (!err) {
    cout << GetLastErrorAsString() << endl;
    exit(1);
  }
  bool show = false;
  GetWindowRect(hd, &rec);
  auto width = rec.right - rec.left,
       height = rec.bottom - rec.top;
  int screenW, screenH;
  GetDesktopResolution(screenW, screenH);
  MSG msg = {0};
  cout << width << 'x' << height << ' ' << screenW << 'x' << screenH << endl;
  auto hideTerminal = [&]() {
    if(!GetWindowRect(hd, &rec)) {
      cout << GetLastErrorAsString() << endl;
      exit(1);
    }
    SetWindowPos(hd, HWND_BOTTOM, screenW - width, 0,
        width, height, SWP_HIDEWINDOW);
  };
  auto showTerminal = [&]() {
    if(!GetWindowRect(hd, &rec)) {
      cout << GetLastErrorAsString() << endl;
      exit(1);
    }
    SetWindowPos(hd, HWND_TOPMOST, screenW - width, 0,
        width, height, SWP_SHOWWINDOW);
    SetForegroundWindow(hd);
  };
  hideTerminal();
  while (GetMessage(&msg, NULL, 0, 0)) {
    cout << "get msg" << msg.message << endl;
    if (msg.message == WM_HOTKEY) {
      cout << "win+enter pressed" << endl;
      if (show) {
        // then hide the window;
        hideTerminal();
      } else {
        // show the window;
        showTerminal();
      }
      show = !show;
    }
  }
}
