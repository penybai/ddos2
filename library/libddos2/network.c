//
//  network.c
//  ddos2
//
//  Created by Andre Zay on 20/01/2020.
//  Copyright © 2020 Andre Zay. All rights reserved.
//

#include "network.h"
#include "arguments.h"
#include "message.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

hashtable* network_ifaces=NULL;
bool stats=true;

void network_begin(hashtable* ifaces, bool _stats){
    network_ifaces=ifaces;
    stats=_stats;
}

void network_set_stats(bool stat){//Sets wheteher we need to collect interface statisitcs
    stats=stat;
}

iface_t* network_iface(char* name){
    iface_t* iface=(iface_t*)malloc(sizeof(iface_t));
    iface->name=(char*)malloc(sizeof(char)*strlen(name)+sizeof(char));
    strcpy(iface->name, name);
    iface->bytes_sent=0;
    iface->packets_sent=0;
    iface->connection_close=NULL;
    iface->connection_open=NULL;
    iface->packet_send=NULL;
    iface->packet_receive=NULL;
    return iface;
}
void register_iface(iface_t* iface){
    if(!network_ifaces){
        error("Programming error: network_ifaces==NULL! Have you called network_begin()?");
        return ;
    }
    hashtbl_add(network_ifaces, iface->name, iface);
    debug("Registered network interface: %s",iface->name);
}

connection_t* connection_open(iface_t* iface, char* target){
    if(!iface){
        error("Programming error: %s: Bad argument: iface(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return NULL;
    }
    if(!iface->connection_open){
        error("Programming error: Interface %s does not support openning connections.",iface->name);
        return NULL;
    }
    connection_t* connection=iface->connection_open(target);
    return connection;
}

bool connection_close(connection_t* connection){
    if(!connection){
        error("Programming error: %s: Bad argument: connection(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return false;
    }
    bool result = connection->iface->connection_close(connection);
    free(connection);
    return result;
}

bool _packet_send(packet_t* packet){
    if(!packet){
        error("Programming error: %s: Bad argument: packet is NULL(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return false;
    }
    if(!packet->iface){
        error("Programming error: %s: Bad argument: packet->iface is NULL(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return false;
    }
    if(!packet->iface->packet_send){
        error("Programming error: %s: Not supported: Interface %s does not support packet_send(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return false;
    }
    if(stats){
        packet->iface->bytes_sent+=packet->sz;
        packet->iface->packets_sent++;
    }
    return packet->iface->packet_send(packet);
}

bool packet_send(iface_t* iface, packet_t* packet){
    if(!packet){
        error("Programming error: %s: Bad argument: packet is NULL(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return false;
    }
    packet->iface=iface;
    return _packet_send(packet);
}

packet_t* packet_receive(connection_t* connection){
    if(!connection){
        error("Programming error: %s: Bad argument: connection is NULL(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return NULL;
    }
    if(!connection->iface){
        error("Programming error: %s: Bad argument: connection is NULL(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return NULL;
    }
    if(!connection->iface->packet_receive){
        error("Programming error: Interface %s does not support openning connections.",connection->iface->name);
        return NULL;
    }
    
    return connection->iface->packet_receive(connection);
}

void packet_set_option(packet_t* packet, char* name, void* value){
    if(packet->options){
        error("Programming error: %s: Bad argument: packet->options is NULL(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        return ;
    }
    if(hashtbl_check_key(packet->options, name)){
        hashtbl_set(packet->options, name, value);
        return ;
    }
    hashtbl_add(packet->options, name, value);
}
