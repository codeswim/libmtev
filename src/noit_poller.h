/*
 * Copyright (c) 2007, OmniTI Computer Consulting, Inc.
 * All rights reserved.
 */

#ifndef _NOIT_POLLER_H
#define _NOIT_POLLER_H

#include "noit_defines.h"

#include <uuid/uuid.h>
#include <netinet/in.h>

#include "eventer/eventer.h"
#include "utils/noit_hash.h"

/*
 * Checks:
 *  attrs:
 *   UUID
 *   host (target)
 *   check (module)
 *   name (identifying the check to the user if
 *         multiple checks of the same module are specified)
 *   config (params for the module)
 *   period (ms)
 *   timeout (ms)
 *  transient:
 *   eventer_t (fire)
 *   stats_t [inprogress, last]
 *   closure
 */

#define NP_RUNNING  0x00000001
#define NP_KILLED   0x00000002
#define NP_DISABLED 0x00000004

#define NP_UNKNOWN 0               /* stats_t.{available,state} */
#define NP_AVAILABLE 1             /* stats_t.available */
#define NP_UNAVAILABLE -1          /* stats_t.available */
#define NP_BAD 1                   /* stats_t.state */
#define NP_GOOD 2                  /* stats_t.state */

typedef struct {
  struct timeval whence;
  int16_t available;
  int16_t state;
  u_int32_t duration;
  char *status;
} stats_t;

typedef struct {
  uuid_t checkid;
  int8_t target_family;
  union {
    struct in_addr addr;
    struct in6_addr addr6;
  } target_addr;
  char *target;
  char *module;
  char *name;
  noit_hash_table *config;
  u_int32_t period;
  u_int32_t timeout;
  u_int32_t flags;             /* NP_KILLED, NP_RUNNING */

  eventer_t fire_event;
  struct timeval last_fire_time;
  struct {
    stats_t current;
    stats_t previous;
  } stats;
  void *closure;
} * noit_check_t;

API_EXPORT(void) noit_poller_init();
API_EXPORT(void) noit_poller_load_checks();

API_EXPORT(int)
  noit_poller_schedule(const char *target,
                       const char *module,
                       const char *name,
                       noit_hash_table *config,
                       u_int32_t period,
                       u_int32_t timeout,
                       uuid_t in,
                       uuid_t out);

API_EXPORT(int)
  noit_poller_deschedule(uuid_t in);

API_EXPORT(noit_check_t)
  noit_poller_lookup(uuid_t in);

API_EXPORT(void)
  noit_poller_set_state(noit_check_t check, stats_t *newstate);

#endif