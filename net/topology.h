#ifndef ARIX_NET_TOPOLOGY_H
#define ARIX_NET_TOPOLOGY_H
/*
 * Network Topology Abstraction — v1.0 (distributed communication patterns)
 *
 * PURPOSE: Maps nodes to logical network positions for collective
 * communication.  Supports ring (all-reduce), tree (broadcast), and
 * arbitrary graph (gossip) topologies.  Used by the distributed
 * training coordinator to choose communication schedules.
 *
 * DEPENDENCIES: arix_grpc_service.h, arix_socket_comm.h
 * VERSION: v1.0
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ARIX_TOPOLOGY_RING,
    ARIX_TOPOLOGY_TREE,
    ARIX_TOPOLOGY_GRAPH,
} ArixTopologyType;

typedef struct {
    int    rank;
    int    prev_rank;
    int    next_rank;
    int    parent_rank;
    int    children[16];
    int    num_children;
    char   host[256];
    int    data_port;
} ArixTopologyNode;

typedef struct {
    ArixTopologyType  type;
    int               world_size;
    ArixTopologyNode* nodes;
} ArixTopology;

/* ---------- Topology construction ---------- */
ArixTopology* arix_topology_create_ring(int world_size);
ArixTopology* arix_topology_create_tree(int world_size, int branching_factor);
ArixTopology* arix_topology_create_graph(int world_size, const int* adjacency_matrix);
void          arix_topology_destroy(ArixTopology* topo);

/* ---------- Neighbor queries ---------- */
int arix_topology_get_prev(const ArixTopology* topo, int rank);
int arix_topology_get_next(const ArixTopology* topo, int rank);
int arix_topology_get_parent(const ArixTopology* topo, int rank);
int arix_topology_get_children(const ArixTopology* topo, int rank, int** children, int* count);

/* ---------- Route calculation (v1.0) ---------- */
int  arix_topology_compute_route(const ArixTopology* topo, int src, int dst, int** path, int* path_len);
void arix_topology_free_route(int* path);

#ifdef __cplusplus
}
#endif

#endif /* ARIX_NET_TOPOLOGY_H */
