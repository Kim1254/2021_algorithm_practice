#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#pragma warning(disable:4996)

typedef struct point2D_s
{
	float x;
	float y;
} point2D_t;

typedef struct edge_s
{
	int idx[2];
	float len;
} edge_t;

typedef struct uf_s
{
	int idx;
	int parent;
} uf_t;

#define new(Ty, len) (Ty*)malloc(sizeof(Ty) * (len))
#define buffer_clear while (getchar() != '\n') {}

// Assertion?
#define exit_check(expression) if (expression) {  printf(#expression); printf("\n"); exit(0); }

// Saving Ink(Problem 9)
// 
// Input: terminal(stdin)
// Output: terminal(stdout)
//
// Used Algorithm: Kruskal Algorithm, Union-Find, Selection Sort
int main()
{
	int len;

	scanf("%d", &len);

	exit_check(len <= 0);

	point2D_t* ptrPoint = new(point2D_t, len);

	for (int i = 0; i < len; i++)
	{
		buffer_clear;
		scanf("%f %f", &ptrPoint[i].x, &ptrPoint[i].y);
	}

	// Step 1: Create edges
	// Edge generation
	int lenEdge = len * (len - 1) / 2; // num_edge(n) = sigma(n - 1)
	edge_t* ptrEdge = new(edge_t, lenEdge);
	int counter = 0;

	for (int i = 0; i < len; i++)
	{
		for (int j = i + 1; j < len; j++)
		{
			ptrEdge[counter].idx[0] = i;
			ptrEdge[counter].idx[1] = j;

			ptrEdge[counter].len = (float)sqrt(pow((double)ptrPoint[i].x - ptrPoint[j].x, 2) + pow((double)ptrPoint[i].y - ptrPoint[j].y, 2));

			counter++;
		}
	}


	// Step 2 (Kruskal Algorithm): sort edges (using Selection Sort)
	// variable 'counter' plays role of minimum selection in this seleciton sort.

	edge_t temp = { {0, 0}, 0 };
	for (int i = 0; i < lenEdge; i++)
	{
		counter = i;
		for (int j = i + 1; j < lenEdge; j++)
		{
			if (ptrEdge[counter].len > ptrEdge[j].len)
				counter = j;
		}

		temp = ptrEdge[i];
		ptrEdge[i] = ptrEdge[counter];
		ptrEdge[counter] = temp;
	}

	// make-set(len) (union-find)
	uf_t* ptrUf = new(uf_t, len);
	for (int i = 0; i < len; i++)
		ptrUf[i].idx = ptrUf[i].parent = i;

	// Identifier whether a vectice is selected.
	int* ptrCheck = new(int, len);
	memset(ptrCheck, 0, sizeof(int) * len);

	float total_length = 0;

	// Last Step (Kruskal Algorithm - Selection)
	// variable 'counter' plays role of check all vertice are selected.
	for (int i = 0; i < lenEdge; i++)
	{
		// Check it creates cycle (find(x) = find(y)) (union-find)
		if (ptrUf[ptrEdge[i].idx[0]].parent == ptrUf[ptrEdge[i].idx[1]].parent)
			continue;

		total_length += ptrEdge[i].len;
		// union(x, y) -> union(idx[0], idx[1])
		ptrUf[ptrEdge[i].idx[1]].parent = ptrUf[ptrEdge[i].idx[0]].parent;

		for (counter = 0; counter < len; counter++)
			if (!ptrCheck[counter])
				break;

		// All vertices are selected - break loop.
		if (counter == len)
			break;
	}

	// NOTE: first when I wrote this, I considered the case of all vertices are not connected.
	// However, the case cannot be occur, since I made all edges that connects all vectice from beginning.
	printf("%.2f\n", total_length);

	// Memory release
	free(ptrPoint);
	free(ptrEdge);
	free(ptrUf);
	free(ptrCheck);

	return 1;
}
