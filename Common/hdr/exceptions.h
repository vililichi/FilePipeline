#include <stdexcept>
#include <string>

namespace cli
{
	struct exception : public std::runtime_error
	{
		exception(std::string _msg = "CLI Exception")
			: std::runtime_error(_msg) {}
	};
	
	struct unknown_flag_exception : public cli::exception
	{
		unknown_flag_exception(std::string _flag)
			: cli::exception("Unknown flag: " + _flag) {}
	};
}
