#pragma once
#include <SDL.h>
class Keypad
{
public:

	unsigned char WaitForKeypress();
	void UpdateInput(SDL_Event* event);
	bool IsKeyPressed(unsigned char key);

	unsigned char ScancodeToChar(SDL_Keycode scancode);
	unsigned char keypad[16];

	Keypad();
	~Keypad();
};

