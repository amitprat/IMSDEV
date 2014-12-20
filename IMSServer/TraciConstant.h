#ifndef TRACICONSTANT_H_
#define TRACICONSTANT_H_



// ****************************************
// COMMANDS
// ****************************************

// command: simulation step
#define CMD_SIMSTEP 0x01

// command: set maximum speed
#define CMD_SETMAXSPEED 0x11

// command: stop node
#define CMD_STOP 0x12

// command: set lane
#define CMD_CHANGELANE 0x13

// command: change route
#define CMD_CHANGEROUTE 0x30

// command: change target
#define CMD_CHANGETARGET 0x31

// command: Simulation Parameter
#define CMD_SIMPARAMETER 0x70

// command: move node
#define CMD_MOVENODE 0x80

// command: close sumo
#define CMD_CLOSE   0x7F

// command: 
#define CMD_UPDATECALIBRATOR 0x50

// command: get all traffic light ids
#define CMD_GETALLTLIDS 0x40

// command: get traffic light status
#define CMD_GETTLSTATUS 0x41

// command: slow down
#define CMD_SLOWDOWN 0x60

// command: report traffic light id
#define CMD_TLIDLIST 0x90

// command: report traffic light status switch
#define CMD_TLSWITCH 0x91

// HBW Application
#define CMD_HBW 0x66

// Heart Beat Message
#define CMD_HBM 0x68

// Eco signal Message
#define CMD_INT 0x69

// Eco Signal Status from TS
#define CMD_INTERSECTION 0x92

// All OK
#define CMD_ACK 0x67

// ****************************************
// POSITION REPRESENTATIONS
// ****************************************

// Omit position
#define POSITION_NONE    0x00
// 2D cartesian coordinates
#define POSITION_2D      0x01
// 2.5D cartesian coordinates
#define POSITION_2_5D    0x02
// 3D cartesian coordinates
#define POSITION_3D      0x03
// Position on road map
#define POSITION_ROADMAP 0x04


// ****************************************
// RESULT TYPES
// ****************************************

// result type: Ok
#define RTYPE_OK    0x00
// result type: not implemented
#define RTYPE_NOTIMPLEMENTED  0x01
// result type: error
#define RTYPE_ERR   0xFF



// ****************************************
// SIMULATION PARAMETERS
// ****************************************
// number of vehicles to use
#define MAX_VEHICLES 50

//DS Commands
#define CMD_DS_INFO 0x20
#define CMD_DS_CONN_SETUP 	0x21 // connection setup notify to TS
#define CMD_DS_CONN_REM 	0x22 //Connection remove notify to TS
#define CMD_DS_NODE_REQ 	0x23 //update req from DS
#define CMD_DS_NOTIFY_REQ 	0x24 //update req from IMS to ds to send periodic update
#define CMD_DS_UPDATE 		0x25
#define CMD_TS_CONN_REM		0x26 // TS Conn Remove notification
#define CMD_NS_CONN_REM		0x27 // NS Conn Remove notification
#define CMD_SIMSTEP_DS 		0x28
#endif /*TRACICONSTANT_H_*/
