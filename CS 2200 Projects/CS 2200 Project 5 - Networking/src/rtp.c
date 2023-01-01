#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "queue.h"
#include "network.h"
#include "rtp.h"

typedef struct message {
    char *buffer;
    int length;
} message_t;

//ERIC CREATED THIS MACRO
#define IS_ODD(x) ((1 & ((size_t) (x))) == 1) //bitwise AND of address and 1

//ERIC CREATED THIS ENUm
enum {
    SENT = 10,
    NOT_SENT
};

/* ================================================================ */
/*                  H E L P E R    F U N C T I O N S                */
/* ================================================================ */

/**
 * --------------------------------- PROBLEM 1 --------------------------------------
 * 
 * Convert the given buffer into an array of PACKETs and returns the array.  The 
 * value of (*count) should be updated so that it contains the number of packets in 
 * the array
 * 
 * @param buffer pointer to message buffer to be broken up packets
 * @param length length of the message buffer.
 * @param count pointer to the number of packets in the returning array
 * 
 * @returns array of packets
 * TODO 1: implement the packetize function
 */
packet_t *packetize(char *buffer, int length, int *count) {
    packet_t *packets;

    /* ----  START OF STUDENT CODE ---- */
    
    /* 0. SETUP*/
    //Find out how many packets we need to split the buffer into
    int numPacketsNeeded = length / MAX_PAYLOAD_LENGTH + 1;
    //edge case if packet size evenly divides into max payload size (consider max size of 7...we only want 1 packet  
    // if the length is 6, still only packet if the size is 7, but 2 if the size is 8).
    if (length % MAX_PAYLOAD_LENGTH == 0) {
        numPacketsNeeded -= 1;
    }

    // 1. Allocate an array of packets big enough to carry all the data.
    //    The provided code in the send thread function will free this array once it is done.
    packets = calloc((size_t) numPacketsNeeded, sizeof(packet_t));

    /* 2. Populate all the fields of the packet including the payload. Remember, The last packet should be
     *    a LAST_DATA packet. All other packets should be DATA packets. THIS IS IMPORTANT. The server
     *    checks for this, and it will disconnect you if they are not filled in correctly. If you neglect
     *    the LAST_DATA
     */
    char *block_of_buffer_to_packetize;
    const int size_of_last_packet = length % MAX_PAYLOAD_LENGTH;
    for (int i = 0; i < numPacketsNeeded; i++) {
        block_of_buffer_to_packetize = buffer + (i * MAX_PAYLOAD_LENGTH);

        if (i < numPacketsNeeded - 1) {
            packets[i].type = DATA;
            packets[i].payload_length = MAX_PAYLOAD_LENGTH;
        } else {
            packets[i].type = LAST_DATA; //no matter the edge case, this is the last packet
            if (size_of_last_packet == 0) { //if the last payload size evenly divides into a packet
                packets[i].payload_length = MAX_PAYLOAD_LENGTH;
            } else { //there is internal fragmentation within the last packet (allocated space that is wasted)
                packets[i].payload_length = size_of_last_packet;
            }
        }
        memcpy(packets[i].payload, block_of_buffer_to_packetize, (size_t) packets[i].payload_length);
        packets[i].checksum = checksum(packets[i].payload, packets[i].payload_length);
    }
    
    //create the last packet


    

    /* 3. The count variable points to an integer. Update this integer setting it equal to the length
     *    of the array you are returning.
     */
    *count = numPacketsNeeded;

    /* ----   END OF STUDENT CODE  ---- */

    //4. Return the array of packets
    return packets;
}

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * 
 * Compute a checksum based on the data in the buffer.
 * 
 * Checksum calculation: Sum the ASCII values of all characters in the buffer, if the
 * index is even, multiply the ASCII value by the index, and return the total. 
 * 
 * Example: "abcd" checksum = (0 * a) + (b) + (2 * c) + (d)
 * 
 * @param buffer pointer to the char buffer that the checksum is calculated from
 * @param length length of the buffer
 * 
 * @returns calculated checksum
 * TODO 2.1.1
 */
int checksum(char *buffer, int length) {
    /* ----  START OF STUDENT CODE ---- */
    int checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum += IS_ODD(i) ? //if the memory location is odd
                    buffer[i] :          //if odd, then just add the ASCII value of the buffer at this index
                    i * buffer[i];       //if even, add the ASCII value multiplied by the index
    }

    return checksum;
    /* ----   END OF STUDENT CODE  ---- */
}


/* ================================================================ */
/*                      R T P       T H R E A D S                   */
/* ================================================================ */

static void *rtp_recv_thread(void *void_ptr) {

    rtp_connection_t *connection = (rtp_connection_t *) void_ptr;

    do {
        message_t *message;
        int buffer_length = 0;
        char *buffer = NULL;
        packet_t packet;
        packet_t *acknowledgement_packet;

        /* Put messages in buffer until the last packet is received  */
        do {
            if (net_recv_packet(connection->net_connection_handle, &packet) <= 0 || packet.type == TERM) {
                /* remote side has disconnected */
                connection->alive = 0;
                pthread_cond_signal(&connection->recv_cond);
                pthread_cond_signal(&connection->send_cond);
                break;
            }

            /* TODO 2.2.1
             *
             * 1. check to make sure payload of packet is correct
             * 2. send an ACK or a NACK, whichever is appropriate
             * 3. if this is the last packet in a sequence of packets
             *    and the payload was corrupted, make sure the loop
             *    does not terminate
             * 4. if the payload matches, add the payload to the buffer
             */
            if (packet.type == DATA || packet.type == LAST_DATA) {
                acknowledgement_packet = calloc(1, sizeof(packet_t));
                if (packet.checksum == checksum(packet.payload, packet.payload_length)) { //if successful, send ACK
                    acknowledgement_packet->type = ACK; //send ACK

                    //Add the payload to the buffer
                    buffer = realloc(buffer, (size_t) (buffer_length + packet.payload_length)); //expand the buffer
                    memcpy(buffer + buffer_length, packet.payload, (size_t) packet.payload_length);
                    buffer_length += packet.payload_length;
                } else { //unsuccessful transmission, send NACK and continue
                    acknowledgement_packet->type = NACK; //send NACK

                    if (packet.type == LAST_DATA) {
//                        continue; //make sure to continue listening for more packets (since the corrected ones will be sent)
                        packet.type = DATA;
                    }
                }
                net_send_packet(connection->net_connection_handle, acknowledgement_packet);
                free(acknowledgement_packet);
            }


            /* TODO 2.2.2
             *
             *  What if the packet received is not a data packet?
             *  If it is a NACK or an ACK, the sending thread should
             *  be notified so that it can finish sending the message.
             *
             *  1. add the necessary fields to the CONNECTION data structure
             *     in rtp.h so that the sending thread has a way to determine
             *     whether a NACK or an ACK was received
             *  2. signal the sending thread that an ACK or a NACK has been
             *     received.
             */
            if (packet.type == ACK || packet.type == NACK) {
                pthread_mutex_lock(&(connection->ack_mutex));

                //1.) update acknowledgement_type
                if (packet.type == ACK) {
                    connection->acknowledgement_type = ACK;
                } else {
                    connection->acknowledgement_type = NACK;
                }

                //2.) signal to the sending thread that an ACK or a NACK has been sent
                connection->message_status = SENT;
                pthread_cond_signal(&(connection->ack_cond));
                pthread_mutex_unlock(&(connection->ack_mutex));
            }


        } while (packet.type != LAST_DATA);





        /* TODO 2.3
         *
         * Now that an entire message has been received, we need to
         * add it to the queue to provide to the rtp client.
         *
         * 1. Add message to the received queue.
         * 2. Signal the client thread that a message has been received.
         */
        if (connection->alive == 1) {
            //1.) assemble the message
            message = calloc(1, sizeof(message_t));
            message->buffer = calloc((size_t) buffer_length, sizeof(char));
            message->length = buffer_length;
            memcpy(message->buffer, buffer, (size_t) message->length);

            //2.) send the message to the "send_queue" and signal to client thread to read message
            pthread_mutex_lock(&(connection->recv_mutex));
            queue_add(&connection->recv_queue, message);
            pthread_cond_signal(&(connection->recv_cond));
            pthread_mutex_unlock(&(connection->recv_mutex));
        }
        free(buffer); //free buffer regardless
    } while (connection->alive == 1);

    return NULL;
}

static void *rtp_send_thread(void *void_ptr) {

    rtp_connection_t *connection = (rtp_connection_t *) void_ptr;
    message_t *message;
    int array_length = 0;
    int i;
    packet_t *packet_array;

    do {
        /* Extract the next message from the send queue */
        pthread_mutex_lock(&connection->send_mutex);
        while (queue_size(&connection->send_queue) == 0 && connection->alive == 1) {
            pthread_cond_wait(&connection->send_cond, &connection->send_mutex);
        }

        if (connection->alive == 0) break;

        message = queue_extract(&connection->send_queue);

        pthread_mutex_unlock(&connection->send_mutex);

        /* Packetize the message and send it */
        packet_array = packetize(message->buffer, message->length, &array_length);

        for (i = 0; i < array_length; i++) {

            /* Start sending the packetized messages */
            if (net_send_packet(connection->net_connection_handle, &packet_array[i]) <= 0) {
                /* remote side has disconnected */
                connection->alive = 0;
                break;
            }

            /* TODO 2.4
             * 
             *  1. wait for the recv thread to notify you of when a NACK or
             *     an ACK has been received
             *  2. check the data structure for this connection to determine
             *     if an ACK or NACK was received.  (You'll have to add the
             *     necessary fields yourself)
             *  3. If it was an ACK, continue sending the packets.
             *  4. If it was a NACK, resend the last packet
             */
            pthread_mutex_lock(&(connection->ack_mutex));
            while (connection->message_status != SENT) { //1.) wait for message
                pthread_cond_wait(&(connection->ack_cond), &(connection->ack_mutex));
            }
            connection->message_status = NOT_SENT; //reset message status
            if (connection->acknowledgement_type == NACK) { //2.) check which ack_packet was sent
                i--; //4.) decrement i so the loop resends the last packet
            } //3.) else if ACK do nothing
            pthread_mutex_unlock(&(connection->ack_mutex));
        }

        free(packet_array);
        free(message->buffer);
        free(message);
    } while (connection->alive == 1);
    return NULL;


}

static rtp_connection_t *rtp_init_connection(int net_connection_handle) {
    rtp_connection_t *rtp_connection = malloc(sizeof(rtp_connection_t));

    if (rtp_connection == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }

    rtp_connection->net_connection_handle = net_connection_handle;

    queue_init(&rtp_connection->recv_queue);
    queue_init(&rtp_connection->send_queue);

    pthread_mutex_init(&rtp_connection->ack_mutex, NULL);
    pthread_mutex_init(&rtp_connection->recv_mutex, NULL);
    pthread_mutex_init(&rtp_connection->send_mutex, NULL);
    pthread_cond_init(&rtp_connection->ack_cond, NULL);
    pthread_cond_init(&rtp_connection->recv_cond, NULL);
    pthread_cond_init(&rtp_connection->send_cond, NULL);

    rtp_connection->alive = 1;

    pthread_create(&rtp_connection->recv_thread, NULL, rtp_recv_thread,
                   (void *) rtp_connection);
    pthread_create(&rtp_connection->send_thread, NULL, rtp_send_thread,
                   (void *) rtp_connection);

    return rtp_connection;
}

/* ================================================================ */
/*                           R T P    A P I                         */
/* ================================================================ */

rtp_connection_t *rtp_connect(char *host, int port) {

    int net_connection_handle;

    if ((net_connection_handle = net_connect(host, port)) < 1)
        return NULL;

    return (rtp_init_connection(net_connection_handle));
}

int rtp_disconnect(rtp_connection_t *connection) {

    message_t *message;
    packet_t term;

    term.type = TERM;
    term.payload_length = term.checksum = 0;
    net_send_packet(connection->net_connection_handle, &term);
    connection->alive = 0;

    net_disconnect(connection->net_connection_handle);
    pthread_cond_signal(&connection->send_cond);
    pthread_cond_signal(&connection->recv_cond);
    pthread_join(connection->send_thread, NULL);
    pthread_join(connection->recv_thread, NULL);
    net_release(connection->net_connection_handle);

    /* emtpy recv queue and free allocated memory */
    while ((message = queue_extract(&connection->recv_queue)) != NULL) {
        free(message->buffer);
        free(message);
    }
    queue_release(&connection->recv_queue);

    /* emtpy send queue and free allocated memory */
    while ((message = queue_extract(&connection->send_queue)) != NULL) {
        free(message);
    }
    queue_release(&connection->send_queue);

    free(connection);

    return 1;

}

int rtp_recv_message(rtp_connection_t *connection, char **buffer, int *length) {

    message_t *message;

    if (connection->alive == 0)
        return -1;
    /* lock */
    pthread_mutex_lock(&connection->recv_mutex);
    while (queue_size(&connection->recv_queue) == 0 && connection->alive == 1) {
        pthread_cond_wait(&connection->recv_cond, &connection->recv_mutex);
    }

    if (connection->alive == 0) {
        pthread_mutex_unlock(&connection->recv_mutex);
        return -1;
    }

    /* extract */
    message = queue_extract(&connection->recv_queue);
    *buffer = message->buffer;
    *length = message->length;
    free(message);

    /* unlock */
    pthread_mutex_unlock(&connection->recv_mutex);

    return *length;
}

int rtp_send_message(rtp_connection_t *connection, char *buffer, int length) {

    message_t *message;

    if (connection->alive == 0)
        return -1;

    message = malloc(sizeof(message_t));
    if (message == NULL) {
        return -1;
    }
    message->buffer = malloc((size_t) length);
    message->length = length;

    if (message->buffer == NULL) {
        free(message);
        return -1;
    }

    memcpy(message->buffer, buffer, (size_t) length);

    /* lock */
    pthread_mutex_lock(&connection->send_mutex);

    /* add */
    queue_add(&(connection->send_queue), message);

    /* unlock */
    pthread_mutex_unlock(&connection->send_mutex);
    pthread_cond_signal(&connection->send_cond);
    return 1;

}
