/**
 * constants.hpp
 */

/** Listenning port for TCP server */
#define PORT 10000

#define CONSOLE_BUFFER 100000

/** Message delimiter */
#define MSG_DELIMETER '\n'

/* Deadlines */

/** Deadline for connection start (seconds) */
#define EXPIRE_CONNECT      60
/** Deadline for read start (seconds) */
#define EXPIRE_READ         30
/** Deadline for heartbeat (seconds) */
#define EXPIRE_HEARTBEAT    10
