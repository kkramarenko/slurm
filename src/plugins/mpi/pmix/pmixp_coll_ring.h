#ifndef PMIXP_COLL_RING_H
#define PMIXP_COLL_RING_H

#include "pmixp_coll.h"

/* PMIx Ring collective */
typedef enum {
	PMIXP_COLL_RING_SYNC,
	PMIXP_COLL_RING_PROGRESS,
	PMIXP_COLL_RING_FINALIZE,
} pmixp_ring_state_t;

typedef struct {
	/* ptr to coll data */
	struct pmixp_coll_t *coll;

	/* context data */
	bool in_use;
	uint32_t seq;
	bool contrib_local;
	uint32_t contrib_prev;
	uint32_t forward_cnt;
	bool *contrib_map;
	pmixp_ring_state_t state;
	buf_t *ring_buf;
} pmixp_coll_ring_ctx_t;

/* coll ring struct */
typedef struct {
	/* next node id */
	int next_peerid;
	/* coll contexts data */
	pmixp_coll_ring_ctx_t ctx_array[PMIXP_COLL_RING_CTX_NUM];
	/* buffer pool to ensure parallel sends of ring data */
	List fwrd_buf_pool;
	List ring_buf_pool;
} pmixp_coll_ring_t;

typedef struct {
	uint32_t type;
	uint32_t contrib_id;
	uint32_t seq;
	uint32_t hop_seq;
	uint32_t nodeid;
	size_t msgsize;
} pmixp_coll_ring_msg_hdr_t;

typedef struct {
	size_t size;
	char *ptr;
	uint32_t contrib_id;
	uint32_t hop_seq;
} pmixp_coll_msg_ring_data_t;

/* ring coll functions */
int pmixp_coll_ring_init(pmixp_coll_t *coll, hostlist_t *hl);
void pmixp_coll_ring_free(pmixp_coll_ring_t *coll_ring);
int pmixp_coll_ring_check(pmixp_coll_t  *coll, pmixp_coll_ring_msg_hdr_t *hdr);
int pmixp_coll_ring_local(pmixp_coll_t  *coll, char *data, size_t size,
			  void *cbfunc, void *cbdata);
int pmixp_coll_ring_neighbor(pmixp_coll_t *coll, pmixp_coll_ring_msg_hdr_t *hdr,
			     buf_t *buf);
void pmixp_coll_ring_reset(pmixp_coll_ring_ctx_t *coll);
int pmixp_coll_ring_unpack(buf_t *buf, pmixp_coll_type_t *type,
			   pmixp_coll_ring_msg_hdr_t *ring_hdr,
			   pmixp_proc_t **r, size_t *nr);
void pmixp_coll_ring_reset_if_to(pmixp_coll_t  *coll, time_t ts);
pmixp_coll_ring_ctx_t *pmixp_coll_ring_ctx_select(pmixp_coll_t *coll,
						  const uint32_t seq);
pmixp_coll_t *pmixp_coll_ring_from_cbdata(void *cbdata);
void pmixp_coll_ring_free(pmixp_coll_ring_t *ring);
void pmixp_coll_ring_log(pmixp_coll_t *coll);

#endif /* PMIXP_COLL_RING_H */
