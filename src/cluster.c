/*-
 *cluster.c
 * created by: Kurt L. Manion
 * on: Sun., 27 May 2018
 */

#include "cluster.h"

#include <assert.h>
#include <stdarg.h>
#include <tgmath.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include "perms.h"

cluster_t* __pure2
cluster_make(
    pt_t *pt,
    ...)
{
	va_list va;
	cluster_t *c;
	pt_t *arg;

	c = (cluster_t *)malloc(cluster_sz);
	if (!c)
	    errx(1,"malloc failure, %s:%d", __FILE__, __LINE__);

	c->pt_lst = (pt_head_t *)malloc(sizeof(pt_head_t));
	if (!c->pt_lst)
	    errx(1,"malloc failure, %s:%d", __FILE__, __LINE__);

	STAILQ_INIT(c->pt_lst);

	if (pt) {
		STAILQ_INSERT_HEAD(c->pt_lst, pt, cdr);

		va_start(va, pt);

		while ((arg=va_arg(va,pt_t *))) {
			arg = pt_copy(arg);
			STAILQ_INSERT_HEAD(c->pt_lst, arg, cdr);
		}

		va_end(va);
	}

	return c;
}

cluster_t* __pure
cluster_free(
    cluster_t *c)
{
	pt_t *e;
	if (c) {
		if (c->pt_lst) {
			if (!STAILQ_EMPTY(c->pt_lst)) {
				STAILQ_FOREACH(e, c->pt_lst, cdr)
				    {
					STAILQ_REMOVE(c->pt_lst, e, _pt, cdr);
					pt_free(e);
				    }
			}
			free(c->pt_lst);
		}
		free(c);
	}

	return c = NULL;
}

/* assumes normalized cluster
 * returns the largest X and Y index,
 * which is one larger than the largest bound represented by a point,
 * since they are 0-indexed */
void
cluster_bounds(
    const cluster_t *const c,
    size_t *const restrict X,
    size_t *const restrict Y)
{
	pt_t *e;

	assert (c);
	assert (c->pt_lst);
	assert (!STAILQ_EMPTY(c->pt_lst));

	if (X)
	    *X = 0;
	if (Y)
	    *Y = 0;

	STAILQ_FOREACH(e, c->pt_lst, cdr)
	    {
		if (X)
		    *X = (size_t)fmax(*X, e->x);
		if (Y)
		    *Y = (size_t)fmax(*Y, e->y);
	    }

	if (X)
	    ++*X;
	if (Y)
	    ++*Y;

	return;
}

bool __pure
cluster_eq(
    const cluster_t *const restrict c0,
    const cluster_t *const restrict c1)
{
	if (!c0 || !c1)
	    return false;

	pt_t *e;
	STAILQ_FOREACH(e, c1->pt_lst, cdr)
	    {
		if (!cluster_contains_pt(c0, e))
		    return false;
	    }
	STAILQ_FOREACH(e, c0->pt_lst, cdr)
	    {
		if (!cluster_contains_pt(c1, e))
		    return false;
	    }

	return true;
}

bool __pure
cluster_pt_adj(
    const cluster_t *const c,
    const pt_t *const pt)
{
	pt_t *e;
	STAILQ_FOREACH(e, c->pt_lst, cdr)
	    {
		if (pt_pt_adj(pt, e))
		    return true;
	    }

	return false;
}

bool __pure
cluster_contains_pt(
    const cluster_t *const c,
    const pt_t *const pt)
{
	pt_t *e;
	STAILQ_FOREACH(e, c->pt_lst, cdr)
	    {
		if (pt_eq(pt, e))
		    return true;
	    }

	return false;
}

bool __pure
cluster_within_move(
    const cluster_t *const restrict c0,
    const cluster_t *const restrict c1)
{
	bool r;
	int8_t dy,dx;
	pt_t *start,*e,*pt,*last;
	cluster_t *move;
	cluster_t *aft; //after move
	static const int8_t tab[][2] = {
		    {-1,0},
		    {0,-1},	{0,1},
		    {1, 0}
	};
	static const size_t tab_sz = sizeof(tab)/2;

	/* these initiallizations are unnecessary,
	 * and only to silence lint warnings */
	aft = move = NULL;
	r = false;

	/* for all valid moves, do move on c1, and check if c0 is equal to it */
	STAILQ_FOREACH(start, c1->pt_lst, cdr) /* for all points in c1 */
	    {
		/* check for that one-piece move */
		move = cluster_make(pt_copy(start), NULL);

		if ((r=cluster_eq(c0, (aft=cluster_do_move(c1, move)))))
		    goto done;
		else
		    aft = cluster_free(aft);

		/* for each direction of each multi-piece move */
		for (size_t i=0; i<tab_sz; ++i)
		    {
			dy = tab[i][0];
			dx = tab[i][1];

			last = start;
			while (cluster_contains_pt(c1, (pt=pt_make(last->x+dx, last->y+dy))))
			    {
				STAILQ_INSERT_TAIL(move->pt_lst, pt, cdr);

				if ((r=cluster_eq(c0, (aft=cluster_do_move(c1, move)))))
				    goto done;
				else
				    aft = cluster_free(aft);

				last = pt;
			    }
			pt_free(pt);

			STAILQ_FOREACH(e, move->pt_lst, cdr)
			    {
				if (e != start) {
					STAILQ_REMOVE(move->pt_lst, e, _pt, cdr);
					pt_free(e);
				}
			    }
		    }

		move = cluster_free(move);
	    }

done:
	if (aft)
	    cluster_free(aft);

if (move)
    cluster_free(move);

return r;
}

bool __pure
cluster_match(
    const cluster_t *const restrict c0,
    const cluster_t *const restrict c1)
{
	static const fp_perm perms[] = {
		rotate, //90
		rotate, //180
		rotate, //270
		reflect_horz,
		reflect_vert,
		NULL
	};

	return cluster_eq(c0, c1)
		|| cluster_within_move(c0, c1)
		|| cluster_within_move(c1, c0)
		|| run_perms(perms, c0, c1)
		;
}

void __attribute__((unused))
cluster_print(
    const cluster_t *const cluster)
{
	size_t X,Y;
	char c;
	pt_t *e;

	if (!cluster || STAILQ_EMPTY(cluster->pt_lst))
	    return;

	cluster_bounds(cluster, &X, &Y);

	char matrix[Y][X];
	memset(matrix, '.', sizeof(matrix));

	c = 'A';

	STAILQ_FOREACH(e, cluster->pt_lst, cdr)
	    {
		matrix[e->y][e->x] = c++;
	    }

	for (size_t i=0; i<Y; ++i) {
		for (size_t j=0; j<X; ++j)
		    printf("%c", matrix[i][j]);
		putchar('\n');
	}
	fflush(stdout);

	return;
}

cluster_t*
cluster_annex(
    cluster_t *restrict dest,
    cluster_t *restrict src) // should be const, but _CONCAT is a destructive macro
{
	STAILQ_CONCAT(dest->pt_lst, src->pt_lst);
	return dest;
}

cluster_t* __pure
cluster_do_move(
    const cluster_t *const restrict c,
    const cluster_t *const restrict move)
{
	cluster_t *r;
	pt_t *pt,*pt_cpy;

	r = cluster_make(NULL);

	STAILQ_FOREACH(pt, c->pt_lst, cdr)
	    {
		if (!cluster_contains_pt(move, pt)) {
			pt_cpy = pt_copy(pt);
			STAILQ_INSERT_TAIL(r->pt_lst, pt_cpy, cdr);
		}
	    }

	if (STAILQ_EMPTY(r->pt_lst))
	    r = cluster_free(r);

	return r;
}

/* There is a non-destructive way to do this task, but it is not necessary,
 * and this saves the program from having to hang up and do a full mapping of
 * all clusters, rather normalizing as necessary without having to make a copy. */
cluster_t* __pure
cluster_normalize(
    cluster_t *c)
{
	pt_t d,*e;

	d.x = ~(1 << 31);
	d.y = ~(1 << 31);

	/* find cluster origin offset */
	STAILQ_FOREACH(e, c->pt_lst, cdr)
	    {
		d.x = fmin(d.x, e->x);
		d.y = fmin(d.y, e->y);

		if (d.x == 0 && d.x == d.y) // already normal with the origin
		    return c;
	    }

	STAILQ_FOREACH(e, c->pt_lst, cdr)
	    {
		e->x -= d.x;
		e->y -= d.y;
	    }

	return c;
}

/* cluster list declarations */

cluster_t* __pure
cluster_lst_match(
    const cluster_head_t *const cl,
    cluster_t *const c)
{
	cluster_t *e;

	cluster_normalize(c);

	STAILQ_FOREACH(e, cl, cdr)
	    {
		if (cluster_match(c, cluster_normalize(e)))
		    return e;
	    }
	return NULL;
}

bool __pure
cluster_lst_eqv(
    const cluster_head_t *const restrict cl0,
    const cluster_head_t *const restrict cl1)
{
	cluster_t *e;
	STAILQ_FOREACH(e, cl1, cdr)
	    {
		if (!cluster_lst_match(cl0, e))
		    return false;
	    }

	return true;
}

void
cluster_lst_print(
    const cluster_head_t *const cl,
    const size_t X,
    const size_t Y)
{
	char matrix[Y][X];
	char c;
	cluster_t *cluster;
	pt_t *pt;

	memset(matrix, '.', sizeof(matrix));
	c = 'A';

	STAILQ_FOREACH(cluster, cl, cdr)
	    {
		STAILQ_FOREACH(pt, cluster->pt_lst, cdr)
		    {
			matrix[pt->y][pt->x] = c;
		    }
		++c;
	    }

	for (size_t i=0; i<Y; ++i) {
		for (size_t j=0; j<X; ++j)
		    printf("%c", matrix[i][j]);
		putchar('\n');
	}

	return;
}

/* vim: set ts=8 sw=8 noexpandtab tw=79: */
