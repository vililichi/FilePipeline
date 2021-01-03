#include "CliParam.h"

namespace cli
{
	bool CliParam::isEmpty() const
	{
		return std::holds_alternative<std::monostate>(*this);
	}
	
	std::optional<unsigned short> CliParam::getUnsignedShort() const
	{
		if (isEmpty()
			|| !std::holds_alternative<unsigned short>(*this))
		{
			return {};
		}
		else
		{
			return std::get<unsigned short>(*this);
		}
	}

	std::optional<std::string> CliParam::getString() const
	{
		if (isEmpty()
			|| !std::holds_alternative<std::string>(*this))
		{
			return {};
		}
		else
		{
			return std::get<std::string>(*this);
		}
	}

	std::optional<std::filesystem::path> CliParam::getPath() const
	{
		if (isEmpty()
			|| !std::holds_alternative<std::filesystem::path>(*this))
		{
			return {};
		}
		else
		{
			return std::get<std::filesystem::path>(*this);
		}
	}
	std::optional<sf::IpAddress> CliParam::getIpAddress() const
	{
		if (isEmpty()
			|| !std::holds_alternative<sf::IpAddress>(*this))
		{
			return {};
		}
		else
		{
			return std::get<sf::IpAddress>(*this);
		}
	}

	std::optional<bool> CliParam::getBool() const
	{
		if (isEmpty()
			|| !std::holds_alternative<bool>(*this))
		{
			return {};
		}
		else
		{
			return std::get<bool>(*this);
		}
	}

	std::string CliParam::toString() const
	{
		return std::to_string(*this);
	}
}

std::string std::to_string(bool _bool)
{
	return (_bool) ? "true" : "false";
}

std::string std::to_string(const cli::CliParam& _param)
{
	if (_param.getUnsignedShort().has_value())
	{
		return std::to_string(_param.getUnsignedShort().value());
	}
	else if (_param.getString().has_value())
	{
		return _param.getString().value();
	}
	else if (_param.getPath().has_value())
	{
		return _param.getPath().value().generic_string();
	}
	else if (_param.getIpAddress().has_value())
	{
		return _param.getIpAddress().value().toString();
	}
	else if (_param.getBool().has_value())
	{
		return std::to_string(_param.getBool().value());
	}
	else
	{
		return std::string();
	}
}