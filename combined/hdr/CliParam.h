#pragma once

#include "SFML/Network/IpAddress.hpp"

#include <string>
#include <filesystem>
#include <variant>
#include <optional>

namespace cli
{
	typedef std::variant<std::monostate, bool, std::string, std::filesystem::path, unsigned short, sf::IpAddress> variantParam;
	class CliParam : public variantParam
	{
	public:
		CliParam() : variantParam() {}
		CliParam(std::monostate _arg) : variantParam(_arg) {}
		CliParam(bool _arg) : variantParam(_arg) {}
		CliParam(std::string _arg) : variantParam(_arg) {}
		CliParam(std::filesystem::path _arg) : variantParam(_arg) {}
		CliParam(unsigned short _arg) : variantParam(_arg) {}
		CliParam(sf::IpAddress _arg) : variantParam(_arg) {}

		bool isEmpty() const;
		std::optional<unsigned short> getUnsignedShort() const;
		std::optional<std::string> getString() const;
		std::optional<std::filesystem::path> getPath() const;
		std::optional<sf::IpAddress> getIpAddress() const;
		std::optional<bool> getBool() const;
		std::string toString() const;
	};
}
namespace std
{
	std::string to_string(bool _bool);
	std::string to_string(const cli::CliParam& _param);
}