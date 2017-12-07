/**
 * constants.hpp
 */

/** Server host */
#define HOST "127.0.0.1"
/** Listenning port for TCP server */
#define PORT 17000

/** Message delimiter */
#define MSG_DELIMETER '\n'
/** Message delimiter string */
#define DELIMETER_STR "\n"

/** Size of receive buffer */
#define RECV_BUFFER         1024
/** Size of send buffer */
#define SEND_BUFFER         PACKET_SIZE
/** Size of packet */
#define PACKET_SIZE			50