#include "CliArg.h"
#include "exceptions.h"
#include <assert.h>

namespace cli
{
	CliArg::CliArg(std::string _longName, char _shortFlag,
		CliParam _paramValue)
	{
		const bool isValid = !_longName.empty() || std::isalnum(_shortFlag);
		assert(isValid);

		m_longName = _longName;
		m_shortFlag = _shortFlag;
		m_paramValue = _paramValue;
		m_isSet = false;
	}
	CliArg::CliArg(char _shortFlag, std::string _longName,
		CliParam _paramValue)
		: CliArg(_longName, _shortFlag, _paramValue)
	{}

	CliParam CliArg::getParam() const
	{
		return m_paramValue;
	}

	bool CliArg::isParamEmpty() const
	{
		return m_paramValue.isEmpty();
	}

	std::optional<unsigned short> CliArg::getUnsignedShort() const
	{
		return m_paramValue.getUnsignedShort();
	}

	std::optional<std::string> CliArg::getString() const
	{
		return m_paramValue.getString();
	}

	std::optional<std::filesystem::path> CliArg::getPath() const
	{
		return m_paramValue.getPath();
	}
	std::optional<sf::IpAddress> CliArg::getIpAddress() const
	{
		return m_paramValue.getIpAddress();
	}

	std::optional<bool> CliArg::getBool() const
	{
		return m_paramValue.getBool();
	}

	std::string CliArg::toString() const
	{
		std::string out;
		bool emptyParam = isParamEmpty();
		if (!m_longName.empty())
		{
			out.assign("--").append(m_longName);
			if (!emptyParam)
			{
				out.append("=").append(m_paramValue.toString());
			}
		}
		else if (m_shortFlag != '\0')
		{
			out.assign("-").append(std::string(m_shortFlag, 1));
			if (!emptyParam)
			{
				out.append(" ").append(m_paramValue.toString());
			}
		}
		return std::move(out);
	}

	std::string CliArg::compareString() const
	{
		if (!m_longName.empty())
		{
			return m_longName;
		}
		else
		{
			return std::string(m_shortFlag, 1);
		}
	}

	bool CliArg::operator<(const CliArg& _other) const
	{
		return compareString() < _other.compareString();
	}

	std::ostream& operator<<(std::ostream& _stream, const CliArg& _arg)
	{
		_stream << _arg.toString();
		return _stream;
	}
}