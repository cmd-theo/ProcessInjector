#include <Windows.h>
#include <stdio.h>

int Error(const char* text, DWORD error = GetLastError()) {
	printf("%s (%u)\n", text, error);
	return 1;
}

bool ClassicalInjection(int pid, wchar_t* dllpath)
{
	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD,
		FALSE, pid);

	if (!hProcess) {
		return Error("Error in opening process\n");
	}

	// Allocate memory in the remote process.
	void* buffer = VirtualAllocEx(hProcess, nullptr, 1 << 12, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!buffer) {
		return Error("Failed to allocate memory\n");
	}

	// Write the DLL path to the remote process.
	DWORD bytesWritten;
	if (!WriteProcessMemory(hProcess, buffer, dllpath, (wcslen(dllpath + 1) * sizeof(WCHAR)), nullptr)) {
		return Error("Failed to write to process\n");
	}

	// Create a remote thread to load the DLL.
	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32"), "LoadLibraryW"),
		buffer, 0, nullptr);

	if (!hThread) {
		return Error("Failed to create thread\n");
	}

	// Wait for the remote thread to finish executing.
	WaitForSingleObject(hThread, INFINITE);

	// Close the remote handles.
	CloseHandle(hThread);
	CloseHandle(hProcess);

	// Print a success message.
	printf("Successfully injected the DLL !");

	return true;
}


                                                                                                        

int wmain(int argc, const wchar_t* argv[])
{
	printf("[-] Welcome to   \n"
		" _______ _________ _       _________ _______  _______ _________ _______  _______        _  _        _   \n"
		"(  ____ )\\__   __/( (    /|\\__    _/(  ____ \\(  ____ \\\\__   __/(  ___  )(  ____ )      ( )/ )      ( \\  \n"
		"| (    )|   ) (   |  \\  ( |   )  (  | (    \\/| (    \\/   ) (   | (   ) || (    )|      |// /___  ___\\ \\ \n"
		"| (____)|   | |   |   \\ | |   |  |  | (__    | |         | |   | |   | || (____)|       / /(___)(___)) )\n"
		"|  _____)   | |   | (\\ \\) |   |  |  |  __)   | |         | |   | |   | ||     __)      ( (  ___  ___ | |\n"
		"| (         | |   | | \\   |   |  |  | (      | |         | |   | |   | || (\\ (          \\ \\(___)(___)) )\n"
		"| )      ___) (___| )  \\  ||\\_)  )  | (____/\\| (____/\\   | |   | (___) || ) \\ \\__        \\ \\        / / \n"
		"|/       \\_______/|/    )_)(____/   (_______/(_______/   )_(   (_______)|/   \\__/         \\_)      (_/  \n"
		"======================================\n"
		"[-] Select your injection method : \n"
		" |-- [+] 1.Classical RemoteThread\n"
		" |-- [+] 2.Process Hollowing\n");
	int input;
	int pid = -1;
	wchar_t dllpath[MAX_PATH];

	scanf_s("%d", &input);

	switch (input)
	{
	case 1:
		printf("[+]Classical injection selected\n");
		printf("[+]Enter Pid:\n");
		scanf_s("%d", &pid);
		printf("[+]Enter DllPath:\n");
		scanf_s("%s", dllpath, _countof(dllpath));

		// Check if the DLL path is valid.
		if (wcslen(dllpath) == 0) {
			Error("Invalid DLL path");
			return 1;
		}
		// Inject the DLL.
		ClassicalInjection(pid, dllpath);
		break;
	default:
		Error("Injection type not recognize...");
	}

	return 0;
}