/*
 *	Project-based Learning II (CPU)
 *
 *	Program:	instruction set simulator of the Educational CPU Board
 *	File Name:	cpuboard.c
 *	Descrioption:	simulation(emulation) of an instruction
 */

#include "cpuboard.h"
#include <stdio.h>

/*=============================================================================
 *   Simulation of a Single Instruction
 *===========================================================================*/
int step(Cpub *cpub)
{
	/*
	 *   [ add codes here for the simulation of an instruction ]
	 *   [ return RUN_STEP or RUN_HALT ]
	 */
	cpub->mar = cpub->pc;
	cpub->pc++;

	cpub->ir = cpub->mem[cpub->mar];

	switch (op_code(cpub->ir))
	{
	case ST:
		return store(cpub);
	case EOR:
		return eor(cpub);
	case ADD:
		return add(cpub);
	case SUB:
		return sub(cpub);
	case B:
		return branch(cpub);
	case 0x0:
		if ((cpub->ir >> 2) == 0x3)
			return RUN_HALT;
		else if (op_A(cpub->ir) == 0x0)
			return RUN_STEP;
	default:
		fprintf(stderr, "Unexecutable Instruction.\n");
		return RUN_HALT;
	}
}

/*=============================================================================
 *   Store Instruction
 *===========================================================================*/
int store(Cpub *cpub)
{
	cpub->mar = cpub->pc;
	cpub->pc++;

	switch (op_B(cpub->ir))
	{
	case 0x04: /* B=100 */
		cpub->mar = cpub->mem[cpub->mar];
		break;
	case 0x05: /* B=101 */
		cpub->mar = cpub->mem[cpub->mar] | 0x100;
		break;
	case 0x06: /* B=110 */
		cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
		break;
	case 0x07: /* B=111 */
		cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
		break;
	default:
		fprintf(stderr, "Unexecutable Instruction.\n");
		return RUN_HALT;
	}

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		cpub->mem[cpub->mar] = cpub->acc;
	}
	else /* A=1 */
	{
		cpub->mem[cpub->mar] = cpub->ix;
	}

	return RUN_STEP;
}

/*=============================================================================
 *   Exclusive OR Instruction
 *===========================================================================*/
int eor(Cpub *cpub)
{
	if (cpub->ir >= 0xc0 && cpub->ir <= 0xc7)
	{
		// A=0
		if (cpub->ir == 0xc0)
		{
			// B=000
			cpub->acc = cpub->acc ^ cpub->acc;
		}
		else if (cpub->ir == 0xc1)
		{
			// B=001
			cpub->acc = cpub->acc ^ cpub->ix;
		}
		else
		{
			cpub->mar = cpub->pc;
			cpub->pc++;

			switch (cpub->ir)
			{
			case 0xc4:
				// B=100
				cpub->mar = cpub->mem[cpub->mar];
				break;
			case 0xc5:
				// B=101
				cpub->mar = cpub->mem[cpub->mar] | 0x100;
				break;
			case 0xc6:
				// B=110
				cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
				break;
			case 0xc7:
				// B=111
				cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
				break;
			default:
				// B=01-
				break;
			}
			cpub->acc = cpub->acc ^ cpub->mem[cpub->mar];
		}
		cpub->nf = cpub->acc >> 7;
		if (cpub->acc == 0)
		{
			cpub->zf = 1;
		}
		else
		{
			cpub->zf = 0;
		}
	}
	else
	{
		// A=1
		if (cpub->ir == 0xc8)
		{
			// B=000
			cpub->ix = cpub->ix ^ cpub->acc;
		}
		else if (cpub->ir == 0xc9)
		{
			// B=001
			cpub->ix = cpub->ix ^ cpub->ix;
		}
		else
		{
			cpub->mar = cpub->pc;
			cpub->pc++;

			switch (cpub->ir)
			{
			case 0xcc:
				// B=100
				cpub->mar = cpub->mem[cpub->mar];
				break;
			case 0xcd:
				// B=101
				cpub->mar = cpub->mem[cpub->mar] | 0x100;
				break;
			case 0xce:
				// B=110
				cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
				break;
			case 0xcf:
				// B=111
				cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
				break;
			default:
				// B=01-
				break;
			}
			cpub->ix = cpub->ix ^ cpub->mem[cpub->mar];
		}
		cpub->nf = cpub->ix >> 7;
		if (cpub->ix == 0)
		{
			cpub->zf = 1;
		}
		else
		{
			cpub->zf = 0;
		}
	}
	cpub->vf = 0;
	return RUN_STEP;
}

/*=============================================================================
 *   Add Instruction
 *===========================================================================*/
int add(Cpub *cpub)
{
	unsigned short result;
	if (cpub->ir >= 0xb0 && cpub->ir <= 0xb7)
	{
		// A=0
		if (cpub->ir == 0xb0)
		{
			// B=000
			result = cpub->acc + cpub->acc;
		}
		else if (cpub->ir == 0xb1)
		{
			// B=001
			result = cpub->acc + cpub->ix;
		}
		else
		{
			cpub->mar = cpub->pc;
			cpub->pc++;

			switch (cpub->ir)
			{
			case 0xb4:
				// B=100
				cpub->mar = cpub->mem[cpub->mar];
				break;
			case 0xb5:
				// B=101
				cpub->mar = cpub->mem[cpub->mar] | 0x100;
				break;
			case 0xb6:
				// B=110
				cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
				break;
			case 0xb7:
				// B=111
				cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
				break;
			default:
				// B=01-
				break;
			}
			result = cpub->acc + cpub->mem[cpub->mar];
		}
		cpub->acc = result;
	}
	else
	{
		// A=1
		if (cpub->ir == 0xb8)
		{
			// B=000
			result = cpub->ix + cpub->acc;
		}
		else if (cpub->ir == 0xb9)
		{
			// B=001
			result = cpub->ix + cpub->ix;
		}
		else
		{
			cpub->mar = cpub->pc;
			cpub->pc++;

			switch (cpub->ir)
			{
			case 0xbc:
				// B=100
				cpub->mar = cpub->mem[cpub->mar];
				break;
			case 0xbd:
				// B=101
				cpub->mar = cpub->mem[cpub->mar] | 0x100;
				break;
			case 0xbe:
				// B=110
				cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
				break;
			case 0xbf:
				// B=111
				cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
				break;
			default:
				// B=01-
				break;
			}
			result = cpub->ix + cpub->mem[cpub->mar];
		}
		cpub->ix = result;
	}

	if (result >> 8 == 0x00)
	{
		cpub->vf = 0;
	}
	else
	{
		cpub->vf = 1;
	}
	cpub->nf = (Uword)result >> 7;

	if ((Uword)result == 0x00)
	{
		cpub->zf = 1;
	}
	else
	{
		cpub->zf = 0;
	}

	return RUN_STEP;
}

/*=============================================================================
 *   Subtract Instruction
 *===========================================================================*/
int sub(Cpub *cpub)
{
	unsigned short result;
	if (cpub->ir >= 0xa0 && cpub->ir <= 0xa7)
	{
		// A=0
		if (cpub->ir == 0xa0)
		{
			// B=000
			result = cpub->acc - cpub->acc;
		}
		else if (cpub->ir == 0xa1)
		{
			// B=001
			result = cpub->acc - cpub->ix;
		}
		else
		{
			cpub->mar = cpub->pc;
			cpub->pc++;

			switch (cpub->ir)
			{
			case 0xa4:
				// B=100
				cpub->mar = cpub->mem[cpub->mar];
				break;
			case 0xa5:
				// B=101
				cpub->mar = cpub->mem[cpub->mar] | 0x100;
				break;
			case 0xa6:
				// B=110
				cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
				break;
			case 0xa7:
				// B=111
				cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
				break;
			default:
				// B=01-
				break;
			}
			result = cpub->acc - cpub->mem[cpub->mar];
		}
		cpub->acc = result;
	}
	else
	{
		// A=1
		if (cpub->ir == 0xa8)
		{
			// B=000
			result = cpub->ix - cpub->acc;
		}
		else if (cpub->ir == 0xa9)
		{
			// B=001
			result = cpub->ix - cpub->ix;
		}
		else
		{
			cpub->mar = cpub->pc;
			cpub->pc++;

			switch (cpub->ir)
			{
			case 0xac:
				// B=100
				cpub->mar = cpub->mem[cpub->mar];
				break;
			case 0xad:
				// B=101
				cpub->mar = cpub->mem[cpub->mar] | 0x100;
				break;
			case 0xae:
				// B=110
				cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
				break;
			case 0xaf:
				// B=111
				cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
				break;
			default:
				// B=01-
				break;
			}
			result = cpub->ix - cpub->mem[cpub->mar];
		}
		cpub->ix = result;
	}

	if (result >> 8 == 0x00)
	{
		cpub->vf = 0;
	}
	else
	{
		cpub->vf = 1;
	}
	cpub->nf = (Uword)result >> 7;
	if ((Uword)result == 0x00)
	{
		cpub->zf = 1;
	}
	else
	{
		cpub->zf = 0;
	}

	return RUN_STEP;
}

/*=============================================================================
 *   Branch Instruction
 *===========================================================================*/
int branch(Cpub *cpub)
{
	cpub->mar = cpub->pc;
	cpub->pc++;

	switch (cpub->ir)
	{
	case 0x31:
		if (!cpub->zf)
		{
			cpub->pc = cpub->mem[cpub->mar];
		}
		break;
	default:
		return RUN_HALT;
		break;
	}
	return RUN_STEP;
}

Uword op_code(Uword ir)
{
	return (ir >> 4);
}

Uword op_A(Uword ir)
{
	return ((ir >> 3) & 0x01);
}

Uword op_B(Uword ir)
{
	return (ir & 0x07);
}