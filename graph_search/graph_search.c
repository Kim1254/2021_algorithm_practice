#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#pragma warning(disable:4996)

//#define _CONSTANT_MATRIX

#define new(Ty, len) (Ty*)malloc(sizeof(Ty) * (len))

typedef struct link_s
{
	int value;
	struct link_s* next;
} link_t;

typedef struct ptrlink_s
{
	void* value;
	struct link_s* next;
} ptrlink_t;

typedef struct point_s
{
	int index;
	int weight;
} point_t;

// TODO: Split string with space and parse them into integer
void Split(const char* str, int value[], int len)
{
	const char* p, * q;

	char* substr = NULL;
	int sublen = 0; // substring length

	p = str;

	for (int i = 0; i < len; i++)
	{
		if (*p == ' ')
		{
			p++;
			i--;
			continue;
		}

		if (!*p)
			break;

		q = strstr(p, " ");

		if (!q) // end of string
		{
			value[i] = atoi(p);
			break;
		}
		else
		{
			sublen = q - p;
			if (sublen > 127)
				sublen = 127;

			substr = new(char, sublen + 1);
			strncpy(substr, p, sublen);
			value[i] = atoi(substr);
			free(substr);
		}

		p = q + 1;
	}
}

void printMatrix(int mat[], int column, int row)
{
	for (int i = 0; i < column; i++)
	{
		for (int j = 0; j < row; j++)
		{
			if (j != 0)
				printf(" ");

			printf("%d", mat[i * row + j]);

			if (j == row - 1)
				printf("\n");
		}
	}
}

void printPointMatrix(point_t mat[], int column, int row)
{
	for (int i = 0; i < column; i++)
	{
		for (int j = 0; j < row; j++)
		{
			if (j != 0)
				printf(" ");

			printf("%d(%d)", mat[i * row + j].weight, mat[i * row + j].index);

			if (j == row - 1)
				printf("\n");
		}
	}
}

link_t* putlink(link_t* thisLink, int value)
{
	//printf("%s: %p %d\n", __FUNCTION__, thisLink, value);

	link_t* node = new(link_t, 1);
	node->value = value;
	node->next = thisLink;

	return node;
}

link_t* pushlink(link_t* thisLink, int value)
{
	//printf("%s: %p %d\n", __FUNCTION__, thisLink, value);

	link_t* node = new(link_t, 1);
	node->value = value;
	node->next = NULL;

	if (!thisLink)
		return node;

	link_t* begin = thisLink;
	while (thisLink->next)
		thisLink = thisLink->next;

	thisLink->next = node;
	return begin;
}

link_t* dumplink(link_t* thisLink)
{
	link_t* begin = NULL;

	link_t* prev = NULL;
	link_t* current = NULL;

	while (thisLink)
	{
		current = new(link_t, 1);
		current->value = thisLink->value;
		current->next = NULL;

		if (!begin)
			begin = current;

		if (prev)
			prev->next = current;

		prev = current;
		thisLink = thisLink->next;
	}

	return begin;
}

ptrlink_t* pushlink_ptr(ptrlink_t* thisLink, link_t* value)
{
	//printf("%s: %p %d\n", __FUNCTION__, thisLink, value);

	ptrlink_t* node = new(ptrlink_t, 1);
	node->value = value;
	node->next = NULL;

	if (!thisLink)
		return node;

	ptrlink_t* begin = thisLink;

	while (thisLink->next)
		thisLink = thisLink->next;
	thisLink->next = node;

	return begin;
}

void printlink(link_t* thisLink)
{
	while (thisLink)
	{
		printf("%d ", thisLink->value);
		thisLink = thisLink->next;
	}
	printf("\b");
}

int getsum_link(link_t* thisLink)
{
	int sum = 0;
	while (thisLink)
	{
		sum += thisLink->value;
		thisLink = thisLink->next;
	}
	return sum;
}

int countlink(link_t* thisLink)
{
	int num = 0;
	while (thisLink)
	{
		num++;
		thisLink = thisLink->next;
	}
	return num;
}

void freelink(link_t* thisLink)
{
	link_t* node = NULL;

	while (thisLink)
	{
		node = thisLink;
		thisLink = node->next;

		free(node);
	}
}

void freelink_ptr(ptrlink_t* thisLink)
{
	ptrlink_t* node = NULL;

	while (thisLink)
	{
		node = thisLink;
		thisLink = node->next;

		freelink((link_t*)node->value);
		free(node);
	}
}

int main()
{
	int col, row;
#ifdef _CONSTANT_MATRIX
	col = 5;
	row = 6;
	/*col = 7;
	row = 7;*/
#else
	scanf("%d %d", &col, &row);

	if (col <= 0 || row <= 0)
		exit(0);

	if (col > 100 || row > 10)
		exit(0);
#endif

	const int len = col * row;

#ifdef _CONSTANT_MATRIX
	int matrix[30] =
	{
		3, 4, 1, 2, 8, 6,
		6, 1, 8, 2, 7, 4,
		5, 9, 3, 9, 9, 5,
		8, 4, 1, 3, 2, 6,
		3, 7, 2, 8, 6, 4
	};
	/*int matrix[49] =
	{
		1, 2, 3, 4, 5, 6, 7,
		2, 3, 4, 3, 3, 3, 4,
		3, 4, 5, 6, 7, 2, 4,
		4, 5, 6, 7, 1, 3, 4,
		5, 6, 7, 1, 2, 3, 3,
		6, 7, 1, 2, 3, 6, 4,
		7, 4, 2, 3, 4, 5, 1
	};*/

	printMatrix(matrix, col, row);
#else
	int* matrix = new(int, len);
	memset(matrix, 0, sizeof(int) * len);

	if (!matrix)
		exit(0);

	char input[512];
	for (int i = 0; i < col; i++)
	{
		while (getchar() != '\n'); // buffer clear

		scanf("%[^\n]", input);
		Split(input, &matrix[i * row], row);
	}
#endif

	float f = 0;
	for (int i = 0; i < len; i++)
		f += (float)matrix[i];

	f /= len;
	f = ceil(f) + 1;

	point_t* pointMatrix = new(point_t, len); // matrix copy
	for (int i = 0; i < len; i++)
	{
		pointMatrix[i].index = i;
		pointMatrix[i].weight = matrix[i];
	}

	int min;
	point_t tmp;

	for (int i = 0; i < row; i++) // Selection sort - sort each row by their weight
	{
		for (int j = 0; j < col; j++)
		{
			min = j;
			for (int n = j + 1; n < col; n++)
			{
				if (pointMatrix[i + row * min].weight > pointMatrix[i + row * n].weight)
					min = n;
			}

			if (min != j)
			{
				tmp = pointMatrix[i + row * j];
				pointMatrix[i + row * j] = pointMatrix[i + row * min];
				pointMatrix[i + row * min] = tmp;
			}
		}
	}

	printf("\n");

	ptrlink_t* linkKeeper = NULL;
	link_t* link;

	int* selectList = new(int, row);
	memset(selectList, 0, sizeof(int) * row);

	link = NULL;
	int dist = 0;

	link_t* tmplink = NULL;

	int ttake = 0;
	for (int i = 0; i < row; i++)
	{
		if (selectList[i] && pointMatrix[i + row * selectList[i]].weight > pointMatrix[i + row * (selectList[i] - 1)].weight)
		{
			if (!i)
			{
				for (i = 1; i < row; i++)
					if (!selectList[i])
						break;

				i--;

				freelink(link);
				link = NULL;
			}
			else
			{
				tmplink = link;

				if (tmplink->next)
				{
					while (tmplink->next->next)
						tmplink = tmplink->next;

					free(tmplink->next);
					tmplink->next = NULL;
				}
				else
				{
					free(link);
					link = NULL;
				}

				selectList[i - 1] += 1;
				i -= 2;
			}
			continue;
		}

		link = pushlink(link, pointMatrix[i + row * selectList[i]].index);

		if (link->next) // length > 1
		{
			dist = pointMatrix[i + row * selectList[i]].index - pointMatrix[i - 1 + row * selectList[i - 1]].index;

			if (dist != 1 && dist != 1 - row && dist != row + 1)
			{
				tmplink = link;

				if (tmplink->next)
				{
					while (tmplink->next->next)
						tmplink = tmplink->next;

					free(tmplink->next);
					tmplink->next = NULL;
				}
				else
				{
					free(link);
					link = NULL;
				}

				linkKeeper = pushlink_ptr(linkKeeper, dumplink(link));

				if (pointMatrix[i + row * (selectList[i] + 1)].weight == pointMatrix[i + row * selectList[i]].weight)
				{
					selectList[i]++;
					i--;
				}
				else
				{
					tmplink = link;

					if (tmplink->next)
					{
						while (tmplink->next->next)
							tmplink = tmplink->next;

						free(tmplink->next);
						tmplink->next = NULL;
					}
					else
					{
						free(link);
						link = NULL;
					}

					selectList[i - 1] += 1;
					i -= 2;
				}

				continue;
			}
			else if (i == row - 1)
			{
				linkKeeper = pushlink_ptr(linkKeeper, dumplink(link));
			}
		}
	}

	free(selectList);

	freelink(link);
	link = NULL;

	ptrlink_t* tmpPtrlink = linkKeeper;
	while (tmpPtrlink)
	{
		if (countlink((link_t*)tmpPtrlink->value) > countlink(link))
			link = tmpPtrlink->value;

		tmpPtrlink = tmpPtrlink->next;
	}

	int count;
	int begin = 0;
	int cand[3] = { 0, 0, 0 };

	count = link->value % row;
	if (count)
	{
		begin = link->value;
		for (int i = 0; i < count; i++)
		{
			min = 0;
			cand[0] = begin - 1; // west
			cand[1] = begin - row - 1; // northwest;
			cand[2] = begin + row - 1; // southwest;

			for (int j = 0; j < 3; j++)
			{
				if (cand[j] < 0)
					cand[j] += len;
				if (cand[j] > len - 1)
					cand[j] -= len;

				if (min != j)
				{
					if (matrix[cand[min]] > matrix[cand[j]])
						min = j;
				}
			}

			link = putlink(link, cand[min]);
			begin = cand[min];
		}
	}

	tmplink = link;
	while (tmplink->next)
		tmplink = tmplink->next;

	count = row - 1 - tmplink->value % row;
	if (count)
	{
		begin = tmplink->value;
		for (int i = 0; i < count; i++)
		{
			min = 0;
			cand[0] = begin + 1; // east
			cand[1] = begin - row + 1; // northeast;
			cand[2] = begin + row + 1; // southeast;

			for (int j = 0; j < 3; j++)
			{
				if (cand[j] < 0)
					cand[j] += len;
				if (cand[j] > len - 1)
					cand[j] -= len;

				if (min != j)
				{
					if (matrix[cand[min]] > matrix[cand[j]])
						min = j;
				}
			}

			link = pushlink(link, cand[min]);
			begin = cand[min];
		}
	}

	tmplink = link;
	link = NULL;

	while (tmplink)
	{
		link = pushlink(link, matrix[tmplink->value]);
		tmplink = tmplink->next;
	}

	printlink(link);
	printf("\n");

	printf("%d\n", getsum_link(link));

	freelink(link);
	freelink_ptr(linkKeeper);
	free(pointMatrix);
#ifndef _CONSTANT_MATRIX
	free(matrix);
#endif

	return 1;
}
