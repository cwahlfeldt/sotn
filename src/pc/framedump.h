// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef PC_FRAMEDUMP_H
#define PC_FRAMEDUMP_H

#include <stdbool.h>

void FrameDump_OnFrame(void);
void FrameDump_OnInitGear(void);

// true for a headless test run driven by SOTN_INPUT or SOTN_EXIT_AFTER
bool FrameDump_IsScripted(void);

#endif
