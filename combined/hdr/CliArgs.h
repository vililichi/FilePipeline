#pragma once
#include <string>
#include "SFML/Network/IpAddress.hpp"
#include <filesystem>

namespace cli
{
	enum class eUpDown
	{
		UPLOAD = 0,
		DOWNLOAD = 1
	};

	class UpDown
	{
	public:
		UpDown(eUpDown _upDown) { m_upDown = _upDown; }
		~UpDown() {};
		eUpDown getEnumValue() { return m_upDown; }
		std::string getString() { return m_upDown == eUpDown::UPLOAD ? "UPLOAD" : "DOWNLOAD"; }
		bool operator==(eUpDown _upDown) { return m_upDown == _upDown; }
		bool operator!=(eUpDown _upDown) { return !operator==(_upDown); }
		bool operator==(UpDown _upDown) { return m_upDown == _upDown.m_upDown; }
		bool operator!=(UpDown _upDown) { return !operator==(_upDown); }
	private:
		eUpDown m_upDown;
	};

	struct sCliParams
	{
		eUpDown upDown;
		bool isInteractive;
		bool usePreset;
		std::string presetName;
		sf::IpAddress address;
		unsigned short port;
		std::filesystem::path pathToFile;
	};
}