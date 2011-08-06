#include "output_color.h"

#if defined(WIN32)
#include <Windows.h>

void set_color(E_COLOR color)
{
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)color);
}

#else

void set_color(E_COLOR color)
{
	char *color_code;
	switch(color)
	{
	case CLR_WHITE:
		color_code = "0";
		break;
	case CLR_RED:
		color_code = "31";
		break;
	case CLR_GREEN:
		color_code = "32";
		break;
	}
	std::cout << "33[" << color_code << "m";
}

#endif