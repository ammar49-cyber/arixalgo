#include "raft.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void* SNEPPX_raft_create(int node_id, const char** peers, size_t num_peers, int election_timeout_ms, int heartbeat_ms) { (void)node_id; (void)peers; (void)num_peers; (void)election_timeout_ms; (void)heartbeat_ms; return calloc(1, 64); }
void SNEPPX_raft_destroy(void* raft) { free(raft); }
int SNEPPX_raft_start(void* raft) { (void)raft; return 0; }
int SNEPPX_raft_stop(void* raft) { (void)raft; return 0; }
int SNEPPX_raft_is_leader(void* raft) { (void)raft; return 0; }
int SNEPPX_raft_get_leader_id(void* raft) { (void)raft; return 0; }
int SNEPPX_raft_propose(void* raft, const unsigned char* data, size_t len) { (void)raft; (void)data; (void)len; return 0; }
int SNEPPX_raft_get_state(void* raft, int* term, int* commit_index, int* last_applied) { (void)raft; (void)term; (void)commit_index; (void)last_applied; return 0; }
int SNEPPX_raft_set_on_commit(void* raft, void (*cb)(void* raft, const unsigned char* data, size_t len, int index)) { (void)raft; (void)cb; return 0; }
int SNEPPX_raft_add_server(void* raft, const char* peer) { (void)raft; (void)peer; return 0; }
int SNEPPX_raft_remove_server(void* raft, const char* peer) { (void)raft; (void)peer; return 0; }
