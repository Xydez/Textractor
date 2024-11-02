#include "mainwindow.h"
#include "types.h"
#include "module.h"
#include <winhttp.h>
#include <strsafe.h>
#include <shellapi.h>
#include <QFileInfo>

#include <iostream>

extern const wchar_t* UPDATE_AVAILABLE;
extern const wchar_t* CL_OPTIONS;

int main(int argc, char *argv[])
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Print a newline
	const wchar_t* nl = L"\n";
	WriteConsoleW(hStdout, nl, wcslen(nl), DUMMY, NULL);

	std::vector<DWORD> attachProcessIds;
	std::vector<QFileInfo> extensionPaths;

	// FIXME: For some reason, when called from the command line, the program freezes until the user presses enter. I suspect the problem is in exception.cpp.

	int args_count = 0;
	std::unique_ptr<LPWSTR[], Functor<LocalFree>> args(CommandLineToArgvW(GetCommandLineW(), &args_count));

	// Parse command line arguments
	if (args_count > 1) {
		auto processes = GetAllProcesses();

		for (int i = 1; i < args_count; ++i) {
			std::wstring arg = args[i];

			if (arg[0] != L'/' && arg[0] != L'-') {
				wchar_t buffer[512];
				StringCchPrintfW(buffer, sizeof(buffer), L"Invalid argument %s: all arguments must start with `-` or `/`\n", arg.c_str());
				WriteConsoleW(hStdout, buffer, wcslen(buffer), DUMMY, NULL);

				return 1;
			}

			if (
				(arg[0] == '/' && arg[1] == '?')
				|| (arg[0] == '-' && arg[1] == 'h')
				|| (arg[0] == '-' && arg[1] == '-' && arg.substr(2).compare(L"help") == 0)
				) {
				// Print help
				WriteConsoleW(hStdout, CL_OPTIONS, wcslen(CL_OPTIONS), DUMMY, NULL);
				WriteConsoleW(hStdout, nl, wcslen(nl), DUMMY, NULL);

				return 1;
			}
			else if (arg[1] == L'p' || arg[1] == L'P') {
				// Add process attach

				std::wstring argValue = arg.substr(2);
				if (argValue.empty()) {
					// PRINT ERROR EMPTY PROCESS ID/NAME
					const wchar_t* message = L"Argument `-p` must be followed by a process name or id";
					WriteConsoleW(hStdout, message, wcslen(message), DUMMY, NULL);

					return 2;
				}
				else if (DWORD processId = wcstoul(argValue.c_str(), nullptr, 0)) {
					attachProcessIds.push_back(processId);
				}
				else {
					bool found = false;
					for (auto& [processId, processName] : processes) {
						if (!processName.has_value())
							continue;

						auto& processNameValue = processName.value();

						if (processNameValue.find(L"\\" + arg.substr(2)) != std::string::npos) {
							wchar_t buffer[512];
							StringCchPrintfW(buffer, sizeof(buffer), L"Argument \"%s\" matched process \"%s\" (process id %d)", argValue.c_str(), processNameValue.c_str(), processId);
							WriteConsoleW(hStdout, buffer, wcslen(buffer), DUMMY, NULL);

							attachProcessIds.push_back(processId);
							found = true;
						}
					}

					if (!found) {
						wchar_t buffer[512];
						StringCchPrintfW(buffer, sizeof(buffer), L"Failed to find any process matching argument \"%s\"", argValue.c_str());
						WriteConsoleW(hStdout, buffer, wcslen(buffer), DUMMY, NULL);

						return 4;
					}
				}
			}
			else if (arg[1] == L'x' || arg[1] == L'X') {
				// Add extensions

				std::wstring argValue = arg.substr(2);
				if (argValue.empty()) {
					const wchar_t* message = L"Argument `-p` must be followed by a process name or id";
					WriteConsoleW(hStdout, message, wcslen(message), DUMMY, NULL);

					return 2;
				}
				else {
					QFileInfo extensionPath = QString::fromWCharArray(argValue.c_str());
					if (extensionPath.exists() && extensionPath.isFile()) {
						wchar_t buffer[512];
						StringCchPrintfW(buffer, sizeof(buffer), L"Appending extension file \"%s\"", argValue.c_str());
						WriteConsoleW(hStdout, buffer, wcslen(buffer), DUMMY, NULL);

						extensionPaths.push_back(extensionPath);
					}
					else {
						wchar_t buffer[512];
						StringCchPrintfW(buffer, sizeof(buffer), L"Extension file \"%s\" does not exist", argValue.c_str());
						WriteConsoleW(hStdout, buffer, wcslen(buffer), DUMMY, NULL);

						return 5;
					}
				}
			}
			else {
				wchar_t buffer[512];
				StringCchPrintfW(buffer, sizeof(buffer), L"Unknown parameter \"%s\"", arg);
				WriteConsoleW(hStdout, buffer, wcslen(buffer), DUMMY, NULL);

				return 3;
			}
		}
	}

	SettingsPack settingsPack = { attachProcessIds, extensionPaths };

	std::thread([]
	{
		if (!*VERSION) return;
		using InternetHandle = AutoHandle<Functor<WinHttpCloseHandle>>;
		// Queries GitHub releases API https://developer.github.com/v3/repos/releases/ and checks the last release tag to check if it's the same
		if (InternetHandle internet = WinHttpOpen(L"Mozilla/5.0 Textractor", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0))
			if (InternetHandle connection = WinHttpConnect(internet, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0))
				if (InternetHandle request = WinHttpOpenRequest(connection, L"GET", L"/repos/Artikash/Textractor/releases", NULL, NULL, NULL, WINHTTP_FLAG_SECURE))
					if (WinHttpSendRequest(request, NULL, 0, NULL, 0, 0, NULL))
					{
						char buffer[1000] = {};
						WinHttpReceiveResponse(request, NULL);
						WinHttpReadData(request, buffer, 1000, DUMMY);
						if (abs(strstr(buffer, "/tag/") - strstr(buffer, VERSION)) > 10) TEXTRACTOR_MESSAGE(UPDATE_AVAILABLE);
					}
	}).detach();

	QDir::setCurrent(QFileInfo(S(GetModuleFilename().value())).absolutePath());

	QApplication app(argc, argv);
	app.setFont(QFont("MS Shell Dlg 2", 10));

	MainWindow win(settingsPack);
	win.show();

	return app.exec();
}
