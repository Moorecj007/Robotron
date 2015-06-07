/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Server.h
* Description : Server side of the network - responsible for sending and recieving messages to the Client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __SERVERNETWORK_H__
#define __SERVERNETWORK_H__

// Library Includes
#include <map>

// Local Includes
#include "../Common Files/Network_Defines.h"

class CServer
{
public:
	// Constructors / Destructors

	/***********************
	* CServer: Default Constructor for Server class
	* @author: Callan Moore
	********************/
	CServer();

	/***********************
	* ~CServer: Default Constructor for Server class
	* @author: Callan Moore
	********************/
	~CServer();

	// Getters

	// Setters

	// Prototypes

	/***********************
	* Initialise: Initialise the Server Network for use
	* @author: Callan Moore
	* @return: void
	********************/
	bool Initialise();

	/***********************
	* SendPacket: Send a Data Packet to all the Clients for processing
	* @author: Callan Moore
	* @parameter: _pSendPacket: Data Packet structure to Send
	* @return: bool: Successful Sending of the packet (or Not)
	********************/
	bool SendPacket(ServerToClient* _pSendPacket);

	/***********************
	* SendPacket: Send a Data Packet to One Client for processing
	* @author: Callan Moore
	* @parameter: _pSendPacket: Data Packet structure to Send
	* @parameter: _strUserName: The Username of the Client to send this Data packet to
	* @return: bool: Successful Sending of the packet (or Not)
	********************/
	bool SendPacket(std::string _strUserName, ServerToClient* _pSendPacket);
	
	/***********************
	* ReceivePacket: Receive a Data Packet from the client for processing
	* @author: Callan Moore
	* @parameter: _pReceivePacket: Data Packet structure to receive/store incoming data packets
	* @return: bool: Successful if an actual Packet was received
	********************/
	bool ReceivePacket(ClientToServer* _pReceivePacket);

private:
	//Disallowing copies and extra constructions
	CServer(const CServer& _kr);
	CServer& operator= (const CServer& _kr);

private:
	SOCKET m_ServerSocket;
	sockaddr_in m_ServerAddr;
	sockaddr_in m_ClientAddr; 
	std::map < std::string, sockaddr_in>* m_pServerUsers;
	sockaddr_in m_FailedClientAddr;
	sockaddr_in m_ClientRequest;

	char* m_cSendData;
	char* m_cReceiveData;
};

#endif // __SERVERNETWORK_H__

