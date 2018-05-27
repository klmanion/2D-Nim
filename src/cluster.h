//cluster.h
// created by: Kurt L. Manion
// on: Sun., 27 May 2018
//

#ifndef _CLUSTER_H_
#define _CLUSTER_H_

#include <stdlib.h>
#include <sys/queue.h>
#include <stdbool.h>
#include "pt.h"

typedef
struct _cluster {
	pt_head_t	*pt_lst;
	STAILQ_ENTRY(_cluster) cdr;
} cluster_t;
#define cluster_sz sizeof(cluster_t)

typedef STAILQ_HEAD(cluster_head_s, _cluster) cluster_head_t;

__BEGIN_DECLS
cluster_t*	cluster_make __P((pt_t *,...)) __pure2;
cluster_t*	cluster_free __P((cluster_t *)) __pure;

void	cluster_bounds __P((const cluster_t *const,size_t *const restrict,
								size_t *const restrict)) __pure;

bool	cluster_eq __P((const cluster_t *const restrict,const cluster_t *const restrict)) __pure;
bool	cluster_pt_adj __P((const cluster_t *const,const pt_t *const)) __pure;
bool	cluster_contains_pt __P((const cluster_t *const,const pt_t *const)) __pure;
bool	cluster_within_move __P((const cluster_t *const,const cluster_t *const)) __pure;
bool	cluster_match __P((const cluster_t *const restrict,
							const cluster_t *const restrict)) __pure;

void	cluster_print __P((const cluster_t *const)) __attribute__((unused));

cluster_t*	cluster_annex __P((cluster_t *restrict,cluster_t *restrict));
cluster_t*	cluster_do_move __P((const cluster_t *const restrict,
									const cluster_t *const restrict)) __pure;
cluster_t*	cluster_normalize __P((cluster_t *)) __pure;

/* cluster list declarations */
cluster_t*	cluster_lst_match __P((const cluster_head_t *const cl,cluster_t *const c)) __pure;
bool	cluster_lst_eqv __P((const cluster_head_t *const restrict,
								const cluster_head_t *const restrict)) __pure;

void	cluster_lst_print __P((const cluster_head_t *const,const size_t,const size_t))
								__attribute__((unused));
__END_DECLS

#endif /* _CLUSTER_H_ */

/* vim: set ts=4 sw=4 noexpandtab tw=79: */
