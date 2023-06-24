#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#pragma warning(disable:4996)

#define new(Ty, len) (Ty*)malloc(sizeof(Ty) * (len))

#if defined _DEBUG
#define assertion(expression) if (!(expression)) { printf("Assertion: "); printf(#expression); printf("\n"); exit(0); }
#else
#define assertion(expression) (void*)0
#endif

#define vdist(a, b) sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y))

typedef struct vertex_s
{
	float x;
	float y;
} vertex_t;

typedef struct list_s
{
	vertex_t v;
	struct list_s* next;
} list_t;

list_t* push(list_t* list, vertex_t v)
{
	list_t* next = new(list_t, 1);

	assertion(next != NULL);

	next->v = v;
	next->next = NULL;

	if (list)
	{
		list_t* tmp = list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = next;

		return list;
	}

	return next;
}

list_t* free_list(list_t* list)
{
	list_t* tmp = NULL;
	while (list)
	{
		tmp = list;
		list = list->next;
		free(tmp);
	}

	return NULL;
}

// TODO: check that one list contains specific element. returns 1 when @list contains @v.
int containv(list_t* list, vertex_t v)
{
	while (list)
	{
		if (list->v.x == v.x && list->v.y == v.y)
			return 1;

		list = list->next;
	}
	return 0;
}

// TODO: Calculate cosine value in three point.
// In this method the angle never proceed PI / 2.
// Which means we can compare the angles in 180 degrees and over 180 as same.
float v3cos(vertex_t a, vertex_t b, vertex_t c)
{
	// ba = a - b
	a.x -= b.x;
	a.y -= b.y;

	// bc = c - b
	c.x -= b.x;
	c.y -= b.y;

	float bad = sqrt((double)a.x * a.x + (double)a.y * a.y);
	float bcd = sqrt((double)c.x * c.x + (double)c.y * c.y);

	// ab = abcos
	// cos = 
	float co = (a.x * c.x + a.y * c.y);
	if (!bad || !bcd) // abc isn't triangle :(
		return 1;

	co /= bad;
	co /= bcd;
	return co;
}

// TODO: Angle calculation between 2 vector2D.
float vec2angle(vertex_t a, vertex_t b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	float angle = 0;

	if (dx >= 0 && !dy)
		angle = 0;
	else
	{
		angle = fabs(dy) / (fabs(dx) + fabs(dy));
		if (dx < 0 && dy >= 0)
			angle = 2 - angle;
		else if (dx <= 0 && dy < 0)
			angle = 2 + angle;
		else if (dx > 0 && dy < 0)
			angle = 4 - angle;
	}

	return angle * 90;
}

list_t* jarvis(list_t* list)
{
	if (!list)
		return NULL;

	list_t* begin = list;
	for (list_t* temp = list->next; temp != NULL; temp = temp->next)
	{
		if (begin->v.y > temp->v.y)
			begin = temp;
		else if (begin->v.y == temp->v.y)
			if (begin->v.x > temp->v.x)
				begin = temp;
	}

	list_t* rs = push(NULL, begin->v);

	list_t* next = NULL;
	list_t* prevprev = begin;
	list_t* prev = begin;

	while (1)
	{
		next = list; // select a node

		float cmin = v3cos(prevprev->v, prev->v, next->v);

		if (prevprev == prev) // use circular angle before three point use (aab triangle has 0 angle :(, it has no link)
			cmin = vec2angle(prev->v, next->v) / 360;

		for (list_t* tmp = next->next; tmp != NULL; tmp = tmp->next)
		{
			if (tmp != begin && containv(rs, tmp->v)) // ignore already linked one, except the begin(cause we should break the loop)
				continue;

			float co = v3cos(prevprev->v, prev->v, tmp->v); // angle calculation
			if (prevprev == prev) // aab triangle
				co = vec2angle(prev->v, tmp->v) / 360;
			if (prev == tmp) // ignore when prev=next(same point)
				co = 1;

			if (co <= cmin)
			{
				next = tmp;
				cmin = co;
			}
		}

		if (next == begin) // end
			break;

		rs = push(rs, next->v); // add
		prevprev = prev;
		prev = next; // update previous visited node
	}

	return rs;
}

// Relink the hull(@jar) until it have @node
//
// We draw the convex hull in convex hull.
// When the convex hull contains all nodes when it have (0, 0) in it,
// it can be told that convex hull first + this convex hull = convex hull contains (0, 0).
list_t* link(list_t* list, list_t* jar, list_t* node)
{
	if (!list || !jar || !node)
		return;

	float cmin, c;

	list_t* begin = jar;
	list_t* end = jar;

	while (end->next)
		end = end->next;

	// 1. Select 2 hull points in shortest distance (for minimize the distance)
	cmin = vdist(begin->v, node->v) + vdist(end->v, node->v);
	for (list_t* tmp = jar; tmp->next != NULL; tmp = tmp->next)
	{
		c = vdist(tmp->v, node->v) + vdist(tmp->next->v, node->v);
		if (cmin > c)
		{
			cmin = c;
			begin = tmp;
			end = tmp->next;
		}
	}

	list_t* next;
	list_t* contain = NULL;

	while (!containv(jar, node->v))
	{
		begin = end;
		end = begin->next;

		if (!end)
			end = jar;

		next = node;
		cmin = v3cos(end->v, node->v, begin->v);

		for (list_t* tmp = list; tmp != NULL; tmp = tmp->next) // Make the list of nodes which contained in triangle(begin-end-(0, 0))
		{
			if (containv(jar, tmp->v))
				continue;

			c = v3cos(end->v, tmp->v, begin->v);
			if (c > cmin)
				contain = push(contain, tmp->v);
		}

		for (list_t* tmp = contain; tmp != NULL; tmp = tmp->next) // Select the maximum angle having node for convex hull.
		{
			c = v3cos(end->v, tmp->v, begin->v);
			if (c < cmin)
			{
				cmin = c;
				next = tmp;
			}
		}

		for (list_t* tmp = jar; tmp != NULL; tmp = tmp->next) // insert next node between begin->next
		{
			if (tmp == begin)
			{
				tmp = push(NULL, next->v);
				tmp->next = begin->next;
				begin->next = tmp;
				break;
			}
		}

		// since the contain list will be released, we exchange the link next to node in list.
		for (list_t* tmp = list; tmp != NULL; tmp = tmp->next)
		{
			if (tmp->v.x == next->v.x && tmp->v.y && next->v.y)
			{
				next = tmp;
				break;
			}
		}

		end = next;
		contain = free_list(contain);

		if (end == node) // the convex hull included (0, 0)
			break;
	}

	return jar; // Since we inserted the 'next' node, the jar value(header) can be changed. so we update it.
}

int main()
{
	int len;
	scanf("%d", &len);

	assertion(len > 0 && len <= 100);

	vertex_t tmpv = { 0.0, 0.0 };
	list_t* head = push(NULL, tmpv); // insert starting point first

	for (int i = 0; i < len; i++)
	{
		scanf("%f %f", &tmpv.x, &tmpv.y);
		head = push(head, tmpv);
	}
	len++;

	list_t* rs = jarvis(head); // create jarvis hull
	rs = link(head, rs, head); // relink jarvis to (0, 0)

	list_t* pv = NULL;
	list_t* cr = rs;

	double dist = 2; // Default length

	while (cr)
	{
		if (pv)
			dist += vdist(cr->v, pv->v);

		pv = cr;
		cr = cr->next;
	}
	dist += vdist(pv->v, rs->v); // add length of end to start again

	printf("%.2f", round(dist * 100) / 100); // display numbers under 2

	head = free_list(head); // mem release
	rs = free_list(rs);

	return 1;
}
