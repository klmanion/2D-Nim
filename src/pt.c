/*-
 *pt.c
 * created by: Kurt L. Manion
 * on: Sun., 27 May 2018
 */

#include "pt.h"

#include <stdint.h>
#include <err.h>
#include <string.h>
#include <tgmath.h>

pt_t* __pure2
pt_make(
    const int x,
    const int y)
{
	pt_t *pt;

	pt = (pt_t *)malloc(pt_sz);
	if (!pt)
	    errx(1,"malloc failure, %s:%d", __FILE__, __LINE__);

	pt->x = x;
	pt->y = y;

	return pt;
}

pt_t* __pure
pt_copy(
    const pt_t *const orig)
{
	pt_t *cpy;

	cpy = (pt_t *)malloc(pt_sz);
	if (!cpy)
	    errx(1,"malloc failure, %s:%d", __FILE__, __LINE__);

	return memcpy(cpy, orig, pt_sz);
}

pt_t* __pure
pt_free(
    pt_t *pt)
{
	if (pt)
	    free(pt);

	return pt = NULL;
}

bool __pure
pt_eq(
    const pt_t *const restrict pt0,
    const pt_t *const restrict pt1)
{
	if (!pt0 || !pt1)
	    return false;

	return pt0->x == pt1->x
		&& pt0->y == pt1->y
		;
}

bool __pure
pt_pt_adj(
    const pt_t *const restrict pt1,
    const pt_t *const restrict pt2)
{
	int8_t yd,xd;
	static const int8_t tab[][2] = {
			{-1,0},
		{0,-1},		{0,1},
			{1,0}
	};
	static const size_t tab_sz = sizeof(tab)/2;

	for (size_t i=0; i<tab_sz; ++i)
	    {
		yd = tab[i][0];
		xd = tab[i][1];

		if (pt1->x + xd == pt2->x
			&& pt1->y + yd == pt2->y)
		    return true;
	    }

	return false;
}

/* vim: set ts=8 sw=8 noexpandtab tw=79: */
