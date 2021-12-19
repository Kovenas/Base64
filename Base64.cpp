#include "Base64.hpp"

#include <array>

constexpr auto EncodingTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr auto PaddingSymbol = '=';

std::string Base64::Encode(std::string_view BufferToEncode)
{
	const size_t BufferSize = BufferToEncode.size();
	const size_t SizeMod = BufferSize % 3;
	const size_t OutputSize = (BufferSize / 3) * 4 + (SizeMod ? 4 : 0);

	std::string OutputBuffer(OutputSize, 0);
	auto OutputBufferIt = reinterpret_cast<uint8_t*>(OutputBuffer.data());

	const auto LoopBegin = reinterpret_cast<const uint8_t*>(BufferToEncode.data());
	const auto LoopEnd = reinterpret_cast<const uint8_t*>(BufferToEncode.data()) + (BufferSize - SizeMod);

	auto It = LoopBegin;
	for (; It < LoopEnd; ++It)
	{
		uint8_t Index = *It >> 2;
		uint8_t Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (*It & 0x03) << 4;
		Index |= *++It >> 4;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (*It & 0x0F) << 2;
		Index |= *++It >> 6;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = *It & 0x3F;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;
	}

	if (SizeMod == 1)
	{
		uint8_t Index = *It >> 2;
		uint8_t Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (*It & 0x03) << 4;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		*OutputBufferIt++ = PaddingSymbol;
		*OutputBufferIt++ = PaddingSymbol;
	}
	else if (SizeMod == 2)
	{
		uint8_t Index = *It >> 2;
		uint8_t Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (*It & 0x03) << 4;
		Index |= *++It >> 4;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (*It & 0x0F) << 2;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		*OutputBufferIt++ = PaddingSymbol;
	}

	return OutputBuffer;
}

constexpr auto GetIndexTable()
{
	std::array<uint8_t, 256> Table{ 0 };

	for (size_t i = 0; i < 64; ++i)
	{
		auto Character = static_cast<uint8_t>(EncodingTable[i]);
		Table[Character] = static_cast<uint8_t>(i);
	}

	return Table;
}

constexpr auto DecodingTable = GetIndexTable();

std::string Base64::Decode(std::string_view BufferToDecode)
{
	const size_t BufferSize = BufferToDecode.size();
	const size_t SizeMod = BufferSize % 4;
	const size_t OutputSize = (BufferSize / 4) * 3 + (SizeMod ? 2 : 0);

	std::string OutputData(OutputSize, 0);
	auto OutputDataIt = OutputData.data();

	const auto LoopBegin = reinterpret_cast<const uint8_t*>(BufferToDecode.data());
	const auto BufferEnd = reinterpret_cast<const uint8_t*>(BufferToDecode.data() + BufferSize);
	const auto LoopEnd = BufferEnd - 4;

	auto It = LoopBegin;
	for (; It < LoopEnd; ++It)
	{
		uint8_t Index = DecodingTable[*It];
		uint8_t Character = Index << 2;
		Index = DecodingTable[*++It];
		Character |= Index >> 4;
		*OutputDataIt++ = Character;

		Character = Index << 4;
		Index = DecodingTable[*++It];
		Character |= Index >> 2;
		*OutputDataIt++ = Character;

		Character = Index << 6;
		Index = DecodingTable[*++It];
		Character |= Index;
		*OutputDataIt++ = Character;
	}

	if (It < BufferEnd)
	{
		uint8_t Index = DecodingTable[*It];
		uint8_t Character = Index << 2;
		Index = DecodingTable[*++It];
		Character |= Index >> 4;
		*OutputDataIt++ = Character;

		size_t ResizedLenght = OutputSize;

		if (!((It + 1) < BufferEnd && *(It + 1) == PaddingSymbol))
		{
			Character = Index << 4;
			Index = DecodingTable[*++It];
			Character |= Index >> 2;
			*OutputDataIt++ = Character;

			if (!((It + 1) < BufferEnd && *(It + 1) == PaddingSymbol))
			{
				Character = Index << 6;
				Index = DecodingTable[*++It];
				Character |= Index;
				*OutputDataIt++ = Character;
			}
			else
			{
				ResizedLenght = OutputSize - 1;
			}
		}
		else
		{
			ResizedLenght = OutputSize - 2;
		}

		if (SizeMod)
		{
			ResizedLenght = OutputSize - (3 - SizeMod);
		}
		OutputData.resize(ResizedLenght);
	}

	return OutputData;
}
