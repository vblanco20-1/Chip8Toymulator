#include "Keypad.h"



unsigned char Keypad::WaitForKeypress()
{
	while (true)
	{		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (SDL_KEYDOWN == event.type)
			{
				unsigned char cd = ScancodeToChar(event.key.keysym.sym);
				if (cd != 16)
				{
					return cd;
				}
			}
		}
	}
}

void Keypad::UpdateInput(SDL_Event *event)
{	
	if (SDL_KEYDOWN == event->type)
	{
		unsigned char cd = ScancodeToChar(event->key.keysym.sym);
		if (cd != 16)
		{
			keypad[cd] = true;
		}
	}
	else if (SDL_KEYUP == event->type)
	{
		unsigned char cd = ScancodeToChar(event->key.keysym.sym);
		if (cd != 16)
		{
			keypad[cd] = false;
		}
	}	
}

bool Keypad::IsKeyPressed(unsigned char key)
{	
	return keypad[key];
}

unsigned char Keypad::ScancodeToChar(SDL_Keycode scancode)
{
	unsigned char code = 16;
	
	switch (scancode)
	{
	case SDLK_1:
		code = 0x1;		
		break;
	case SDLK_2:
		code = 0x2;
		break;
	case SDLK_3:
		code = 0x3;
		break;
	case SDLK_4:
		code = 0xC;
		break;

	case SDLK_q:
		code = 0x4;
		break;
	case SDLK_w:
		code = 0x5;
		break;
	case SDLK_e:
		code = 0x6;
		break;
	case SDLK_r:
		code = 0xD;
		break;

	case SDLK_a:
		code = 0x7;
		break;
	case SDLK_s:
		code = 0x8;
		break;
	case SDLK_d:
		code = 0x9;
		break;
	case SDLK_f:
		code = 0xE;
		break;

	case SDLK_z:
		code = 0xA;
		break;
	case SDLK_x:
		code = 0x0;
		break;
	case SDLK_c:
		code = 0xB;
		break;
	case SDLK_v:
		code = 0xF;
		break;

	default:
		code = 16;
		break;
	}

	return code;
}

Keypad::Keypad()
{
}


Keypad::~Keypad()
{
}
