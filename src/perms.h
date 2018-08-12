/*-
 *perms.h
 * created by: Kurt L. Manion
 * on: Sun., 27 2018
 */

#ifndef _PERMS_H_
#define _PERMS_H_

#include <stdlib.h>
#include "cluster.h"

typedef cluster_t* (*const fp_perm)(const cluster_t *const);

__BEGIN_DECLS
bool	run_perms __P((const fp_perm *,const cluster_t *const restrict,
		       const cluster_t *const restrict)) __pure;

cluster_t*	rotate __P((const cluster_t *const));
cluster_t*	reflect_vert __P((const cluster_t *const));
cluster_t*	reflect_horz __P((const cluster_t *const));
__END_DECLS

#endif /* !_PERMS_H_ */

/* vim: set ts=8 sw=8 noexpandtab tw=79: */
