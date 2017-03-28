/*
 * Contains common definitions for the client-server protocol.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

/* How long to wait for a keep alive packet before disconnecting */
const int TIMEOUT_LEN = 10;

const qint8 PACKET_KEEP_ALIVE = 0;

#endif // !PROTOCOL_H
