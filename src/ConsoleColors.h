#pragma once

#define ENABLE_CONSOLE_COLOR_CODES

#ifdef ENABLE_CONSOLE_COLOR_CODES
	static const char* RedColor = "\x1B[31m";
	static const char* GreenColor = "\x1B[32m";
	static const char* YellowColor = "\x1B[33m";
	static const char* WhiteColor = "\x1B[37m";
	static const char* MagentaColor = "\x1B[35m";
	static const char* CyanColor = "\x1B[36m";
#else
	static const char* RedColor = "";
	static const char* GreenColor = "";
	static const char* YellowColor = "";
	static const char* WhiteColor = "";
	static const char* MagentaColor = "";
	static const char* CyanColor = "";
#endif
