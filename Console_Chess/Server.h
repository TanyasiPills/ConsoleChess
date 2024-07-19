#pragma once
#ifndef SERVER_H
#define SERVER_H

int initalize_server();

extern SOCKET clientSock;

void message();
void getmessage();
#endif