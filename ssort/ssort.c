#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

// Disable warning 4996
#pragma warning(disable:4996)

#define new(Ty, Length) (Ty*)malloc(sizeof(Ty) * Length)

// Random macros
// NUM: the number of data
// WEIGHT_MIN: minimun value of weight
// WEIGHT_MAX: maximun value of weight
// INTEL_MIN: minimun value of iq scale 100.0
// INTEL_MAX: maximun value of iq

#define NUM 100

#define WEIGHT_MIN 1000
#define WEIGHT_MAX 20000

#define INTEL_MIN 5000
#define INTEL_MAX 20000

// ERROR CODES
// 0x00: Failed open file "input.txt"
// 0x01: No data have been founded.
// 0x02: Failed dynamic allocation.
//

typedef struct input_s
{
	int weight;
	int intel;
	int index;
} input_t;

// Random function for exceed RAND_MAX
// This function work like this: min + rand / max * difference;
int random_int(int min, int max)
{
	const int diff = abs(max - min);
	const float a = (float)rand() / RAND_MAX;

	return min + a * diff;
}

// Method 1: Reads file stream "input.txt." for reading values, we should write data in there.
//
// Writing example:
// 6008 1300
// 6000 2100
// 500 2000
// ...
// This function avoids blank(s[0] = NULL) and semicolon(;) in first character.
//
int Read_Config(input_t** p1)
{
	FILE* fp = fopen("input.txt", "r");

	if (!fp)
	{
		printf("ERROR: %X\n", 0x00);
		return 0;
	}

	int numLine = 0;
	int len;
	char linedata[1024];

	while (fgets(linedata, 1024, fp))
	{
		if (!linedata[0] || linedata[0] == ';')
			continue;

		len = strlen(linedata);

		if (len > 2)
		{
			if (linedata[len - 2] == '\r' && linedata[len - 1] == '\n')
				linedata[len - 2] = 0;
		}

		if (len > 1)
		{
			if (linedata[len - 1] == '\n')
				linedata[len - 1] = 0;
		}

		numLine++;
	}

	if (!numLine)
	{
		printf("ERROR: %X\n", 0x01);
		return 0;
	}

	*p1 = new(input_t, numLine);

	fseek(fp, SEEK_SET, 0);

	numLine = 0;
	while (fgets(linedata, 1024, fp))
	{
		if (!linedata[0] || linedata[0] == ';')
			continue;

		len = strlen(linedata);

		if (len > 2)
		{
			if (linedata[len - 2] == '\r' && linedata[len - 1] == '\n')
				linedata[len - 2] = 0;
		}

		if (len > 1)
		{
			if (linedata[len - 1] == '\n')
				linedata[len - 1] = 0;
		}

		sscanf(linedata, "%d %d", &(*p1)[numLine].weight, &(*p1)[numLine].intel);
		(*p1)[numLine].index = numLine++;
	}

	return numLine;
}

// Method 2: Random given values
// returns @len when it sucessfully done.
int Random_Give(input_t** p1)
{
	*p1 = new(input_t, NUM);
	if (!*p1)
	{
		printf("ERROR: %X\n", 0x02);
		return 0;
	}

	for (int i = 0; i < NUM; i++)
	{
		(*p1)[i].index = i;
		(*p1)[i].weight = random_int(WEIGHT_MIN, WEIGHT_MAX);
		(*p1)[i].intel = random_int(INTEL_MIN, INTEL_MAX);
		//printf("%d: %d %d\n", ptrElephant[i].index + 1, ptrElephant[i].weight, ptrElephant[i].intel);
	}

	return NUM;
}

// selection_sort
void sort(input_t* ptr, int len)
{
	if (len <= 0)
		return NULL;

	int s;
	input_t temp;

	for (int i = 0; i < len; i++)
	{
		s = i;
		for (int j = i + 1; j < len; j++)
		{
			if (ptr[s].weight > ptr[j].weight)
				s = j;
		}

		if (s == i)
			continue;

		temp = ptr[i];
		ptr[i] = ptr[s];
		ptr[s] = temp;
	}
}

int main()
{
	srand(time(NULL));

	// Pointer to save allocated data
	input_t* ptrElephant = NULL;

	//printf("Reading files...\n");

	int n = Read_Config(&ptrElephant);
	//int n = Random_Give(&ptrElephant);

	if (!n)
		return;

	// Selection sort the elephant for weight
	sort(ptrElephant, n);

	// A value for check previous position
	int j = -1;

	// TODO: save the list of index.
	int* ptrList = new(int, n);
	int numList = 0;

	input_t* target = NULL;
	for (int i = 0; i < n; i++)
	{
		//printf("%d: %d %d\n", ptrElephant[i].index + 1, ptrElephant[i].weight, ptrElephant[i].intel);

		if (!i)
			continue;

		// Initial state(j == -1): the first(previous) element is not exists. Check the old(i - 1) elem.
		if (j == -1)
			target = ptrElephant + i - 1;
		else // Older value exists(j != -1): Check with previous read elem(j).
			target = ptrElephant + j;

		if (target->weight >= ptrElephant[i].weight || target->intel <= ptrElephant[i].intel)
			continue;

		if (j == -1) // Add both first elem before intializing.
			ptrList[numList++] = ptrElephant[i - 1].index;

		ptrList[numList++] = ptrElephant[i].index;
		j = i;
	}

	printf("%d\n", numList);

	for (int i = 0; i < numList; i++)
		printf("%d\n", ptrList[i] + 1);

	free(ptrList);
	free(ptrElephant);

	return 1;
}
