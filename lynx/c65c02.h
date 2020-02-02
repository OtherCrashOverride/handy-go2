//
// Copyright (c) 2004 K. Wilkins
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

//////////////////////////////////////////////////////////////////////////////
//                       Handy - An Atari Lynx Emulator                     //
//                          Copyright (c) 1996,1997                         //
//                                 K. Wilkins                               //
//////////////////////////////////////////////////////////////////////////////
// 65C02 Emulation class                                                    //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This class emulates a 65C02 processor. It is interfaced to the rest of   //
// the system via the PEEK/POKE macros and a number of global variables     //
//                                                                          //
//    K. Wilkins                                                            //
// August 1997                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// Revision History:                                                        //
// -----------------                                                        //
//                                                                          //
// 01Aug1997 KW Document header added & class documented.                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#ifndef C65C02_H
#define C65C02_H

#include <stdint.h>
#include <cstdio>

#include "machine.h"

//#include <crtdbg.h>
//#define	TRACE_CPU

#ifdef TRACE_CPU

#define TRACE_CPU0(msg)					_RPT1(_CRT_WARN,"C65C02::"msg" (Time=%012d)\n",gSystemCycleCount)
#define TRACE_CPU1(msg,arg1)			_RPT2(_CRT_WARN,"C65C02::"msg" (Time=%012d)\n",arg1,gSystemCycleCount)
#define TRACE_CPU2(msg,arg1,arg2)		_RPT3(_CRT_WARN,"C65C02::"msg" (Time=%012d)\n",arg1,arg2,gSystemCycleCount)
#define TRACE_CPU3(msg,arg1,arg2,arg3)	_RPT4(_CRT_WARN,"C65C02::"msg" (Time=%012d)\n",arg1,arg2,arg3,gSystemCycleCount)

#else

#define TRACE_CPU0(msg)
#define TRACE_CPU1(msg,arg1)
#define TRACE_CPU2(msg,arg1,arg2)
#define TRACE_CPU3(msg,arg1,arg2,arg3)

#endif

//
// Handy definitions
//

#define NMI_VECTOR	0xfffa
#define BOOT_VECTOR	0xfffc
#define IRQ_VECTOR	0xfffe

#define MAX_CPU_BREAKPOINTS	8

//
// ACCESS MACROS
//

//#define CPU_PEEK(m)				(mSystem.Peek_CPU(m))
//#define CPU_PEEKW(m)			(mSystem.PeekW_CPU(m))
//#define CPU_POKE(m1,m2)			(mSystem.Poke_CPU(m1,m2))

#define CPU_PEEK(m)				(((m<0xfc00)?mRamPointer[m]:CpuPeek(m)))
#define CPU_PEEKW(m)			(((m<0xfc00)?(mRamPointer[m]+(mRamPointer[m+1]<<8)) : CpuPeek(m) | (CpuPeek(m + 1) << 8)))
#define CPU_POKE(m1,m2)			{if(m1<0xfc00) mRamPointer[m1]=m2; else CpuPoke(m1,m2);}


enum
{
   illegal=0,
   accu,
   imm,
   absl,
   zp,
   zpx,
   zpy,
   absx,
   absy,
   iabsx,
   impl,
   rel,
   zrel,
   indx,
   indy,
   iabs,
   ind
};

typedef struct
{
   int PS;		// Processor status register   8 bits
   int A;		// Accumulator                 8 bits
   int X;		// X index register            8 bits
   int Y;		// Y index register            8 bits
   int SP;		// Stack Pointer               8 bits
   int Opcode;	// Instruction opcode          8 bits
   int Operand;// Intructions operand		  16 bits
   int PC;		// Program Counter            16 bits
   bool NMI;
   bool IRQ;
   bool WAIT;
#ifdef _LYNXDBG
   int	cpuBreakpoints[MAX_CPU_BREAKPOINTS];
#endif
}C6502_REGS;

//
// The CPU emulation macros
//
#include "c6502mak.h"
//
// The CPU emulation macros
//

class CSystem;

typedef struct lssfile LSS_FILE;


class C65C02
{
   public:
      C65C02(CSystem& parent);
      ~C65C02();

   public:
      void Reset(void);

      bool ContextSave(FILE *fp);

      bool ContextLoad(LSS_FILE *fp);

      void Update(void);

      //		inline void SetBreakpoint(ULONG breakpoint) {mPcBreakpoint=breakpoint;};

      void SetRegs(C6502_REGS &regs);

      void GetRegs(C6502_REGS &regs);

      int GetPC(void);

      void xILLEGAL(void);

      void CMemMapPoke(ULONG addr, UBYTE data);
      UBYTE CMemMapPeek(ULONG addr);
      void CpuPoke(ULONG addr, UBYTE data);
      UBYTE CpuPeek(ULONG addr);

      
   private:
      CSystem	&mSystem;

      // CPU Flags & status

      int mA;		// Accumulator                 8 bits
      int mX;		// X index register            8 bits
      int mY;		// Y index register            8 bits
      int mSP;		// Stack Pointer               8 bits
      int mOpcode;  // Instruction opcode          8 bits
      int mOperand; // Intructions operand		  16 bits
      int mPC;		// Program Counter            16 bits

      int mN;		// N flag for processor status register
      int mV;		// V flag for processor status register
      int mB;		// B flag for processor status register
      int mD;		// D flag for processor status register
      int mI;		// I flag for processor status register
      int mZ;		// Z flag for processor status register
      int mC;		// C flag for processor status register

      int mIRQActive;

#ifdef _LYNXDBG
      int mPcBreakpoints[MAX_CPU_BREAKPOINTS];
      int mDbgFlag;
#endif
      uint8_t *mRamPointer;

      // Associated lookup tables

      int mBCDTable[2][256];

      //
      // Opcode prototypes
      //

   private:

        int				mMikieEnabled;
        int				mSusieEnabled;
        int				mRomEnabled;
        int				mVectorsEnabled;

        // Answers value of the Processor Status register
        int PS() const;

        // Change the processor flags to correspond to the given value
        void PS(int ps);



};


#endif
