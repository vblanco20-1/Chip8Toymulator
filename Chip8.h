#pragma once
#include "Screen.h"
#include "Keypad.h"
#include <string>
#include <iostream>
#include <functional>
class Chip8;
constexpr bool bDissasembly = true;



void PrintDissasembly(unsigned short opcode,std::string fname, int K);
void PrintDissasembly(unsigned short opcode,std::string fname, char Vx, char Vy );
void PrintDissasemblyConstant(unsigned short opcode,std::string fname, char Vx, int constant);

class Chip8
{
	
public:
	Chip8();
	~Chip8();

	void uninplemented(unsigned short opcode);
	
	void Initialize();
	void InitializeFunctionPointers();
	void EmulateCycle();
	void Run();
	bool LoadRom(std::string filename);
	unsigned short FetchInstruction();


	void noop(unsigned short opcode);
	void cls(unsigned short opcode);
	void rts(unsigned short opcode);
	void jmp(unsigned short opcode);
	void jsr(unsigned short opcode);
	void skeqc(unsigned short opcode);
	void sknec(unsigned short opcode);
	void skeqr(unsigned short opcode);
	void movc(unsigned short opcode);
	void addc(unsigned short opcode);
	void movr(unsigned short opcode);
	void _or(unsigned short opcode);
	void _and(unsigned short opcode);
	void _xor(unsigned short opcode);

	void addr (unsigned short opcode);
	void sub (unsigned short opcode);
	void shr(unsigned short opcode);
	void rsb(unsigned short opcode);
	void shl(unsigned short opcode);
	void skner(unsigned short opcode);
	void mvi(unsigned short opcode);
	void jmi(unsigned short opcode);
	

	void randc(unsigned short opcode);
	void sprite(unsigned short opcode);
	void skpr(unsigned short opcode);

	void skup(unsigned short opcode);
	void gdelay(unsigned short opcode);
	void keyr(unsigned short opcode);
	void sdelay(unsigned short opcode);

	void ssound(unsigned short opcode);
	void adi(unsigned short opcode);
	void font(unsigned short opcode);
	void xfont(unsigned short opcode);
	void bcd(unsigned short opcode);
	void str(unsigned short opcode);
	void ldr(unsigned short opcode);
	
	Screen screen;
	Keypad keypad;
	//main memory
	unsigned char memory[4096];
	unsigned short stack[16];
	//registers
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned short sp;

	bool drawFlag;

	//timers
	unsigned char delay_timer;
	unsigned char sound_timer;

	
	const unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	//function pointers for the instruction decoding using arrays
	std::vector<std::function<void(unsigned short)>> EmulatorFunctions;
	std::vector<std::function<void(unsigned short)>> Arithmetic;
	std::vector<std::function<void(unsigned short)>> ExtraFunctions;

	void graphics(unsigned short opcode);
	void arithmetic(unsigned short opcode);
	void keyfunction(unsigned short opcode);
	void extra(unsigned short opcode);
};

