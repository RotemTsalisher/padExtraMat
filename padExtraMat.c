#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"

#define MAX_MAT_SIZE 1000

typedef struct {
	int A[MAX_MAT_SIZE];
	int n, m;
}s_mat;

void initTestMat(s_mat* testMat);
void printMat(s_mat* mat);
void initMask(s_mat* mask, int k);
s_mat* padMat(s_mat* mat, int xpad, int ypad);
int wSum(int* m1, int* m2, int n_m1, int m_m1, int n_m2, int m_m2);
s_mat* corr2D(s_mat* m1, s_mat* m2);
void occupiedSpots(s_mat* corr, s_mat* orig);
int findMax(s_mat* m);

void main()
{
	s_mat testMat,mask,*corr;
	initTestMat(&testMat);
	initMask(&mask, 2);

	printf("test mat:\n");
	printMat(&testMat);
	printf("mask: \n");
	printMat(&mask);

	corr = corr2D(&testMat, &mask);
	printf("corr\n");
	printMat(corr);
	occupiedSpots(corr, &testMat);
	printf("after occupoed spots:\n");
	printMat(corr);
	int max = findMax(corr);
	printf("max value in corr = %d\n", max);
}
void initTestMat(s_mat* testMat)
{
	testMat->n = 3;
	testMat->m = 4;

	*(testMat->A)         = 1;
	*(testMat->A +  1)    = 0;
	*(testMat->A +  2)    = 0;
	*(testMat->A +  3)    = 0;
	*(testMat->A +  4)    = 0;
	*(testMat->A +  5)    = 1;
	*(testMat->A +  6)    = 1;
	*(testMat->A +  7)    = 0;
	*(testMat->A +  8)    = 0;
	*(testMat->A +  9)    = 0;
	*(testMat->A + 10)    = 0;
	*(testMat->A + 11)    = 1;
}
void printMat(s_mat* mat)
{
	int i, j;

	printf("PRINTING %dx%d MAT:\n", mat->n, mat->m);
	for (i = 0; i < mat->n; i++)
	{
		for (j = 0; j < mat->m; j++)
		{
			printf("| %d |", *(mat->A + mat->m * i + j));
		}
		printf("\n");
	}
}
void initMask(s_mat* mask, int k)
{
	int i, j, offset;
	offset = k;
	mask->n = (k << 1) + 1;
	mask->m = (k << 1) + 1;

	for (i = 0; i < mask->n; i++)
	{
		for (j = 0; j < mask->m; j++)
		{
			if ( (((i - offset) < 0) && ((j - offset) < 0) && ((i + j - (offset << 1)) >= -k)) ^
				((i - offset < 0) && (j - i <= k)) ^
				((j - offset < 0) && (i - j <= k)) ^
				((i-offset >=0) && (j-offset >=0) && (i + j -(offset<<1) <=k)) )
			{
				*(mask->A + mask->m * i + j) = 1;
			}
			else
			{
				*(mask->A + mask->m * i + j) = 0;
			}
		}
	}
}
s_mat* padMat(s_mat* mat, int xpad, int ypad)
{
	s_mat paddedMat;
	int i, j;

	paddedMat.n = mat->n + (ypad << 1);
	paddedMat.m = mat->m + (xpad << 1);

	for (i = 0; i < paddedMat.n; i++)
	{
		for (j = 0; j < paddedMat.m; j++)
		{
			if ((i < ypad) || (j < xpad) || (i > (mat->n - 1) + ypad) || (j > (mat->m - 1) + xpad))
			{
				*(paddedMat.A + paddedMat.m * i + j) = 0;
			}
			else
			{
				*(paddedMat.A + paddedMat.m * i + j) = *(mat->A + mat->m * (i - ypad) + (j - xpad));
			}
		}
	}

	return &paddedMat;
}
int wSum(int* m1, int* m2, int n_m1, int m_m1, int n_m2, int m_m2)
{
	int i, j, xdim, ydim, sum;

	xdim = (m_m1 <= m_m2) ? m_m1 : m_m2;
	ydim = (n_m1 <= n_m2) ? n_m1 : n_m2;

	sum = 0;
	for (i = 0; i < ydim; i++)
	{
		for (j = 0; j < xdim; j++)
		{
			sum += (*(m1 + m_m1 * i + j)) * (*(m2 + m_m2 * i + j));
		}
	}
	return sum;
}
s_mat* corr2D(s_mat* mat, s_mat* mask)
{
	s_mat corr;
	s_mat *paddedMat;
	int i, j,xpad,ypad;

	corr.n = mat->n;
	corr.m = mat->m;
	xpad = (mask->m - 1) >> 1;
	ypad = (mask->n - 1) >> 1;
	paddedMat = padMat(mat, xpad, ypad);
	printf("padded mat:\n");
	printMat(paddedMat);
	for (i = 0; i < mat->n; i++)
	{
		for (j = 0; j < mat->m; j++)
		{
			*(corr.A + corr.m * i + j) = wSum((paddedMat->A + paddedMat->m*i + j), mask->A, paddedMat->n, paddedMat->m, mask->n, mask->m);
		}
	}
	return &corr;
}
void occupiedSpots(s_mat* corr, s_mat* orig)
{
	int i, j;

	for (i = 0; i < corr->n; i++)
	{
		for (j = 0; j < corr->m; j++)
		{
			if (*(orig->A + orig->m * i + j) & 0x01)
			{
				*(corr->A + corr->m * i + j) &= 0x00;
			}
		}
	}
}
int findMax(s_mat* m)
{
	int i, j, max;

	max = *m->A;
	for (i = 0; i < m->n; i++)
	{
		for (j = 0; j < m->m; j++)
		{
			if (*(m->A + m->m * i + j) > max)
			{
				max = *(m->A + m->m * i + j);
			}
		}
	}
	return max;
}