#ifndef SNEPPX_RAFT_H
#define SNEPPX_RAFT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_raft_create(int node_id, const char** peers, size_t num_peers, int election_timeout_ms, int heartbeat_ms);
void SNEPPX_raft_destroy(void* raft);
int SNEPPX_raft_start(void* raft);
int SNEPPX_raft_stop(void* raft);
int SNEPPX_raft_is_leader(void* raft);
int SNEPPX_raft_get_leader_id(void* raft);
int SNEPPX_raft_propose(void* raft, const unsigned char* data, size_t len);
int SNEPPX_raft_get_state(void* raft, int* term, int* commit_index, int* last_applied);
int SNEPPX_raft_set_on_commit(void* raft, void (*cb)(void* raft, const unsigned char* data, size_t len, int index));
int SNEPPX_raft_add_server(void* raft, const char* peer);
int SNEPPX_raft_remove_server(void* raft, const char* peer);
#ifdef __cplusplus
}
#endif
#endif
