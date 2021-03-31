/*****************************************************************************\
 **  pmix_coll.h - PMIx collective primitives
 *****************************************************************************
 *  Copyright (C) 2014-2015 Artem Polyakov. All rights reserved.
 *  Copyright (C) 2015-2017 Mellanox Technologies. All rights reserved.
 *  Written by Artem Polyakov <artpol84@gmail.com, artemp@mellanox.com>,
 *             Boris Karasev <karasev.b@gmail.com, boriska@mellanox.com>.
 *
 *  This file is part of Slurm, a resource management program.
 *  For details, see <https://slurm.schedmd.com/>.
 *  Please also read the included file: DISCLAIMER.
 *
 *  Slurm is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  In addition, as a special exception, the copyright holders give permission
 *  to link the code of portions of this program with the OpenSSL library under
 *  certain conditions as described in each individual source file, and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify file(s) with this exception, you may extend this
 *  exception to your version of the file(s), but you are not obligated to do
 *  so. If you do not wish to do so, delete this exception statement from your
 *  version.  If you delete this exception statement from all source files in
 *  the program, then also delete it here.
 *
 *  Slurm is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Slurm; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 \*****************************************************************************/

#ifndef PMIXP_COLL_H
#define PMIXP_COLL_H
#include "pmixp_common.h"
#include "pmixp_debug.h"

#define PMIXP_COLL_DEBUG 1
#define PMIXP_COLL_RING_CTX_NUM 3

typedef enum {
	PMIXP_COLL_TYPE_FENCE_TREE = 0,
	PMIXP_COLL_TYPE_FENCE_RING,
	/* reserve coll fence ids up to 15 */
	PMIXP_COLL_TYPE_FENCE_MAX = 15,

	PMIXP_COLL_TYPE_CONNECT,
	PMIXP_COLL_TYPE_DISCONNECT
} pmixp_coll_type_t;

typedef enum {
	PMIXP_COLL_CPERF_TREE = PMIXP_COLL_TYPE_FENCE_TREE,
	PMIXP_COLL_CPERF_RING = PMIXP_COLL_TYPE_FENCE_RING,
	PMIXP_COLL_CPERF_MIXED = PMIXP_COLL_TYPE_FENCE_MAX,
	PMIXP_COLL_CPERF_BARRIER
} pmixp_coll_cperf_mode_t;

int pmixp_hostset_from_ranges(const pmixp_proc_t *procs, size_t nprocs,
			      hostlist_t *hl_out);

typedef enum {
	PMIXP_COLL_REQ_PROGRESS,
	PMIXP_COLL_REQ_SKIP,
	PMIXP_COLL_REQ_FAILURE
} pmixp_coll_req_state_t;

/* General coll struct */
typedef struct pmixp_coll_s {
#ifndef NDEBUG
#define PMIXP_COLL_STATE_MAGIC 0xC011CAFE
	int magic;
#endif
	/* element-wise lock */
	pthread_mutex_t lock;

	/* collective state */
	uint32_t seq;

	/* general information */
	pmixp_coll_type_t type;

	/* PMIx collective id */
	struct {
		pmixp_proc_t *procs;
		size_t nprocs;
	} pset;
	int my_peerid;
	int peers_cnt;
#ifdef PMIXP_COLL_DEBUG
	hostlist_t peers_hl;
#endif

	/* libpmix callback data */
	void *cbfunc;
	void *cbdata;

	/* timestamp for stale collectives detection */
	time_t ts, ts_next;

	/* coll states */
	union {
		pmixp_coll_tree_t tree;
		pmixp_coll_ring_t ring;
	} state;
} pmixp_coll_t;

/* common coll func */
int pmixp_coll_check(pmixp_coll_t *coll, uint32_t seq);
int pmixp_coll_init(pmixp_coll_t *coll, pmixp_coll_type_t type,
		    const pmixp_proc_t *procs, size_t nprocs);
int pmixp_coll_contrib_local(pmixp_coll_t *coll, pmixp_coll_type_t type,
			     char *data, size_t ndata,
			     void *cbfunc, void *cbdata);
void pmixp_coll_free(pmixp_coll_t *coll);
void pmixp_coll_localcb_nodata(pmixp_coll_t *coll, int status);
int pmixp_coll_belong_chk(const pmixp_proc_t *procs, size_t nprocs);
void pmixp_coll_log(pmixp_coll_t *coll);

#endif /* PMIXP_COLL_H */
