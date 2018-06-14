#include <stdio.h>
#include <Windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// equivalent of __attribute__((constructor))...

		// return TRUE if succeeded, FALSE if you failed to initialize properly
		return TRUE; // I'm assuming you succeeded.
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		// equivalent of __attribute__((destructor))...
	}

	// Return value is ignored when fdwReason isn't DLL_PROCESS_ATTACH, so we'll
	// just return TRUE.
	return TRUE;
}

void hello_world(void) {
    fprintf(stdout, "Hello world from shared object !\n");
}
