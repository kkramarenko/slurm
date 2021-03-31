#ifndef PMIXP_COLL_TREE_H
#define PMIXP_COLL_TREE_H

/* PMIx Tree collective */
typedef enum {
	PMIXP_COLL_TREE_SYNC,
	PMIXP_COLL_TREE_COLLECT,
	PMIXP_COLL_TREE_UPFWD,
	PMIXP_COLL_TREE_UPFWD_WSC, /* Wait for the upward Send Complete */
	PMIXP_COLL_TREE_UPFWD_WPC, /* Wait for Parent Contrib */
	PMIXP_COLL_TREE_DOWNFWD,
} pmixp_coll_tree_state_t;

typedef enum {
	PMIXP_COLL_TREE_SND_NONE,
	PMIXP_COLL_TREE_SND_ACTIVE,
	PMIXP_COLL_TREE_SND_DONE,
	PMIXP_COLL_TREE_SND_FAILED,
} pmixp_coll_tree_sndstate_t;

/* tree coll struct */
typedef struct {
	/* general information */
	pmixp_coll_tree_state_t state;

	/* tree topology */
	char *prnt_host;
	int prnt_peerid;
	char *root_host;
	int root_peerid;
	int chldrn_cnt;
	hostlist_t all_chldrn_hl;
	char *chldrn_str;
	int *chldrn_ids;

	/* collective state */
	bool contrib_local;
	uint32_t contrib_children;
	bool *contrib_chld;
	pmixp_coll_tree_sndstate_t ufwd_status;
	bool contrib_prnt;
	uint32_t dfwd_cb_cnt, dfwd_cb_wait;
	pmixp_coll_tree_sndstate_t dfwd_status;

	/* collective data */
	buf_t *ufwd_buf, *dfwd_buf;
	size_t serv_offs, dfwd_offset, ufwd_offset;
} pmixp_coll_tree_t;

/* tree coll functions*/
int pmixp_coll_tree_init(pmixp_coll_t *coll, hostlist_t *hl);
void pmixp_coll_tree_free(pmixp_coll_tree_t *tree);

pmixp_coll_t *pmixp_coll_tree_from_cbdata(void *cbdata);

int pmixp_coll_tree_local(pmixp_coll_t *coll, char *data, size_t size,
			  void *cbfunc, void *cbdata);
int pmixp_coll_tree_child(pmixp_coll_t *coll, uint32_t nodeid,
			  uint32_t seq, buf_t *buf);
int pmixp_coll_tree_parent(pmixp_coll_t *coll, uint32_t nodeid,
			   uint32_t seq, buf_t *buf);
void pmixp_coll_tree_bcast(pmixp_coll_t *coll);
bool pmixp_coll_tree_progress(pmixp_coll_t *coll, char *fwd_node,
			      void **data, uint64_t size);
int pmixp_coll_tree_unpack(buf_t *buf, pmixp_coll_type_t *type,
			   int *nodeid, pmixp_proc_t **r,
			   size_t *nr);
void pmixp_coll_tree_reset_if_to(pmixp_coll_t *coll, time_t ts);
void pmixp_coll_tree_log(pmixp_coll_t *coll);

#endif /* PMIXP_COLL_TREE_H */
