#ifndef INTERFACE_H
#define INTERFACE_H
bool packet_send(packet_t* _packet);

packet_t* packet_receive(connection_t* _connection);

connection_t* connection_open(char* str);

bool connection_close(connection_t* _connection);
#endif
