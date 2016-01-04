#include <SDL.h>
#include <SDL_render.h>
#include <iostream>
#include <vector>
#include "Chip8.h"
#include "Screen.h"

using namespace std;

int main(int argc, char** argv)
{

	Chip8 *machine = new Chip8();

	machine->Initialize();
	machine->LoadRom("Roms/INVADERS");

	machine->Run();
	delete machine;
	
	return 0;
}