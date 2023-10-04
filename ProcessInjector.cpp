#include <Windows.h>
#include <stdio.h>

int Error(const char* text, DWORD error = GetLastError()) {
	printf("%s (%u)\n", text, error);
	return 1;
}

bool HollowingInjection(int pid, wchar_t* dllpath)
{
	LPSTARTUPINFOA si = new STARTUPINFOA();
	LPPROCESS_INFORMATION pi = new PROCESS_INFORMATION();
	//PROCESS_BASIC_INFORMATION* pbi = new PROCESS_BASIC_INFORMATION();

	return true;
}

bool ClassicalInjection(DWORD pid, LPCSTR dllpath)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,
		FALSE, pid);

	if (!hProcess) {
		return Error("Error in opening process\n");
	}

	// Allocate memory in the remote process.
	LPVOID buffer = VirtualAllocEx(hProcess, 0, strlen(dllpath) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!buffer) {
		return Error("Failed to allocate memory\n");
	}

	// Write the DLL path to the remote process.
	if (!WriteProcessMemory(hProcess, buffer, (LPVOID)dllpath, strlen(dllpath)+1 , 0)) {
		return Error("Failed to write to process\n");
	}

	// Create a remote thread to load the DLL.
	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA"),
		buffer, 0, 0);

	if (!hThread) {
		return Error("Failed to create thread\n");
	}

	// Wait for the remote thread to finish executing.
	WaitForSingleObject(hThread, INFINITE);

	// Close the remote handles.
	CloseHandle(hThread);
	CloseHandle(hProcess);

	// Print a success message.
	printf("Successfully injected the DLL !!");

	return true;
}


                                                                                                        

int main(int argc, const wchar_t* argv[])
{
	printf(
		"======================================\n"
		"[-] Select your injection method : \n"
		" |-- [+] 1.Classical RemoteThread\n"
		" |-- [+] 2.Process Hollowing\n");
	int input;
	DWORD pid = 0;
	LPCSTR dllpath = "C:\\Users\\theob\\Desktop\\workshop\\ProcessInjector\\x64\\Release\\injecteddll.dll";

	scanf_s("%d", &input);

	switch (input)
	{
	case 1:
		printf("[+]Classical injection selected\n");
		printf("[+]Enter Pid:\n");
		scanf_s("%d", &pid);
		//printf("[+]Enter DllPath:\n");
		//scanf_s("%s", dllpath, strlen(dllpath));
	
		// Inject the DLL.
		ClassicalInjection(pid, dllpath);
		break;
	default:
		Error("Injection type not recognize...");
	}

	return 0;
}