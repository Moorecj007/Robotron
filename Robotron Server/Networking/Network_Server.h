/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Network_Server.h
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
#include "../../Common Files/Networking/Network_Defines.h"

class CNetwork_Server
{
public:
	// Constructors / Destructors

	/***********************
	* CNetwork_Server: Default Constructor for Server class
	* @author: Callan Moore
	********************/
	CNetwork_Server();

	/***********************
	* ~CNetwork_Server: Default Constructor for Server class
	* @author: Callan Moore
	********************/
	~CNetwork_Server();

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
	* @parameter: _clientAddr: The socket address of the client to send this data packet to
	* @parameter: _pSendPacket: Data Packet structure to Send
	* @return: bool: Successful Sending of the packet (or Not)
	********************/
	bool SendPacket(sockaddr_in _clientAddr, ServerToClient* _pSendPacket);
	
	/***********************
	* ReceivePacket: Receive a Data Packet from the client for processing
	* @author: Callan Moore
	* @parameter: _pReceivePacket: Data Packet structure to receive/store incoming data packets
	* @return: bool: Successful if an actual Packet was received
	********************/
	bool ReceivePacket(ClientToServer* _pReceivePacket);
	
	/***********************
	* AddClientAddr: Add a Client address to the networks map
	* @author: Callan Moore
	* @parameter: _strUser: The username of the client to add
	* @parameter: _clientAddr: The socket address of the client to add
	* @return: void
	********************/
	void AddClientAddr(std::string _strUser, sockaddr_in _clientAddr);

	/***********************
	* RemoveClientAddr: remove a Client address to the networks map
	* @author: Callan Moore
	* @parameter: _strUser: The username of the client to remove
	* @return: void
	********************/
	void RemoveClientAddr(std::string _strUser);

private:
	//Disallowing copies and extra constructions
	CNetwork_Server(const CNetwork_Server& _kr);
	CNetwork_Server& operator= (const CNetwork_Server& _kr);

private:
	SOCKET m_ServerSocket;
	sockaddr_in m_ServerAddr;
	sockaddr_in m_ClientAddr; 
	std::map<std::string,  sockaddr_in>* m_pServerUsers;
	sockaddr_in m_FailedClientAddr;
	sockaddr_in m_ClientRequest;

	char* m_cSendData;
	char* m_cReceiveData;
};

#endif // __SERVERNETWORK_H__

