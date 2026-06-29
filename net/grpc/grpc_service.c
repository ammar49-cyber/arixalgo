/*
 * gRPC Service Implementation — SKELETON
 * VERSION: v1.0
 */

#include "grpc_service.h"
#include <stdlib.h>
#include <string.h>

int arix_grpc_server_start(ArixGRPCServer** server, int port) {
    (void)port;
    if (!server) return -1;
    *server = (ArixGRPCServer*)calloc(1, sizeof(ArixGRPCServer));
    if (*server) (*server)->port = port;
    return *server ? 0 : -1;
}

void arix_grpc_server_stop(ArixGRPCServer* server) { free(server); }
void arix_grpc_server_wait(ArixGRPCServer* server) { (void)server; }

ArixGRPCStub* arix_grpc_stub_create(const char* target) {
    if (!target) return NULL;
    ArixGRPCStub* stub = (ArixGRPCStub*)calloc(1, sizeof(ArixGRPCStub));
    if (stub) { strncpy(stub->target, target, sizeof(stub->target)-1); }
    return stub;
}

void arix_grpc_stub_destroy(ArixGRPCStub* stub) { free(stub); }

int arix_grpc_register_node(ArixGRPCStub* stub, const ArixGRPCNodeInfo* info) {
    (void)stub; (void)info; return 0;
}
int arix_grpc_get_world_size(ArixGRPCStub* stub, int* size) {
    (void)stub; if (size) *size = 1; return 0;
}
int arix_grpc_get_rank(ArixGRPCStub* stub, int* rank) {
    (void)stub; if (rank) *rank = 0; return 0;
}
int arix_grpc_barrier(ArixGRPCStub* stub) { (void)stub; return 0; }
int arix_grpc_all_gather(ArixGRPCStub* stub, const void* send_buf, void** recv_buf, size_t elem_size) {
    (void)stub; (void)send_buf; (void)recv_buf; (void)elem_size; return 0;
}
int arix_grpc_send_tensor(ArixGRPCStub* stub, const void* tensor, int dest_rank) {
    (void)stub; (void)tensor; (void)dest_rank; return 0;
}
int arix_grpc_recv_tensor(ArixGRPCStub* stub, void** tensor, int src_rank) {
    (void)stub; (void)src_rank; if (tensor) *tensor = NULL; return 0;
}
int arix_grpc_set_auth_token(ArixGRPCStub* stub, const char* token) {
    (void)stub; (void)token; return 0;
}
const char* arix_grpc_status_string(ArixGRPCStatus status) {
    switch (status) {
        case ARIX_GRPC_OK: return "OK";
        case ARIX_GRPC_UNAVAILABLE: return "UNAVAILABLE";
        case ARIX_GRPC_DEADLINE_EXCEEDED: return "DEADLINE_EXCEEDED";
        case ARIX_GRPC_INTERNAL: return "INTERNAL";
        case ARIX_GRPC_UNAUTHENTICATED: return "UNAUTHENTICATED";
        default: return "UNKNOWN";
    }
}
