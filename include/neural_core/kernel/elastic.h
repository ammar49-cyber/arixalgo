#ifndef SNEPPX_ELASTIC_H
#define SNEPPX_ELASTIC_H

#include <stddef.h>
#include <stdint.h>

#define SNEPPX_ELASTIC_MAX_NODES 256
#define SNEPPX_ELASTIC_MAX_RESTARTS 10

typedef enum {
    SNEPPX_ELASTIC_OK = 0,
    SNEPPX_ELASTIC_JOINING = 1,
    SNEPPX_ELASTIC_LEAVING = 2,
    SNEPPX_ELASTIC_RECONFIG = 3,
    SNEPPX_ELASTIC_FAILED = 4
} SNEPPXElasticState;

typedef struct {
    int world_size;
    int rank;
    int num_nodes;
    int ranks_per_node;
    int heartbeat_timeout_ms;
    int max_restarts;
    int restart_count;

    SNEPPXElasticState state;
    int* global_ranks;
    int* node_ranks;
    int num_global_ranks;
    int num_node_ranks;

    int64_t last_reconfig_ns;
    int version;
    int enable_auto_scale;

    int (*barrier_fn)(void);
    int (*checkpoint_restore_fn)(int version);
} SNEPPXElasticTraining;

int  SNEPPX_elastic_init(SNEPPXElasticTraining** et, int world_size, int rank,
                         int num_nodes, int ranks_per_node);
int  SNEPPX_elastic_join(SNEPPXElasticTraining* et, int new_rank,
                         const char* addr);
int  SNEPPX_elastic_leave(SNEPPXElasticTraining* et, int leaving_rank);
int  SNEPPX_elastic_handle_failure(SNEPPXElasticTraining* et, int failed_rank);
int  SNEPPX_elastic_reconfigure(SNEPPXElasticTraining* et);
int  SNEPPX_elastic_get_new_topology(SNEPPXElasticTraining* et,
                                     int* new_world_size, int* new_rank);
void SNEPPX_elastic_set_barrier(SNEPPXElasticTraining* et,
                                int (*fn)(void));
void SNEPPX_elastic_set_checkpoint_restore(SNEPPXElasticTraining* et,
                                           int (*fn)(int version));
void SNEPPX_elastic_destroy(SNEPPXElasticTraining* et);

#endif
