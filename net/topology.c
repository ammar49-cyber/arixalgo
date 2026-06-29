/*
 * Network Topology Implementation — SKELETON
 * VERSION: v1.0
 */

#include "topology.h"
#include <stdlib.h>
#include <string.h>

ArixTopology* arix_topology_create_ring(int world_size) {
    (void)world_size;
    ArixTopology* t = (ArixTopology*)calloc(1, sizeof(ArixTopology));
    if (t) t->type = ARIX_TOPOLOGY_RING;
    return t;
}

ArixTopology* arix_topology_create_tree(int world_size, int branching_factor) {
    (void)world_size; (void)branching_factor;
    ArixTopology* t = (ArixTopology*)calloc(1, sizeof(ArixTopology));
    if (t) t->type = ARIX_TOPOLOGY_TREE;
    return t;
}

ArixTopology* arix_topology_create_graph(int world_size, const int* adjacency_matrix) {
    (void)world_size; (void)adjacency_matrix;
    ArixTopology* t = (ArixTopology*)calloc(1, sizeof(ArixTopology));
    if (t) t->type = ARIX_TOPOLOGY_GRAPH;
    return t;
}

void arix_topology_destroy(ArixTopology* topo) {
    if (topo) free(topo->nodes);
    free(topo);
}

int arix_topology_get_prev(const ArixTopology* topo, int rank) {
    (void)topo; (void)rank; return -1;
}

int arix_topology_get_next(const ArixTopology* topo, int rank) {
    (void)topo; (void)rank; return -1;
}

int arix_topology_get_parent(const ArixTopology* topo, int rank) {
    (void)topo; (void)rank; return -1;
}

int arix_topology_get_children(const ArixTopology* topo, int rank, int** children, int* count) {
    (void)topo; (void)rank;
    if (children) *children = NULL;
    if (count) *count = 0;
    return 0;
}

int arix_topology_compute_route(const ArixTopology* topo, int src, int dst, int** path, int* path_len) {
    (void)topo; (void)src; (void)dst;
    if (path) *path = NULL;
    if (path_len) *path_len = 0;
    return 0;
}

void arix_topology_free_route(int* path) { free(path); }
