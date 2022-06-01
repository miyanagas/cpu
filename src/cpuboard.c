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
	case Branch:
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
	Uword result, A, B;

	switch (op_B(cpub->ir))
	{
	case 0x0: /* B=000 */
		B = cpub->acc;
		break;
	case 0x1: /* B=001 */
		B = cpub->ix;
		break;
	default:
		cpub->mar = cpub->pc;
		cpub->pc++;
		switch (op_B(cpub->ir))
		{
		case 0x4: /* B=100 */
			cpub->mar = cpub->mem[cpub->mar];
			break;
		case 0x5: /* B=101 */
			cpub->mar = cpub->mem[cpub->mar] | 0x100;
			break;
		case 0x6: /* B=110 */
			cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
			break;
		case 0x7: /* B=111 */
			cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
			break;
		default: /* B=01- */
			break;
		}
		B = cpub->mem[cpub->mar];
		break;
	}

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A - B;
		cpub->acc = result;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A - B;
		cpub->ix = result;
	}

	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);
	cpub->vf = 0;

	return RUN_STEP;
}

/*=============================================================================
 *   Add Instruction
 *===========================================================================*/
int add(Cpub *cpub)
{
	unsigned short result;
	Uword A, B;

	switch (op_B(cpub->ir))
	{
	case 0x0: /* B=000 */
		B = cpub->acc;
		break;
	case 0x1: /* B=001 */
		B = cpub->ix;
		break;
	default:
		cpub->mar = cpub->pc;
		cpub->pc++;
		switch (op_B(cpub->ir))
		{
		case 0x4: /* B=100 */
			cpub->mar = cpub->mem[cpub->mar];
			break;
		case 0x5: /* B=101 */
			cpub->mar = cpub->mem[cpub->mar] | 0x100;
			break;
		case 0x6: /* B=110 */
			cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
			break;
		case 0x7: /* B=111 */
			cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
			break;
		default: /* B=01- */
			break;
		}
		B = cpub->mem[cpub->mar];
		break;
	}

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A + B;
		cpub->acc = result;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A + B;
		cpub->ix = result;
	}

	cpub->vf = overflow_flag(A, B, result);
	cpub->cf = carry_flag(result);
	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);

	return RUN_STEP;
}

/*=============================================================================
 *   Subtract Instruction
 *===========================================================================*/
int sub(Cpub *cpub)
{
	unsigned short result;
	Uword A, B;

	switch (op_B(cpub->ir))
	{
	case 0x0: /* B=000 */
		B = cpub->acc;
		break;
	case 0x1: /* B=001 */
		B = cpub->ix;
		break;
	case 0x4: /* B=100 */
		cpub->mar = cpub->pc;
		cpub->pc++;
		cpub->mar = cpub->mem[cpub->mar];
		B = cpub->mem[cpub->mar];
		break;
	case 0x5: /* B=101 */
		cpub->mar = cpub->pc;
		cpub->pc++;
		cpub->mar = cpub->mem[cpub->mar] | 0x100;
		B = cpub->mem[cpub->mar];
		break;
	case 0x6: /* B=110 */
		cpub->mar = cpub->pc;
		cpub->pc++;
		cpub->mar = cpub->mem[cpub->mar] + cpub->ix;
		B = cpub->mem[cpub->mar];
		break;
	case 0x7: /* B=111 */
		cpub->mar = cpub->pc;
		cpub->pc++;
		cpub->mar = (cpub->mem[cpub->mar] + cpub->ix) | 0x100;
		B = cpub->mem[cpub->mar];
		break;
	default: /* B=01- */
		cpub->mar = cpub->pc;
		cpub->pc++;
		B = cpub->mem[cpub->mar];
		break;
	}

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A - B;
		cpub->acc = result;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A - B;
		cpub->ix = result;
	}

	cpub->vf = overflow_flag(A, B, result);
	cpub->cf = carry_flag(result);
	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);

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

Uword overflow_flag(Sword A, Sword B, Sword result)
{
	if (((A * B) > 0) && ((A * result) < 0))
	{
		return (0x1);
	}
	else
	{
		return (0x0);
	}
}

Uword carry_flag(unsigned short result)
{
	return (result >> 8);
}

Uword negative_flag(Uword result)
{
	return (result >> 7);
}

Uword zero_flag(Uword result)
{
	if (result == 0x00)
	{
		return 0x1;
	}
	else
	{
		return 0x0;
	}
}