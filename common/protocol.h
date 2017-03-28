/*
 * Contains common definitions for the client-server protocol.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

/* How long to wait for a keep alive packet before disconnecting */
const int TIMEOUT_LEN = 10;

typedef quint8 packet_t;

/* 
 * Keep Alive packet. 
 *
 * Spec: <PACKET_KEEP_ALIVE> 
 * Direction: Both ways
 */
const packet_t PACKET_KEEP_ALIVE = 0;
/*
 * Request Join packet. Indicates the client wants to join
 * a new game. The subsequent string sets the player's name.
 * If the string is empty, then the player's name is unchanged.
 * If the string is empty initially, then a default name will
 * be assigned.
 *
 * Spec: <PACKET_REQUEST_JOIN> <QString: name> 
 * Direction: Client to Server
 */
const packet_t PACKET_REQUEST_JOIN = 1;
/*
 * Queued packet. Informs the client it has been queued to join a
 * game (i.e., a PACKET_REQUEST_JOIN has been successfully processed).
 *
 * Spec: <PACKET_QUEUED>
 * Direction: Server to Client
 */
const packet_t PACKET_QUEUED = 2;
/*
 * Game Join packet. Informs the client it is now in game and provides
 * initial state information.
 *
 * Spec: <PACKET_GAME_JOIN> <quint8: id> <quint32: current_tick> <quint8: score>
 *       <quint8: player_count> {<quint8: player_id> <QString: player_name>}[player_count times: player name map]
 *       {<quint8: player_id> <quint8: score>}[5 times: leader board in descending order]
 *       {<quint32: board_state>}[31 * 31 times, populating the local board state, L to R, T to B]
 * Direction: Server to Client
 */
const packet_t PACKET_GAME_JOIN = 3;
/*
 * Players Update packet. Informs the client that the currently connected players have changed.
 *
 * Spec: <PACKET_PLAYERS_UPDATE> <quint8: player_count> {<quint8: player_id> <QString: player_name>}[player_count times: player name map]
 * Direction: Server to Client
 */
const packet_t PACKET_PLAYERS_UPDATE = 4;
/*
 * Leaderboard Update packet. Informs the client that the leaderboard has changed.
 * 
 * Spec: <PACKET_LEADERBOARD_UPDATE> {<quint8: player_id> <quint8: score>}[5 times: leader board in descending order]
 * Direction: Server to Client
 */
const packet_t PACKET_LEADERBOARD_UPDATE = 5;
/*
 * Game Tick packet. Informs the client of a server tick and change in board state.
 *
 * Spec: <PACKET_GAME_TICK> <quint32: current_tick> <quint8: direction_moved> <quint8: score>
 *       {<quint32: board_state>}[31 times, the new row visible either L to R or T to B depending on direction]
 *       <quint32: diff_len> {<quint8>}[diff_len times: RLE encoded XOR difference of existing board, L to R, T to B]
 *       <quint64: a hash of the new board state>
 * Direction: Server to Client
 */
const packet_t PACKET_GAME_TICK = 6;
/*
 * Update Direction packet. Informs the server the direction the client wants to move.
 * This may be ignored if invalid.
 *
 * Spec: <PACKET_UPDATE_DIR> <quint8: direction>
 * Direction: Client to Server
 */
const packet_t PACKET_UPDATE_DIR = 7;
/*
 * Resend Request packet. Requests the server resend the local board state in its entirety (probably
 * because of a hash mismatch).
 *
 * Spec: <PACKET_REQUEST_RESEND>
 * Direction: Client to Server
 */
const packet_t PACKET_REQUEST_RESEND = 8;
/*
 * Resend Board packet. Sends the entire local board state after a client request.
 *
 * Spec: <PACKET_RESEND_BOARD> {<quint32: board_state>}[31 * 31 times, populating the local board state, L to R, T to B]
 * Direction: Server to Client
 */
const packet_t PACKET_RESEND_BOARD = 9;
/*
 * End Game packet. Informs the player that their game has ended and provides the final score. Note
 * this does not distinguish between death and victory. The client may either quit or issue a
 * PACKET_REQUEST_JOIN to continue playing.
 *
 * Spec: <PACKET_GAME_END> <quint8: score>
 * Direction: Serber to Client
 */
const packet_t PACKET_GAME_END = 10;

#endif // !PROTOCOL_H
