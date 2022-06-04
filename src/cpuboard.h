/*
 *	Project-based Learning II (CPU)
 *
 *	Program:	instruction set simulator of the Educational CPU Board
 *	File Name:	cpuboard.h
 *	Descrioption:	resource definition of the educational computer board
 */

/*=============================================================================
 *   Architectural Data Types
 *===========================================================================*/
typedef signed char Sword;
typedef unsigned char Uword;
typedef unsigned short Addr;
typedef unsigned char Bit;

/*=============================================================================
 *   CPU Board Resources
 *===========================================================================*/
#define MEMORY_SIZE 256 * 2
#define IMEMORY_SIZE 256

typedef struct iobuf
{
	Bit flag;
	Uword buf;
} IOBuf;

typedef struct cpuboard
{
	Uword pc;
	Uword acc;
	Uword ix;
	Bit cf, vf, nf, zf;
	IOBuf *ibuf;
	IOBuf obuf;
	Addr mar;
	Uword ir;
	/*
	 *   [ add here the other CPU resources if necessary ]
	 */
	Uword mem[MEMORY_SIZE]; /* 0XX:Program, 1XX:Data */
} Cpub;

/*=============================================================================
 *   Top Function of an Instruction Simulation
 *===========================================================================*/
#define RUN_HALT 0
#define RUN_STEP 1
#define LD 0x6
#define ST 0x7

#define AND 0xe
#define OR 0xd
#define EOR 0xc
#define ADD 0xb
#define SUB 0xa
#define CMP 0xf
#define ShiftRotate 0x4
#define Branch 0x3
#define BNZ 0x31
#define BZ 0x39

int step(Cpub *);
int load(Cpub *);
int store(Cpub *);
int and (Cpub *);
int or (Cpub *);
int exclusive_or(Cpub *);
int add(Cpub *);
int subtract(Cpub *);
int compare(Cpub *);
int shift(Cpub *);
int branch(Cpub *);

Uword op_code(Uword);
Uword op_A(Uword);
Uword op_B(Uword);
Uword get_B(Cpub *);
Bit overflow_flag(Sword, Sword, Sword);
Bit negative_flag(Uword);
Bit zero_flag(Uword);
