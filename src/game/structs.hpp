#pragma once

namespace game
{
	enum errorParm
	{
		ERR_FATAL,                  // exit the entire game with a popup window
		ERR_DROP,                   // print to console and disconnect from game
		ERR_SERVERDISCONNECT,       // don't kill server
		ERR_DISCONNECT,             // client disconnected from the server
		ERR_NEED_CD                 // pop up the need-cd dialog
	};

	enum dvar_flag
	{
		DVAR_FLAG_NONE = 0x0,			//no flags
		DVAR_FLAG_SAVED = 0x1,			//saves in config_mp.cfg for clients
		DVAR_FLAG_LATCHED = 0x2,			//no changing apart from initial value (although it might apply on a map reload, I think)
		DVAR_FLAG_CHEAT = 0x4,			//cheat
		DVAR_FLAG_REPLICATED = 0x8,			//on change, this is sent to all clients (if you are host)
		DVAR_FLAG_UNKNOWN10 = 0x10,			//unknown
		DVAR_FLAG_UNKNOWN20 = 0x20,			//unknown
		DVAR_FLAG_UNKNOWN40 = 0x40,			//unknown
		DVAR_FLAG_SERVER = 0x80,			//server dvar
		DVAR_FLAG_USERCREATED = 0x100,		//a 'set' type command created it
		DVAR_FLAG_USERINFO = 0x200,		//userinfo?
		DVAR_FLAG_SERVERINFO = 0x400,		//in the getstatus oob
		DVAR_FLAG_WRITEPROTECTED = 0x800,		//write protected
		DVAR_FLAG_UNKNOWN1000 = 0x1000,		//unknown
		DVAR_FLAG_READONLY = 0x2000,		//read only (same as 0x800?)
		DVAR_FLAG_UNKNOWN4000 = 0x4000,		//unknown
		DVAR_FLAG_UNKNOWN8000 = 0x8000,		//unknown
		DVAR_FLAG_UNKNOWN10000 = 0x10000,		//unknown
		DVAR_FLAG_DEDISAVED = 0x1000000,		//unknown
		DVAR_FLAG_NONEXISTENT = 0xFFFFFFFF	//no such dvar
	};

	struct CmdArgs
	{
		int nesting;
		int localClientNum[8];
		int argc[8];
		const char** argv[8];
	};

	struct cmd_function
	{
		cmd_function* next;
		const char* name;
		const char* autoCompleteDir;
		const char* autoCompleteExt;
		void(__cdecl* function)();
		int flags;
	};

}
