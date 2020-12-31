#pragma once
#include "CliArgs.h"

#include <string>
#include <vector>

/*###########################################################################################################################################################################################
	ARGUMENTS
		upload (-U)          -> Upload a file, use with --path
		download (-D)        -> Download a file, use with --name
		test (-T)            -> Checks if a file is available, use with and IP and a port, or with a --target (use with --name for a specific file)
		add-file             -> Adds a file to the upload list(used with --name and --path)
		remove-file          -> Removes a file from the upload list (used with --name or --path)
		list                 -> Lists the files in the upload list. If used with an IP and a port or with a --target, lists the files available for download from there.

		--target (-t)        -> Uses a preset from the config file for the IP and the port, requires a name as argument
		--name (-n)          -> Sets the name for an file, takes a name as argument
		--path (-p)          -> Sets the path of a file, takes a path as argument
		--ip4 (-4)           -> IPv4 used by the uploader (used with --download)
		--ip6 (-6)           -> IPv6 used by the uploader (used with --download)
		--port (-P)          -> Port used by the uploader (used with --download) OR Port used to upload (used with --upload)
		--config (-C)        -> Uses a specific config file, takes a path as argument
		--daemon (-d)        -> Starts the upload server as a daemon (as a background process)

		--can-upload         -> Used with "test" to check if the target can receive files or only provide them

		--i (-i)             -> Interactive mode (can't be used with --daemon)
		--help (-h)          -> Shows this help message
		--verbose (-v)       -> Enables output of debugging informations
		--version            -> Shows the version and exits
###########################################################################################################################################################################################*/


class CliParser
{
public:
	CliParser(int argc, char* argv[]);
	~CliParser() {}

	cli::sCliParams GetParams() { return cli::sCliParams(); }
	std::vector<std::string>& getVector() { return m_arguments; }
private:
	std::vector<std::string> m_arguments;
};