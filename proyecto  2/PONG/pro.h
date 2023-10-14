#ifndef PROTO_H
#define PROTO_H
#include "glo.h"

void *sender();
void *receiver();
void start_server(pthread_t threads[2], pthread_attr_t attr);

#endif