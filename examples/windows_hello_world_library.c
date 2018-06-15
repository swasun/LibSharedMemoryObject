/************************************************************************************
* Copyright (C) 2018 by Charly Lamothe												*
*																					*
* This file is part of LibSharedMemorySlot.                                         *
*																					*
*   LibSharedMemorySlot is free software: you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by			*
*   the Free Software Foundation, either version 3 of the License, or				*
*   (at your option) any later version.												*
*																					*
*   LibSharedMemorySlot is distributed in the hope that it will be useful,          *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of					*
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					*
*   GNU General Public License for more details.									*
*																					*
*   You should have received a copy of the GNU General Public License               *
*   along with LibSharedMemorySlot.  If not, see <http://www.gnu.org/licenses/>.    *
************************************************************************************/

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
