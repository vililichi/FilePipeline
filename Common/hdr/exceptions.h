#pragma once
#include <stdexcept>
#include <string>

namespace cli
{
	constexpr int64_t EXCEPTION_EXIT_CODE = -81;
	constexpr int64_t FLAG_EXCEPTION_EXIT_CODE = -82;
	constexpr int64_t PARAMETER_EXCEPTION_EXIT_CODE = -83;

	constexpr int64_t NEVER_HAPPENS_EXIT_CODE = -101;
	
	struct exception : public std::runtime_error
	{
		exception(std::string _msg = "CLI Exception")
			: std::runtime_error(_msg) {}
		virtual int64_t exitCode() { return EXCEPTION_EXIT_CODE; }
	};
	
	struct unknown_flag_exception : public cli::exception
	{
		unknown_flag_exception(std::string _flag)
			: cli::exception("Unknown option: " + _flag) {}
		virtual int64_t exitCode() { return FLAG_EXCEPTION_EXIT_CODE; }
	};

	struct duplicate_flag_exception : public cli::exception
	{
		duplicate_flag_exception(std::string _flag)
			: cli::exception("Duplicate flag: " + _flag) {}
		virtual int64_t exitCode() { return FLAG_EXCEPTION_EXIT_CODE; }
	};

	struct missing_parameter_exception : public cli::exception
	{
		missing_parameter_exception(std::string _flag)
			: cli::exception("Missing parameter for flag: " + _flag) {}
		virtual int64_t exitCode() { return PARAMETER_EXCEPTION_EXIT_CODE; }
	};

	struct conversion_failed : public cli::exception
	{
		conversion_failed(std::string _arg, std::string _msg)
			: cli::exception("Conversion from " + _arg + " to [" + _msg + "] failed.") {}
		virtual int64_t exitCode() { return PARAMETER_EXCEPTION_EXIT_CODE; }
	};

	// This exception signals things that should never happen
	struct never_happens : public cli::exception
	{
		never_happens(std::string _msg)
			: cli::exception("Should never happen:" + _msg) {}
		virtual int64_t exitCode() { return NEVER_HAPPENS_EXIT_CODE; }
	};
}
