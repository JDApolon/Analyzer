#include "pch.h"
#include <fstream>
#include <iostream>
#include <bitset>

using namespace std;
int FindSyncByte(ifstream&,  char*);
int OpenFile(ifstream&);
int ProcessPacket(ifstream&);
bool ValidateSyncByte(char);
int ParseNextPacket(ifstream&, char*);
void Parse_TS_Header(ifstream&, char*);

/// All the flags and data for the the TS Header
struct ts_header
{
	char sync_byte;
	bool transport_error_indicator;
	bool payload_unit_start_indicator;
	bool transport_priority;
	unsigned int PID;
	unsigned char ts_scrambling_control;
	unsigned char adaptation_field_ctrl;
	unsigned char transport_scrambling_control;
	unsigned char adaptation_field_control;
	unsigned char continuity_counter;
};



int main()
{
	ifstream Test; 
	OpenFile(Test);
    

	ProcessPacket(Test);

     
  
	
	
   

	Test.close();
	system("pause");

	return 0;
}

/**
 Starts from the beginning and the first 7 0x47s in the file.
 It then returns the address of the first 0x47 in the file.
*/
int FindSyncByte(ifstream& File,  char *Packet)
{

	char Byte; ///The buffer used in searching for the 0x47s
	int Start, Size, NumFound = 0;
	int *ReadData;

	while (File >> hex >> Byte && NumFound < 7)  ///Keeps going through the file either until the file ends, of 7 0x47s are found
	{

		if (static_cast<int>(Byte) == 0x47)
		{
			if (NumFound == 0)
			{
				Start = File.tellg();
				Start -= 1;
			}				

			File.seekg(187, ios::cur);
			NumFound++;

		}
		

	}

	return Start;

}


/**
Opens the file, and checks to see if the file opens successfully or not.
*/
int OpenFile(ifstream& File)
{
	File.open("C:\\Reference_Streams\\bbb_360p_c.ts", ios::binary);
	if (!File)
	{
		cout << "File could not be opened. Try again." << endl;
		system("pause");
		return 1;
	}
	return 0;
}

/**
Finds the sync byte, goes to the address of the sync byte, and then reads the first byte. 
It then reads in the following packet(validation is handled in that function).
*/
int ProcessPacket(ifstream& File) 
{
	char Packet[188];
	int FoundSync;

	FoundSync = FindSyncByte(File, Packet) ; ///Find the sync byte

	File.seekg(FoundSync , ios::beg);        ///Go to the address of the sync byte
	
	File.read(&Packet[0], 1);                ///Read the first byte of the packet

	ParseNextPacket(File, Packet);           ///Checks if the first byte is 0x47 and then parses the packet

	File.read(&Packet[0], 1);

	ParseNextPacket(File, Packet);

	return 0;
}
/**
Checks to see if the inputted value is equal to 0x47.
In other words, it validates the Sync Byte.
*/
bool ValidateSyncByte(char SyncByte)
{
	if (static_cast<int>(SyncByte) == 0x47)
	{
		return true;
	}
	return false;
}

/**
Parses the first three bytes of the packet, the header, and extracts all the
information found in the header.
*/
void Parse_TS_Header(ifstream& File, char* Packet)
{
	ts_header packet_header;

	packet_header.sync_byte = Packet[0];
	packet_header.transport_error_indicator    = (Packet[1] & 0x80) >> 7;
	packet_header.payload_unit_start_indicator = (Packet[1] & 0x40) >> 6;
	packet_header.transport_priority		   = (Packet[1] & 0x20) >> 5;
	packet_header.PID						   = ((Packet[1] & 31) << 8) | Packet[2];
	packet_header.transport_scrambling_control = (Packet[3] & 0xC0);
	packet_header.adaptation_field_control     = (Packet[3] & 0x30) >> 4;
	packet_header.continuity_counter           = (Packet[3] & 0xF);
	cout << hex << packet_header.PID << endl;
}

/**
Takes in the buffer, of which the first byte is filled, and checks if the first byte is equal to 0x47.
If it is, it proceeds to populate the array with the rest of the packet.
After that, it parses the header of the packet, and then outputs the contents of the header in bindary.
*/
int ParseNextPacket(ifstream& File, char* Packet)
{
	if (ValidateSyncByte(Packet[0]))
	{
		File.read(&Packet[1], 187);         ///Populate the array with the contents of the packet
		Parse_TS_Header(File, Packet);      ///Parse the header
		cout << bitset<8>(Packet[1]) << "  " << bitset<8>(Packet[2]) << "  " << bitset<8>(Packet[3]) << endl;      ///Output the contents of the header in binary/
	}
	else
	{
		cout << "Sync Byte not valid" << endl;
		return 1;
	}
	return 0;
}