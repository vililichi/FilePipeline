#pragma once
namespace command
{

	enum Comm : uint8_t
	{
		list = 0,
		download = 1,
		upload = 2,
		rename = 3,
		remove = 4
	};

}