/*******************************************************************************
 * Copyright (C) 2018 Charly Lamothe                                           *
 *                                                                             *
 * This file is part of LibSharedMemoryObject.                                 *
 *                                                                             *
 *   Licensed under the Apache License, Version 2.0 (the "License");           *
 *   you may not use this file except in compliance with the License.          *
 *   You may obtain a copy of the License at                                   *
 *                                                                             *
 *   http://www.apache.org/licenses/LICENSE-2.0                                *
 *                                                                             *
 *   Unless required by applicable law or agreed to in writing, software       *
 *   distributed under the License is distributed on an "AS IS" BASIS,         *
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 *   See the License for the specific language governing permissions and       *
 *   limitations under the License.                                            *
 *******************************************************************************/

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
