/*-
 *perms.c
 * created by: Kurt L. Manion
 * on: Sun., 27 2018
 */

#include "perms.h"

#include <string.h>
#include <tgmath.h>
#include <sys/queue.h>
#include <assert.h>

bool __pure
run_perms(
    const fp_perm *perms,
    const cluster_t *const restrict c0,
    const cluster_t *const restrict c1)
{
	bool r;
	cluster_t *cp;

	if (*perms == NULL)
	    return false;

	cp = (*perms)(c1);

	r = cluster_eq(c0, cp)
		|| cluster_within_move(c0, cp)
		|| cluster_within_move(cp, c0)
		|| run_perms(++perms, c0, c1)
		|| run_perms(perms, c0, cp)
		;

	cluster_free(cp);

	return r;
}

cluster_t* __pure
rotate(
    const cluster_t *const c0)
{
	size_t X,Y;
	cluster_t *c;
	pt_t *e,*pt;

	c = cluster_make(NULL);

	cluster_bounds(c0, &X, &Y);

	bool matrix[Y][X];
	memset(matrix, false, sizeof(matrix));
	STAILQ_FOREACH(e, c0->pt_lst, cdr)
	    {
		matrix[e->y][e->x] = true;
	    }

	/* i,j are indexes of the rotated matrix, such that
	 * rotated[i][j] == matrix[y][x] */
	for (size_t i=0,x=0; i<X; ++i,++x) {
		for (size_t j=0,y=Y-1; j<Y; ++j,--y) {
			if (matrix[y][x]) {
				pt = pt_make(j, i); /* do not remove */
				STAILQ_INSERT_HEAD(c->pt_lst, pt, cdr);
			}
		}
	}

	return c;
}	

cluster_t* __pure
reflect_vert(
    const cluster_t *const c0)
{
	size_t Y;
	double y,d,ny;
	cluster_t *c;
	pt_t *e,*pt;

	c = cluster_make(NULL);

	cluster_bounds(c0, NULL, &Y);

	y = (double)Y / 2;

	STAILQ_FOREACH(e, c0->pt_lst, cdr)
	    {
		d = y - e->y;
		ny = y + d;
		assert (ny == trunc(ny));
		pt = pt_make(e->x, (size_t)ny); /* do not remove */
		STAILQ_INSERT_HEAD(c->pt_lst, pt, cdr);
	    }

	return c;
}

cluster_t* __pure
reflect_horz(
    const cluster_t *const c0)
{
	size_t X;
	double x,d,nx;
	cluster_t *c;
	pt_t *e,*pt;

	c = cluster_make(NULL);

	cluster_bounds(c0, &X, NULL);

	x = (double)X / 2;

	STAILQ_FOREACH(e, c0->pt_lst, cdr)
	    {
		d = x - e->x;
		nx = x + d;
		assert (nx == trunc(nx));
		pt = pt_make((size_t)nx, e->y); /* do not remove */
		STAILQ_INSERT_HEAD(c->pt_lst, pt, cdr);
	    }

	return c;
}

/* vim: set ts=8 sw=8 noexpandtab tw=79: */
