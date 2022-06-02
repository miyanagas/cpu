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
	case LD:
		return load(cpub);
	case ST:
		return store(cpub);
	case AND:
		return and(cpub);
	case OR:
		return or (cpub);
	case EOR:
		return exclusive_or(cpub);
	case ADD:
		return add(cpub);
	case SUB:
		return subtract(cpub);
	case CMP:
		return compare(cpub);
	case ShiftRotate:
		if (((cpub->ir >> 2) & 0x1) == 0)
		{
			return shift(cpub);
		}
		else
		{
			fprintf(stderr, "Unexecutable Instruction.\n");
			return RUN_HALT;
		}
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
int exclusive_or(Cpub *cpub)
{
	Uword result, A, B;

	B = get_B(cpub);

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A ^ B;
		cpub->acc = result;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A ^ B;
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

	B = get_B(cpub);

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
	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);

	return RUN_STEP;
}

/*=============================================================================
 *   Subtract Instruction
 *===========================================================================*/
int subtract(Cpub *cpub)
{
	unsigned short result;
	Uword A, B;

	B = get_B(cpub);

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
	case BNZ:
		if (!cpub->zf)
		{
			cpub->pc = cpub->mem[cpub->mar];
		}
		break;
	case BZ:
		if (cpub->zf)
		{
			cpub->pc = cpub->mem[cpub->mar];
		}
		break;
	default:
		fprintf(stderr, "Unexecutable Instruction.\n");
		return RUN_HALT;
	}
	return RUN_STEP;
}

/*=============================================================================
 *   Load Instruction
 *===========================================================================*/
int load(Cpub *cpub)
{
	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		cpub->acc = get_B(cpub);
	}
	else /* A=1 */
	{
		cpub->ix = get_B(cpub);
	}

	return RUN_STEP;
}

/*=============================================================================
 *   AND Instruction
 *===========================================================================*/
int and (Cpub * cpub)
{
	Uword result, A, B;

	B = get_B(cpub);

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A & B;
		cpub->acc = result;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A & B;
		cpub->ix = result;
	}

	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);
	cpub->vf = 0;

	return RUN_STEP;
}

/*=============================================================================
 *   OR Instruction
 *===========================================================================*/
int or (Cpub * cpub)
{
	Uword result, A, B;

	B = get_B(cpub);

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A | B;
		cpub->acc = result;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A | B;
		cpub->ix = result;
	}

	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);
	cpub->vf = 0;

	return RUN_STEP;
}

/*=============================================================================
 *   Compare Instruction
 *===========================================================================*/
int compare(Cpub *cpub)
{
	unsigned short result;
	Uword A, B;

	B = get_B(cpub);

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
		result = A - B;
	}
	else /* A=1 */
	{
		A = cpub->ix;
		result = A - B;
	}

	cpub->vf = overflow_flag(A, B, result);
	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);

	return RUN_STEP;
}

/*=============================================================================
 *   Shift Instruction
 *===========================================================================*/
int shift(Cpub *cpub)
{
	unsigned short result;
	Uword A;

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		A = cpub->acc;
	}
	else /* A=1 */
	{
		A = cpub->ix;
	}

	switch (cpub->ir & 0x03)
	{
	case 0x2: /* SRL */
		cpub->cf = A & 0x1;
		result = A >> 1;
		break;
	case 0x3: /* SLL */
		cpub->cf = A >> 7;
		result = A << 1;
		break;
	default:
		fprintf(stderr, "Unexecutable Instruction.\n");
		return RUN_HALT;
	}

	if (op_A(cpub->ir) == 0) /* A=0 */
	{
		cpub->acc = result;
	}
	else /* A=1 */
	{
		cpub->ix = result;
	}

	cpub->vf = 0;
	cpub->nf = negative_flag(result);
	cpub->zf = zero_flag(result);

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

Uword get_B(Cpub *cpub)
{
	switch (op_B(cpub->ir))
	{
	case 0x0: /* B=000 */
		return cpub->acc;
	case 0x1: /* B=001 */
		return cpub->ix;
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
		return cpub->mem[cpub->mar];
	}
}

Uword overflow_flag(Sword A, Sword B, Sword result)
{
	if (((A >> 7) == (B >> 7)) && ((A >> 7) != (result >> 7)))
	{
		return (0x1);
	}
	else
	{
		return ((unsigned short)result >> 8);
	}
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