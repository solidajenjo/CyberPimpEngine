#ifndef _FILE_EXPLORER_H
#define _FILE_EXPLORER_H

#include <string>
#include <stack>

class FileExplorer
{
public:
	
	void Reset(); //set up a new file dialog request
	bool Open(); //returns true on accepted, false on cancelled or running

	//members

	bool opened = false;
	char title[100] = "File Explorer";
	char filename[1024] = "";
	std::string path = "";
	std::stack<std::string> pathStack;
};

#endif