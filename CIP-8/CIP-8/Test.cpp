#define CLOVE_SUITE_NAME Test
#include "Include/clove-unit.h"
#include <memory>
#include "CIP-8/Emulator.h"

chipotto::Emulator* emulator = nullptr;

CLOVE_SUITE_SETUP_ONCE()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		{
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		}
	}
}
CLOVE_SUITE_SETUP() {
	emulator = new chipotto::Emulator();
}
CLOVE_SUITE_TEARDOWN_ONCE() 
{
	SDL_Quit();
}
CLOVE_SUITE_TEARDOWN() 
{
	delete emulator;
}
CLOVE_TEST(InitTest)
{
	CLOVE_NOT_NULL(emulator);
	CLOVE_IS_TRUE(emulator->IsValid());
}
//Opcode 1
CLOVE_TEST(JPaddr)
{
   CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode1(0x1200));
   CLOVE_INT_EQ((int)(0x200 - 2), emulator->GetPCAddress());
}
//OPcode 2
CLOVE_TEST(CALLSOF)
{
	for (size_t i = 0; i < 16; i++)
	{
		emulator->Opcode2(0x2200);
	}
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)emulator->Opcode2(0x2200));
}
CLOVE_TEST(CALLIPC) 
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::NotIncrementPC,(int)emulator->Opcode2(0x2201));
	CLOVE_INT_EQ((int)0x201,(int)emulator->GetPCAddress());
	CLOVE_INT_EQ((int)0x200,(int)emulator->GetStackAddress());
}
//Opcode 3
CLOVE_TEST(SEV)
{
	emulator->Opcode6(0x6211);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode3(0x3211));
	CLOVE_INT_EQ((int)(0x202),(int)emulator->GetPCAddress());
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode3(0x3212));
	CLOVE_INT_NE((int)(0x204), (int)emulator->GetPCAddress());
}
//Opcode 4
CLOVE_TEST(SNEV) {
	emulator->Opcode6(0x6211);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode4(0x4200));
	CLOVE_INT_EQ((int)(0x202), (int)emulator->GetPCAddress());
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode4(0x4211));
	CLOVE_INT_NE((int)(0x204), (int)emulator->GetPCAddress());
}
//Opcode 6
CLOVE_TEST(LDVxbyte)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC,(int)emulator->Opcode6(0x6212));
	CLOVE_INT_EQ((int)0x12,(int)emulator->GetRegisters(0x2));
}
//Opcode 7
CLOVE_TEST(ADDVxbyte)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode7(0x7212));
	CLOVE_INT_EQ((int)0x12,(int)emulator->GetRegisters(0x2));
}
//Opcode 8
CLOVE_TEST(LDVxVy)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC,(int)emulator->Opcode8(0x8210));
	uint8_t registerX_index = 0x0;
	uint8_t registerY_index = 0x0;
	CLOVE_INT_EQ((int)emulator->GetRegisters(registerX_index),(int)emulator->GetRegisters(registerY_index));
}
CLOVE_TEST(ANDVxVy)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8212));
	uint8_t registerX_index = 0x2;
	uint8_t registerY_index = 0x1;
	CLOVE_INT_EQ((int)0x0,registerX_index & registerY_index);
}
CLOVE_TEST(XORVxVy)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8213));
	uint8_t registerX_index = 0x2;
	uint8_t registerY_index = 0x1;
	CLOVE_INT_EQ((int)0x3, registerX_index ^= registerY_index);
}
CLOVE_TEST(ADDVxVy)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8214));
	uint8_t registerX_index = 0x2;
	uint8_t registerY_index = 0x1;
	CLOVE_INT_EQ((int)0x3, registerX_index += registerY_index);
}
CLOVE_TEST(SHLxVy)
{
	emulator->Opcode6(0x61A5);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x810E));
	CLOVE_INT_EQ((int)emulator->GetRegisters(0xF), 1);
	CLOVE_INT_EQ((int)emulator->GetRegisters(0x1),(int)0x4A);

}
//OPcode A
CLOVE_TEST(LDIaddr)
{
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeA(0xA000));
	CLOVE_INT_EQ(emulator->GetI(),(int)0x0)

}
//Opcode C
CLOVE_TEST(RNDVxbyte)
{
	std::srand(0);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeC(0xC156));
	CLOVE_INT_EQ(emulator->GetRegisters(0x1),6);
}

//OPcode F
CLOVE_TEST(LDIV)
{
	emulator->Opcode6(0x6032);
	emulator->Opcode6(0x6123);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC,(int)emulator->OpcodeF(0xF155));
	CLOVE_INT_EQ((int)emulator->GetMemoryMapping((emulator->GetI())),(int)emulator->GetRegisters(0x0));
	CLOVE_INT_EQ((int)emulator->GetMemoryMapping((emulator->GetI()+ 1)), (int)emulator->GetRegisters(0x1));
}
CLOVE_TEST(LDVI)
{
	emulator->Opcode6(0x6032);
	emulator->Opcode6(0x6123);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC,(int)emulator->OpcodeF(0xF155));
	CLOVE_INT_EQ((int)emulator->GetRegisters(0x0),(int)emulator->GetMemoryMapping((emulator->GetI())));
	CLOVE_INT_EQ((int)emulator->GetRegisters(0x1), (int)emulator->GetMemoryMapping((emulator->GetI() + 1)));
}
CLOVE_TEST(LDBV)
{
	emulator->Opcode6(0x6025);
	emulator->Opcode6(0x6135);
	emulator->Opcode6(0x6245);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF133));
	CLOVE_INT_EQ((int)emulator->GetMemoryMapping(emulator->GetI()),(int)(emulator->GetRegisters(0x1) / 100));
	CLOVE_INT_EQ((int)emulator->GetMemoryMapping(emulator->GetI() + 1), (int)emulator->GetRegisters(0x1) - (emulator->GetMemoryMapping(emulator->GetI() * (100 / 10))));
	CLOVE_INT_EQ((int)emulator->GetMemoryMapping(emulator->GetI() + 2), (int)emulator->GetRegisters(0x1) % 10);
}
CLOVE_TEST(LDFV)
{
	emulator->Opcode6(0x6025);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF129));
	CLOVE_INT_EQ((int)emulator->GetI(), 5 * (int)emulator->GetRegisters(0x1));
}
CLOVE_TEST(LDVK)
{
	emulator->Opcode6(0x6025);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::WaitForKeyboard, (int)emulator->OpcodeF(0xF10A));
	CLOVE_INT_EQ((int)emulator->GetWaitForKeyboard(),(int)0x0);
	CLOVE_IS_TRUE(emulator->GetSuspended());
}
CLOVE_TEST(ADDIV)
{
	emulator->Opcode6(0x6025);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF11E));
	CLOVE_INT_EQ((int)emulator->GetI(),(int)emulator->GetRegisters(0x1));
}
CLOVE_TEST(LDSTV)
{
	emulator->Opcode6(0x6025);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF118));
}
CLOVE_TEST(LDDTV)
{
	emulator->Opcode6(0x6125);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF115));
	CLOVE_INT_EQ((int)emulator->GetDelayTimer(), (int)emulator->GetRegisters(0x1));
	CLOVE_INT_EQ((int)emulator->GetDeltaTick(),17 + SDL_GetTicks64());

}
CLOVE_TEST(LDVDT)
{
	emulator->Opcode6(0x6156);
  CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC,(int)emulator->OpcodeF(0xF107));
  CLOVE_INT_EQ((int)emulator->GetRegisters(0x1),(int)emulator->GetDelayTimer());
}