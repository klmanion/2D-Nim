//pt.h
// created by: Kurt L. Manion
// on: Sun., 27 May 2018
//

#ifndef _PT_H_
#define _PT_H_

#include <stdlib.h>
#include <sys/queue.h>
#include <stdbool.h>

typedef
struct _pt {
	int	x;
	int	y;
	STAILQ_ENTRY(_pt) cdr;
} pt_t;
#define pt_sz sizeof(pt_t)

typedef STAILQ_HEAD(pt_head_s, _pt) pt_head_t;

__BEGIN_DECLS
pt_t*	pt_make __P((const int,const int)) __pure2;
pt_t*	pt_copy __P((const pt_t *const)) __pure;
pt_t*	pt_free __P((pt_t *)) __pure;

bool	pt_eq __P((const pt_t *const restrict,const pt_t *const restrict)) __pure;
bool	pt_pt_adj __P((const pt_t *const restrict,const pt_t *const restrict)) __pure;
__END_DECLS

#endif /* !_PT_H_ */

/* vim: set ts=4 sw=4 noexpandtab tw=79: */
