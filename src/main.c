//main.c
// created by: Kurt L. Manion
// on: Sat., 19 May 2018
// challenge from: Berkeley Fall 2001 #4, 2D-Nims
//

/*-
 * DESCRIPTION
 * Check if two boards of the game 2D-Nims are equivalent,
 * in the sense that any winning strategy of the one board would apply for
 * the other as well.
 *
 * GIVEN
 * o dimensions of a board
 * o two boards to be compared of thoes dimensions
 *
 * DESIDERATUM
 * o reduce boards to their major clusters
 * o bijectively relate and match each maps clusters to each other
 * 		- for every cluster in the first list find its correlate in the 
 * 			second
 * 		- in finding the correlate, each pair of clusters being compared
 * 			must test for permutations:
 * 				• rotation (4);
 * 				• reflection (4);
 * 				• and move taken.
 * 			All others are taken into account by the abstract
 * 			representation of boards as sets of clusters.
 * o print out whether boards are equivalent
 */

#ifndef DEBUG
# define NDEBUG /* turns off assertions */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <sys/queue.h>
#include <stdint.h>
#include <stdbool.h>
#include <tgmath.h>
#include <assert.h>

#include "pt.h"
#include "cluster.h"

int
main(
	int argc,
	char *const argv[])
{
	int X,Y,xi,yi;
	cluster_head_t cla[2];
	pt_head_t ptl;
	pt_t *pt,*e;
	cluster_t *cluster,*saved;

	for (unsigned set=1; !feof(stdin); ++set)
	{
		scanf("%d %d ",&X, &Y);

		for (size_t i=0; i<2; ++i)
		{
			cluster_head_t *cl;
			cl = &cla[i];
			STAILQ_INIT(cl);
			STAILQ_INIT(&ptl);
			while (scanf("%d %d ", &xi, &yi),xi != -1)
			{
				pt = pt_make(xi, yi);

				saved = NULL;
				if (!STAILQ_EMPTY(cl)) {
					STAILQ_FOREACH(cluster, cl, cdr)
					{
						if (cluster_pt_adj(cluster, pt)) {
							if (!saved) {
								STAILQ_INSERT_HEAD(cluster->pt_lst, pt, cdr);
								saved = cluster;
							} else {
								// pt has matched with two or more clusters,
								// thus thoes clusters must be annexed
								STAILQ_REMOVE(cl, cluster, _cluster, cdr);
								saved = cluster_annex(saved, cluster);
							}
						}
					}
				}

				if (!saved) {
					cluster = NULL;
					if (!STAILQ_EMPTY(&ptl)) {
						STAILQ_FOREACH(e, &ptl, cdr)
						{
							if (pt_pt_adj(pt, e)) {
								STAILQ_REMOVE(&ptl, e, _pt, cdr);
								if (!cluster)
									cluster = cluster_make(pt, e, NULL);
								else
									STAILQ_INSERT_HEAD(cluster->pt_lst, e, cdr);
							}
						}
					}
					if (cluster)
						STAILQ_INSERT_HEAD(cl, cluster, cdr);
					else
						STAILQ_INSERT_HEAD(&ptl, pt, cdr);
				}
			}

			if (!STAILQ_EMPTY(&ptl)) {
				STAILQ_FOREACH(pt, &ptl, cdr) {
					STAILQ_REMOVE(&ptl, pt, _pt, cdr);
					pt_free(pt);
				}
			}
		}

		printf("Set %u:\n", set);

		cluster_lst_eqv(&cla[0], &cla[1])
			? puts("boards are equivalent")
			: puts("boards are not equivalent");
		putchar('\n');

		for (size_t i=0; i<2; ++i)
		{
			STAILQ_FOREACH(cluster, &cla[i], cdr)
			{
				STAILQ_REMOVE(&cla[i], cluster, _cluster, cdr);
				cluster_free(cluster);
			}
		}
	}

	return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 noexpandtab tw=79: */
