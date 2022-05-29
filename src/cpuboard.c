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

	if (cpub->ir >= 0x70 && cpub->ir <= 0x7f)
	{
		return store(cpub);
	}
	else if (cpub->ir >= 0xc0 && cpub->ir <= 0xcf)
	{
		return eor(cpub);
	}
	else if (cpub->ir >= 0xb0 && cpub->ir <= 0xbf)
	{
		return add(cpub);
	}
	else if (cpub->ir >= 0xa0 && cpub->ir <= 0xaf)
	{
		return sub(cpub);
	}
	else if (cpub->ir >= 0x30 && cpub->ir <= 0x3f)
	{
		return branch(cpub);
	}
	else if (cpub->ir >= 0x0c && cpub->ir <= 0x0f)
	{
		return RUN_HALT;
	}
	else
	{
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

	if (cpub->ir >= 0x70 && cpub->ir <= 0x77) /* A=0 */
	{
		switch (cpub->ir)
		{
		case 0x74: /* B=100 */
			cpub->mar = cpub->mem[cpub->mar];
			break;
		case 0x75: /* B=101 */
			cpub->mar = cpub->mem[cpub->mar] | 0x100;
			break;
		case 0x76: /* B=110 */
			cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
			break;
		case 0x77: /* B=111 */
			cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
			break;
		default:
			fprintf(stderr, "Unexecutable Instruction.\n");
			return RUN_HALT;
		}
		cpub->mem[cpub->mar] = cpub->acc;
	}
	else /* A=1 */
	{
		switch (cpub->ir)
		{
		case 0x7c: /* B=100 */
			cpub->mar = cpub->mem[cpub->mar];
			break;
		case 0x7d: /* B=101 */
			cpub->mar = cpub->mem[cpub->mar] | 0x100;
			break;
		case 0x7e: /* B=110 */
			cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
			break;
		case 0x7f: /* B=111 */
			cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
			break;
		default:
			fprintf(stderr, "Unexecutable Instruction.\n");
			return RUN_HALT;
		}
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