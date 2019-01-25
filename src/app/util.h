#ifndef __UTIL_H__
#define __UTIL_H__

#include "core_errno.h"
#include "core_hash.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int get_cpu_cores();

float get_cpu_load();

/**
 * Randomaly select `select_count` non-duplicate indices among `haystack_count` items.
 * 
 * @par Example:
 * 
 * @code
 * {
 *     int n_total = 8;
 *     int n_select = 3;
 *     int selected[n_select];
 *     
 *     d_assert(rand_select_non_dup(n_total, n_select, selected) == CORE_OK, return CORE_ERROR,);
 *     
 *     int i;
 *     for (i = 0; i < n_select; i++)
 *         printf("selected[%d] = %d\n", i, selected[i]);
 * }
 * @endcode
 */
status_t rand_select_non_dup(int haystack_count, int select_count, int *sel_indices);

/**
 * Randomaly select `select_count` non-duplicate items from `haystack`.
 */
status_t ht_rand_select_non_dup(hash_t *haystack, int select_count, void **sel_items);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UTIL_H__ */
