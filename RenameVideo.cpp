#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>
#include <iostream>
#include <thread>
#include <iomanip>
#include <sstream>
#include <shlobj.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

namespace fs = std::filesystem;

HWND hMainWnd = NULL;
HWND hStatusLabel = NULL;
HWND hProgressBar = NULL;
HWND hRunBtn = NULL;
HWND hPathEdit = NULL;
HWND hBrowseBtn = NULL;
HWND hRecursiveCheck = NULL;
HWND hVidTrackEdit = NULL;
HWND hAudTrackEdit = NULL;
HWND hSubTrackEdit = NULL;

struct ThreadData {
    HWND hWnd;
    std::string targetPath;
    bool recursive;
    std::string vName;
    std::string aName;
    std::string sName;
};

std::string ExecuteCommand(const std::string& cmd) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hStdOutRead, hStdOutWrite;
    if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) return "";

    SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdOutWrite;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    std::string fullCmd = "cmd.exe /c " + cmd;
    
    std::vector<char> cmdVec(fullCmd.begin(), fullCmd.end());
    cmdVec.push_back(0);

    if (!CreateProcessA(NULL, cmdVec.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hStdOutRead);
        CloseHandle(hStdOutWrite);
        return "";
    }

    CloseHandle(hStdOutWrite);

    std::string output;
    char buffer[4096];
    DWORD bytesRead;
    while (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }

    CloseHandle(hStdOutRead);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return output;
}

std::string GetDateString() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[100];
    sprintf_s(buf, "%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    return std::string(buf);
}


std::string GetConfigPath() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    fs::path p(exePath);
    return (p.parent_path() / "config.ini").string();
}

std::string CleanFilename(std::string title) {
    if (title.empty()) return "";
    std::regex invalid_chars(R"([<>:"/\\|?*])");
    title = std::regex_replace(title, invalid_chars, "-");
    
    size_t pos = 0;
    while ((pos = title.find("&")) != std::string::npos) {
        title.replace(pos, 1, "and");
    }
    
    while (!title.empty() && isspace(title.back())) title.pop_back();
    while (!title.empty() && isspace(title.front())) title.erase(title.begin());

    if (title.length() > 240) title = title.substr(0, 240);
    return title;
}

bool SetTrackTitles(const std::string& filepath, const std::string& vName, const std::string& aName, const std::string& sName) {
    if (filepath.substr(filepath.length() >= 4 ? filepath.length() - 4 : 0) != ".mkv") return false;

    if (!vName.empty()) ExecuteCommand("mkvpropedit \"" + filepath + "\" --edit track:v1 --set name=\"" + vName + "\"");
    if (!aName.empty()) ExecuteCommand("mkvpropedit \"" + filepath + "\" --edit track:a1 --set name=\"" + aName + "\"");
    if (!sName.empty()) ExecuteCommand("mkvpropedit \"" + filepath + "\" --edit track:s1 --set name=\"" + sName + "\"");
    
    return true;
}

std::string RenameFile(const std::string& filepath, const std::string& title, std::ofstream& log_file) {
    std::string clean_title = CleanFilename(title);
    if (clean_title.empty()) return "";

    std::string directory = fs::path(filepath).parent_path().string();
    std::string extension = fs::path(filepath).extension().string();
    std::string original_filename = fs::path(filepath).filename().string();

    std::string new_name = clean_title + extension;
    std::string new_path = directory + "\\" + new_name;

    if (original_filename == new_name) return filepath;

    int counter = 1;
    while (fs::exists(new_path) && filepath != new_path) {
        new_name = clean_title + " (" + std::to_string(counter) + ")" + extension;
        new_path = directory + "\\" + new_name;
        counter++;
    }

    try {
        fs::rename(filepath, new_path);
        return new_path;
    } catch (...) {
        return "";
    }
}

void ProcessRename(ThreadData* data) {
    std::string log_filename = "rename_log_" + GetDateString() + ".txt";
    std::ofstream log_file(log_filename);
    
    log_file << "Log proses penggantian nama dan pengaturan track" << "\n========================================\n";

    std::vector<std::string> exts = {".mkv", ".mp4", ".avi", ".mov", ".wmv", ".flv", ".webm"};
    int total_files = 0;
    std::vector<std::string> files_to_process;

    try {
        if (data->recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(data->targetPath)) {
                if (!entry.is_regular_file()) continue;
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                for (const auto& e : exts) if (e == ext) files_to_process.push_back(entry.path().string());
            }
        } else {
            for (const auto& entry : fs::directory_iterator(data->targetPath)) {
                if (!entry.is_regular_file()) continue;
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                for (const auto& e : exts) if (e == ext) files_to_process.push_back(entry.path().string());
            }
        }
    } catch (...) {
        MessageBoxA(data->hWnd, "Gagal membaca direktori.", "Error", MB_OK | MB_ICONERROR);
    }

    total_files = files_to_process.size();
    if (total_files == 0) {
        SendMessageA(hStatusLabel, WM_SETTEXT, 0, (LPARAM)"Tidak ada video ditemukan");
        MessageBoxA(data->hWnd, "Tidak ada file video ditemukan.", "Info", MB_OK);
        EnableWindow(hRunBtn, TRUE);
        EnableWindow(hBrowseBtn, TRUE);
        delete data;
        return;
    }

    int files_processed = 0, files_renamed = 0, files_tracks_set = 0, files_skipped = 0;

    for (const auto& fpath : files_to_process) {
        std::string filename = fs::path(fpath).filename().string();
        std::string msg = "Memproses: " + filename;
        SendMessageA(hStatusLabel, WM_SETTEXT, 0, (LPARAM)msg.c_str());

        log_file << "\nMemproses: " << filename << "\n";
        
        std::string cmd = "ffprobe -v quiet -show_entries format_tags=title,TITLE,Title,name,NAME,Name -of default=noprint_wrappers=1:nokey=1 \"" + fpath + "\"";
        std::string output = ExecuteCommand(cmd);
        
        if (output.empty() || output.find_first_not_of(" \r\n\t") == std::string::npos) {
            cmd = "ffprobe -v quiet -show_entries stream_tags=title,TITLE,Title,name,NAME,Name -of default=noprint_wrappers=1:nokey=1 \"" + fpath + "\"";
            output = ExecuteCommand(cmd);
        }

        size_t pos = output.find_first_of("\r\n");
        if (pos != std::string::npos) {
            output = output.substr(0, pos);
        }

        std::string title = output;

        if (!title.empty()) {
            std::string new_fpath = RenameFile(fpath, title, log_file);
            if (!new_fpath.empty()) {
                if (new_fpath != fpath) {
                    files_renamed++;
                }
                
                if (fs::path(new_fpath).extension().string() == ".mkv") {
                    if (SetTrackTitles(new_fpath, data->vName, data->aName, data->sName)) {
                        files_tracks_set++;
                    }
                }
            } else {
                files_skipped++;
            }
        } else {
            files_skipped++;
        }

        files_processed++;
        SendMessage(hProgressBar, PBM_SETPOS, (files_processed * 100) / total_files, 0);
    }

    log_file << "\n========================================\nSelesai! Diproses: " << files_processed;

    std::string finalMsg = "Proses selesai!\n" + std::to_string(files_processed) + " file diproses.\n" +
                           std::to_string(files_renamed) + " diganti nama.\n" +
                           std::to_string(files_skipped) + " dilewati.\n" +
                           std::to_string(files_tracks_set) + " MKV track diatur.";
    
    MessageBoxA(data->hWnd, finalMsg.c_str(), "Selesai", MB_OK);
    SendMessageA(hStatusLabel, WM_SETTEXT, 0, (LPARAM)"Selesai");
    EnableWindow(hRunBtn, TRUE);
    EnableWindow(hBrowseBtn, TRUE);
    delete data;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            DragAcceptFiles(hwnd, TRUE);
            
            CreateWindowA("STATIC", "Target Folder:", WS_VISIBLE | WS_CHILD, 10, 10, 280, 20, hwnd, NULL, NULL, NULL);
            hPathEdit = CreateWindowA("EDIT", fs::current_path().string().c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 10, 30, 230, 22, hwnd, NULL, NULL, NULL);
            hBrowseBtn = CreateWindowA("BUTTON", "...", WS_VISIBLE | WS_CHILD, 250, 30, 40, 22, hwnd, (HMENU)2, NULL, NULL);
            
            hRecursiveCheck = CreateWindowA("BUTTON", "Scan Sub-folder", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 60, 200, 20, hwnd, NULL, NULL, NULL);
            
            hStatusLabel = CreateWindowA("STATIC", "Status: Menunggu...", WS_VISIBLE | WS_CHILD, 10, 200, 280, 20, hwnd, NULL, NULL, NULL);
            hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 10, 225, 280, 20, hwnd, NULL, NULL, NULL);
            SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

            hRunBtn = CreateWindowA("BUTTON", "Mulai Proses", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 100, 260, 100, 30, hwnd, (HMENU)1, NULL, NULL);
            
            CreateWindowA("STATIC", "Ubah Nama Track MKV (Kosongkan jika skip):", WS_VISIBLE | WS_CHILD, 10, 85, 280, 20, hwnd, NULL, NULL, NULL);
            char defVid[256], defAud[256], defSub[256];
            std::string iniPath = GetConfigPath();
            GetPrivateProfileStringA("Settings", "Video", "Video Track", defVid, 256, iniPath.c_str());
            GetPrivateProfileStringA("Settings", "Audio", "Audio Track", defAud, 256, iniPath.c_str());
            GetPrivateProfileStringA("Settings", "Sub", "Subtitle Track", defSub, 256, iniPath.c_str());
            
            CreateWindowA("STATIC", "Video:", WS_VISIBLE | WS_CHILD, 10, 110, 50, 20, hwnd, NULL, NULL, NULL);
            hVidTrackEdit = CreateWindowA("EDIT", defVid, WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 110, 230, 22, hwnd, NULL, NULL, NULL);
            CreateWindowA("STATIC", "Audio:", WS_VISIBLE | WS_CHILD, 10, 140, 50, 20, hwnd, NULL, NULL, NULL);
            hAudTrackEdit = CreateWindowA("EDIT", defAud, WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 140, 230, 22, hwnd, NULL, NULL, NULL);
            CreateWindowA("STATIC", "Sub:", WS_VISIBLE | WS_CHILD, 10, 170, 50, 20, hwnd, NULL, NULL, NULL);
            hSubTrackEdit = CreateWindowA("EDIT", defSub, WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 170, 230, 22, hwnd, NULL, NULL, NULL);
            break;
        }
        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            char path[MAX_PATH];
            if (DragQueryFileA(hDrop, 0, path, MAX_PATH)) {
                if (fs::is_directory(path)) {
                    SetWindowTextA(hPathEdit, path);
                } else {
                    SetWindowTextA(hPathEdit, fs::path(path).parent_path().string().c_str());
                }
            }
            DragFinish(hDrop);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 2) {
                BROWSEINFOA bi = {0};
                bi.hwndOwner = hwnd;
                bi.lpszTitle = "Pilih Folder Video";
                bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
                if (pidl != 0) {
                    char path[MAX_PATH];
                    SHGetPathFromIDListA(pidl, path);
                    SetWindowTextA(hPathEdit, path);
                    CoTaskMemFree(pidl);
                }
            }
            if (LOWORD(wParam) == 1) {
                EnableWindow(hRunBtn, FALSE);
                EnableWindow(hBrowseBtn, FALSE);
                SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
                
                char tPath[MAX_PATH], tVid[256], tAud[256], tSub[256];
                GetWindowTextA(hPathEdit, tPath, MAX_PATH);
                GetWindowTextA(hVidTrackEdit, tVid, 256);
                GetWindowTextA(hAudTrackEdit, tAud, 256);
                GetWindowTextA(hSubTrackEdit, tSub, 256);
                bool recursive = SendMessage(hRecursiveCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
                
                std::string iniPath = GetConfigPath();
                WritePrivateProfileStringA("Settings", "Video", tVid, iniPath.c_str());
                WritePrivateProfileStringA("Settings", "Audio", tAud, iniPath.c_str());
                WritePrivateProfileStringA("Settings", "Sub", tSub, iniPath.c_str());
                
                ThreadData* data = new ThreadData{hwnd, tPath, recursive, tVid, tAud, tSub};
                std::thread(ProcessRename, data).detach();
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    InitCommonControls();
    
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(101));
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszClassName = "RenameAppClass";
    RegisterClassA(&wc);

    hMainWnd = CreateWindowA(
        wc.lpszClassName, "Rename Video", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 340,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
