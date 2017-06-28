/*
	This file manages user input from the keyboard or mouse
*/

#ifndef EAE6320_USERINPUT_H
#define EAE6320_USERINPUT_H

// Interface
//==========

namespace eae6320
{
	namespace UserInput
	{
		// For standard letter or number keys, the representative ascii char can be used:
		// IsKeyPressed( 'A' ) or IsKeyPressed( '6' )

		// For arrow keys use:
		// VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN

		// As of this comment, a list of all codes for these functions can be found at:
		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
		// (if that doesn't work, try searching for "Virtual Key Codes")

		// Note that these codes are in Winuser.h,
		// and this interface is _not_ platform-independent.

		bool IsKeyPressed( const int i_virtualKeyCode );
		bool IsMouseButtonPressed( const int i_virtualButtonCode );
	}
}

#endif	// EAE6320_USERINPUT_H
