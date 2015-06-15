/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Client.h
* Description : Client side of the network - responsible for sending and recieving messages to the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __CLIENTNETWORK_H__
#define __CLIENTNETWORK_H__

// Local Includes
#include "../../Common Files/Networking/Network_Defines.h"
#include <map>


class CClient
{
public:
	// Constructors / Destructors

	/***********************
	* CClient: Default Constructor for Client class
	* @author: Callan Moore
	********************/
	CClient();

	/***********************
	* ~CClient: Default Destructor for Client class
	* @author: Callan Moore
	********************/
	~CClient();

	// Getters

	// Setters

	// Prototypes

	/***********************
	* Initialise: Initialise the Client Network for use
	* @author: Callan Moore
	* @return: void
	********************/
	bool Initialise();

	/***********************
	* SendPacket: Send a Data Packet to the server for processing
	* @author: Callan Moore
	* @parameter: _pSendPacket: Data Packet structure to Send
	* @return: bool: Successful Sending of the packet (or Not)
	********************/
	bool SendPacket(ClientToServer* _pSendPacket);

	/***********************
	* BroadcastToServers: Broadcast Packet to all servers
	* @author: Callan Moore
	* @parameter: _pSendPacket: Data Packet structure to broadcast
	* @return: bool: Successful broadcast of the packet (or Not)
	********************/
	bool BroadcastToServers(ClientToServer* _pSendPacket);

	/***********************
	* ReceivePacket: Receive a Data Packet from the server for processing
	* @author: Callan Moore
	* @parameter: _pReceivePacket: Data Packet structure to receive/store incoming data packets
	* @return: bool: Successful if an actual Packet was received
	********************/
	bool ReceivePacket(ServerToClient* _pReceivePacket);

	/***********************
	* SelectServer: Set the server on the network to the chosen server
	* @author: Callan Moore
	* @parameter: _ServerAddr: A server Address
	* @return: bool: Successful if an actual Packet was received
	********************/
	void SelectServer(sockaddr_in _ServerAddr);

	/***********************
	* Reset: Reset values set for a specific server
	* @author: Callan Moore
	* @return: void
	********************/
	void Reset();


private:
	//Disallowing copies and extra constructions
	CClient(const CClient& _kr);
	CClient& operator= (const CClient& _kr);

private:
	SOCKET m_ClientSocket;
	sockaddr_in m_ClientAddr;
	sockaddr_in m_ReceivedAddr;
	char* m_cReceiveData;
	char* m_cSendData;

	sockaddr_in m_ServerAddr;
};
#endif //__CLIENTNETWORK_H__
