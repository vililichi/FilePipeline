#pragma once
#include "CliParam.h"

namespace cli
{
	class CliArg
	{
	public:
		CliArg(std::string _longName, char _shortFlag = '\0', CliParam _paramValue = std::monostate());
		CliArg(char _shortFlag, std::string _longName = std::string(), CliParam _paramValue = std::monostate());
		~CliArg() {}
		const std::string& getLongName() const { return m_longName; }
		char getShortFlag() const { return m_shortFlag; }
		bool isParamEmpty() const;
		std::optional<unsigned short> getUnsignedShort() const;
		std::optional<std::string> getString() const;
		std::optional<std::filesystem::path> getPath() const;
		std::optional<sf::IpAddress> getIpAddress() const;
		std::optional<bool> getBool() const;
		std::string toString() const;

		bool operator<(const CliArg& _other) const;

		bool isSet() const { return m_isSet; }
		void set(bool _isSet) { m_isSet = _isSet; }
		CliParam getParam() const;

		friend std::ostream& operator<<(std::ostream& _stream, const CliArg& _arg);

	private:
		std::string m_longName;
		char m_shortFlag;
		CliParam m_paramValue;
		bool m_isSet;

		std::string compareString() const;
	};
}