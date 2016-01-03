#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>
using namespace std;


Chip8::Chip8()
{
}

void UpdateTimer60hz(Chip8 machine)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
		
		

	}
}

void PrintDissasembly(unsigned short opcode,std::string fname, int K)
{

	if (bDissasembly)
	{
		cout << std::hex << opcode << "=" << fname << ": " << "K = " << std::hex << K << endl;
	}
}

void PrintDissasembly(unsigned short opcode, std::string fname, char Vx, char Vy)
{
	if (bDissasembly)
	{
		cout << std::hex << opcode << "=" << fname << ": " << "Vx = " << std::hex << unsigned int( Vx) << "; Vy = " << std::hex << unsigned int(Vy) << endl;
	}
}

void PrintDissasemblyConstant(unsigned short opcode, std::string fname, char Vx, int constant)
{
	if (bDissasembly)
	{
		cout << std::hex << opcode <<  "="<<fname << ": " << "Vx = " << std::hex << unsigned int( Vx) << "; Constant = " << std::hex <<( constant) << endl;
	}
}

Chip8::~Chip8()
{
}

void Chip8::uninplemented(unsigned short opcode)
{
	cout << "uninplemented: " << opcode << endl;
}

void Chip8::Initialize()
{
	InitializeFunctionPointers();

	for (int m = 0; m < 80; m++)
	{
		memory[m] = chip8_fontset[m];
	}
	pc = 0x200;  // Program counter starts at 0x200
	//opcode = 0;      // Reset current opcode	
	I = 0;      // Reset index register
	sp = 0;      // Reset stack pointer

				 // Clear display	
				 // Clear stack
				 // Clear registers V0-VF
				 // Clear memory

				 // Load fontset
	//for (int i = 0; i < 80; ++i)
	//	memory[i] = chip8_fontset[i];

	// Reset timers


}

void Chip8::InitializeFunctionPointers()
{
	for (int i = 0; i < 16; i++)
	{
		FunctionArray[i] = nullptr;
		ArithmeticFunctions[i] = nullptr;
		OtherFunctions[i] = nullptr;
	}

	FunctionArray[0] = nullptr; //need other table
	FunctionArray[1] = &Chip8::jmp;
	FunctionArray[2] = &Chip8::jsr;
	FunctionArray[3] = &Chip8::skeqc;
	FunctionArray[4] = &Chip8::sknec;
	FunctionArray[5] = &Chip8::skeqr;
	FunctionArray[6] = &Chip8::movc;
	FunctionArray[7] = &Chip8::addc;
	FunctionArray[8] = nullptr; // 8 its for arithmetic, that goes in other array
	FunctionArray[9] = &Chip8::skner;
	FunctionArray[10] = &Chip8::mvi; //a
	FunctionArray[11] = &Chip8::jmi; //b
	FunctionArray[12] = &Chip8::randc; //c
	FunctionArray[13] = &Chip8::sprite; //d
	FunctionArray[14] = nullptr; //e special functions for input
	FunctionArray[15] = nullptr; //f others, need other table


	
	OtherFunctions[7] = &Chip8::gdelay;
	OtherFunctions[0xA] = &Chip8::keyr;	
	OtherFunctions[8] = &Chip8::ssound;
	OtherFunctions[0xE] = &Chip8::adi;
	OtherFunctions[0x9] = &Chip8::font;
	OtherFunctions[0x0] = &Chip8::xfont;
	OtherFunctions[0x3] = &Chip8::bcd;
	OtherFunctions[0xE] = &Chip8::adi;


	ArithmeticFunctions[0] = &Chip8::movr;
	ArithmeticFunctions[1] = &Chip8::_or;
	ArithmeticFunctions[2] = &Chip8::_and;
	ArithmeticFunctions[3] = &Chip8::_xor;
	ArithmeticFunctions[4] = &Chip8::addr;
	ArithmeticFunctions[5] = &Chip8::sub;
	ArithmeticFunctions[6] = &Chip8::shr;
	ArithmeticFunctions[7] = &Chip8::rsb;
	ArithmeticFunctions[0xE] = &Chip8::shl;

}

void Chip8::EmulateCycle()
{

	//fetch
	unsigned short opcode = FetchInstruction();
	//decode

	if (opcode != 0)
	{
		Ch8Function fun = GetFunctionFromOPCode(opcode);
		//execute (jesus christ the function pointer syntax)
		if (fun != nullptr)
		{
			((this)->*(fun))(opcode);
		}
		

		
	}
	//cout << std::hex << opcode << endl;
	if (drawFlag)
	{
		screen.DrawFrame();
		drawFlag = false;
	}
		
	
}

void Chip8::Run()
{
	int t = 0;
	while (true)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{

			if ((SDL_QUIT == event.type) ||
				(SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode))
			{
				return;
			}
			else
			{
				keypad.UpdateInput(&event);
			}
			
		}


		EmulateCycle();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		t += 1;
		if (t % 10 == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (t >= 1000 / 60)
		{
			t = 0;
			if (delay_timer > 0)
			{
				delay_timer--;
			}
			if (sound_timer > 0)
			{
				sound_timer--;
			}			
		}		
	}
}

bool Chip8::LoadRom(std::string filename)
{


	ifstream rom;
	rom.open(filename, ios::binary);

	if (rom.is_open())
	{
		rom.seekg(0, ios::end);
		auto size = rom.tellg();
		auto memblock = new char[size];
		rom.seekg(0, ios::beg);
		rom.read(memblock, size);
		memcpy(&memory[512], memblock, size);
		//for (int m = 0; m < size; m++)
		//{
		//	memory[512 + m] = memblock[m];
		//	//cout << memblock[m];
		//}

		cout << "ROM successfully loaded: " << filename << endl;

		
		

		delete[] memblock;
		return true;
	}

	return false;
}

unsigned short Chip8::FetchInstruction()
{
	unsigned short r = memory[pc] << 8 | memory[pc + 1];
	pc += 2;
	return r;

}

void Chip8::noop(unsigned short opcode)
{
	PrintDissasembly(opcode, "NOOP", 0);
}

void Chip8::cls(unsigned short opcode)
{
	PrintDissasembly(opcode, "CLS", 0); screen.Clear(); drawFlag = true;
}

void Chip8::rts(unsigned short opcode)
{

	PrintDissasembly(opcode, "RET", 0);
	stack[sp] = 0;
	pc = stack[sp - 1];
	sp--;
}

void Chip8::jmp(unsigned short opcode)
{
	PrintDissasembly(opcode, "JMP", opcode & 0x0FFF); pc = opcode & 0x0FFF;
}

void Chip8::jsr(unsigned short opcode)
{
	PrintDissasembly(opcode, "JSR", opcode & 0x0FFF); stack[sp] = pc; sp++; pc = opcode & 0x0FFF;
}

void Chip8::skeqc(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short kk = (opcode & 0x00FF);

	PrintDissasemblyConstant(opcode, "SKEQC ", (opcode & 0x0F00) >> 8,kk);

	if (x == kk)
	{
		pc += 2;
	}
}

void Chip8::sknec(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short kk = (opcode & 0x00FF);
	PrintDissasemblyConstant(opcode, "SKNEC", (opcode & 0x0F00) >> 8, kk);
	if (x != kk)
	{
		pc += 2;
	}
}

void Chip8::skeqr(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];
	PrintDissasembly(opcode,"SKEQ ", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
	if (x == y)
	{
		pc += 2;
	}
}

void Chip8::movc(unsigned short opcode)
{

	char constant = opcode & 0x00FF;
	short reg = (opcode & 0x0F00) >> 8;

	PrintDissasemblyConstant(opcode, "MOV ", reg, constant);
	V[reg] = constant;
}

void Chip8::addc(unsigned short opcode)
{

	char constant = opcode & 0x00FF;
	short reg = (opcode & 0x0F00) >> 8;
	PrintDissasemblyConstant(opcode, "ADD ", reg, constant);
	V[reg] += constant;
}

void Chip8::movr(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vy];

	PrintDissasembly(opcode, "MOV ", vx, vx);
}

void Chip8::_or(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vx] | V[vy];
	PrintDissasembly(opcode, "OR ", vx, vx);
}

void Chip8::_and(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vx] & V[vy];
	PrintDissasembly(opcode, "AND ", vx, vx);
}

void Chip8::_xor(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = (V[vx] ^ V[vy]);
	PrintDissasembly(opcode, "XOR ", vx, vx);
}

void Chip8::addr(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vx] + V[vy];
	PrintDissasembly(opcode, "ADD ", vx, vx);
}

void Chip8::sub(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vx] - V[vy];
	PrintDissasembly(opcode, "SUB ", vx, vx);
}

void Chip8::shr(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vx] >> 1;
	PrintDissasembly(opcode, "SHR ", vx, vx);
}

void Chip8::rsb(unsigned short opcode)
{
	uninplemented(opcode);
}

void Chip8::shl(unsigned short opcode)
{
	char vy = (opcode & 0x00F0) >> 4;
	char vx = (opcode & 0x0F00) >> 8;
	V[vx] = V[vx] << 1;
	PrintDissasembly(opcode, "SHL ", vx, vx);
}

void Chip8::skner(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];

	PrintDissasembly(opcode, "SKNE ", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);

	if (x != y)
	{
		pc += 2;
	}
	//uninplemented(opcode);
}

void Chip8::mvi(unsigned short opcode)
{
	//remove first 4 bits
	I = opcode & 0x0FFF;
	PrintDissasembly(opcode, "MVI ", I);
	
}

void Chip8::jmi(unsigned short opcode)
{
	PrintDissasembly(opcode, "SHL ", (opcode & 0x0FFF));
	pc = V[0] + (opcode & 0x0FFF);
}

void Chip8::randc(unsigned short opcode)
{
	unsigned char rnd = rand() % 256;
	unsigned short x = (opcode & 0x0F00) >> 8;
	unsigned short kk= (opcode & 0x00FF);
	PrintDissasemblyConstant(opcode, "RND ", x, kk);

	V[x] = rnd & kk;
	//uninplemented(opcode);
}

void Chip8::sprite(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];
	unsigned short height = opcode & 0x000F;
	unsigned short pixel;
	PrintDissasembly(opcode, "DRW ", ((opcode & 0x0F00) >> 8), (opcode & 0x00F0) >> 4);
	V[0xF] = 0;
	for (int yline = 0; yline < height; yline++)
	{
		pixel = memory[I + yline];
		for (int xline = 0; xline < 8; xline++)
		{
			if ((pixel & (0x80 >> xline)) != 0)
			{
				if (pixel != 0)
				{
					bool c = screen.TogglePixel(x + xline, y + yline);
					if (c)
					{
						V[0xF] = 1;
						
					}
				}
					
			}
		}
	}
	if (V[0xF] == 0)
	{
		PrintDissasembly(opcode, "DRW - No Collision", ((opcode & 0x0F00) >> 8), (opcode & 0x00F0) >> 4);
	}
	else
	{
		PrintDissasembly(opcode, "DRW -  Collision", ((opcode & 0x0F00) >> 8), (opcode & 0x00F0) >> 4);

	}
	drawFlag = true;
	
}

void Chip8::skpr(unsigned short opcode)
{

	PrintDissasembly(opcode, "SKPR ", ((opcode & 0x0F00) >> 8));
	if (keypad.IsKeyPressed(V[(opcode & 0x0F00) >> 8])) {
		pc += 2;
	}
}

void Chip8::skup(unsigned short opcode)
{
	PrintDissasembly(opcode, "SKUP ", ((opcode & 0x0F00) >> 8));

	if (!keypad.IsKeyPressed(V[(opcode & 0x0F00) >> 8])) {
		pc += 2;
	}
}

void Chip8::gdelay(unsigned short opcode)
{
	 V[(opcode & 0x0F00) >> 8] = delay_timer;
	 PrintDissasembly(opcode, "LD VX DT", (opcode & 0x0F00) >> 8);
}

void Chip8::keyr(unsigned short opcode)
{

	PrintDissasembly(opcode, "KEYR ", ((opcode & 0x0F00) >> 8));
	V[(opcode & 0x0F00) >> 8] = keypad.WaitForKeypress();
}

void Chip8::sdelay(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	delay_timer = x;
	PrintDissasembly(opcode, "LD DT", (opcode & 0x0F00) >> 8);
}

void Chip8::ssound(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	sound_timer = x;
	PrintDissasembly(opcode, "LD ST", (opcode & 0x0F00) >> 8);
	//uninplemented(opcode);
}

void Chip8::adi(unsigned short opcode)
{
	I += V[(opcode & 0x0F00) >> 8];
	PrintDissasembly(opcode, "ADD I ", ((opcode & 0x0F00) >> 8));
}

void Chip8::font(unsigned short opcode)
{
	unsigned char f = (opcode & 0x0F00) >> 8;
	I = f * 5;

	PrintDissasembly(opcode, "LD F", (opcode & 0x0F00) >> 8);
}

void Chip8::xfont(unsigned short opcode)
{
	PrintDissasembly(opcode, "NOT FOR THIS MACHINEEEE", (opcode & 0x0F00) >> 8);
	//unsigned char f = (opcode & 0x0F00) >> 8;
	//I = f * 10;
}

void Chip8::bcd(unsigned short opcode)
{
	unsigned char r = (opcode & 0x0F00) >> 8;
	unsigned char Vr = V[r];
	int n100 = Vr / 100;
	int n10 = (Vr / 10) % 10;
	int n1 = Vr % 10;
	memory[I] = Vr / 100;
	memory[I + 1] = (Vr / 10) % 10 ;
	memory[I + 2] =Vr % 10;

	PrintDissasembly(opcode, "BCD", (opcode & 0x0F00) >> 8);
}

void Chip8::str(unsigned short opcode)
{
	unsigned char r = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= r; i++)
	{
		memory[I + i] = V[i];
		//V[i] = ;
	}
	PrintDissasembly(opcode, "STR", (opcode & 0x0F00) >> 8);
	//uninplemented(opcode);
}

void Chip8::ldr(unsigned short opcode)
{
	unsigned char r = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= r; i++)
	{
		V[i] = memory[I + i];
	}
	PrintDissasembly(opcode, "LDR", (opcode & 0x0F00) >> 8);
	//uninplemented(opcode);
}

Ch8Function Chip8::GetFunctionFromOPCode(const unsigned short opcode)
{
	char fn = (opcode & 0xF000) >> 12;

	auto function = FunctionArray[fn];
	if (function == nullptr) // other
	{
		if (fn == 8) //arithmetic
		{
			function = ArithmeticFunctions[opcode & 0x000F];
		}
		else if (fn == 0)
		{
			if (opcode == 0x00E0)
			{
				function = &Chip8::cls;
			}
			else if (opcode == 0x00EE)
			{
				function = &Chip8::rts;
			}
		}
		else if (fn == 14) //0xE, input functions
		{
			if ((opcode & 0x00FF) == 0x009E)
			{
				function = &Chip8::skpr;
			}
			else if ((opcode & 0x00FF) == 0x00A1)
			{
				function = &Chip8::skup;
			}
		}
		else if (fn == 15) // 0xF other functions
		{
			
			function = OtherFunctions[opcode & 0x000F];
			if (function == nullptr && (opcode & 0x000F) == 0x0005) // ending in 5 there is 3 possibilities
			{
				if ((opcode & 0x00F0) == 0x0010)
				{
					function = &Chip8::sdelay;
				}
				else if ((opcode & 0x00F0) == 0x0050)
				{
					function = &Chip8::str;
				}
				else if ((opcode & 0x00F0 )== 0x0060)
				{
					function = &Chip8::ldr;
				}
			}
		}
	}
	if (function == nullptr && opcode != 0)
	{
		
		cout << "opcode error, function not found, = " << opcode << endl;

		
	}
	if (opcode == 0)
	{
		function = &Chip8::noop;
	}
	return function;
}
