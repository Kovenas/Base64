#include "Base64.hpp"

#include <array>

constexpr auto EncodingTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr auto PaddingSymbol = '=';

size_t Base64::GetEncodedBufferSize(size_t BufferSize)
{
	size_t Mod = BufferSize % 3;
	size_t Size = (BufferSize / 3) * 4;
	Size += Mod ? 4 : 0; // Include padding symbols
	return Size;
}

size_t Base64::GetDecodedBufferSize(size_t BufferSize)
{
	size_t Mod = BufferSize % 4;
	size_t Size = (BufferSize / 4) * 3;
	Size += Mod ? 2 : 0; // Include last chars
	return Size;
}

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
		uint8_t Byte = *It;
		uint8_t Index = Byte >> 2;
		uint8_t Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (Byte & 0x03) << 4;
		Byte = *++It;
		Index |= Byte >> 4;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (Byte & 0x0F) << 2;
		Byte = *++It;
		Index |= Byte >> 6;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = Byte & 0x3F;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;
	}

	if (SizeMod == 1)
	{
		uint8_t Byte = *It;
		uint8_t Index = Byte >> 2;
		uint8_t Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (Byte & 0x03) << 4;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		*OutputBufferIt++ = PaddingSymbol;
		*OutputBufferIt++ = PaddingSymbol;
	}
	else if (SizeMod == 2)
	{
		uint8_t Byte = *It;
		uint8_t Index = Byte >> 2;
		uint8_t Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (Byte & 0x03) << 4;
		Byte = *++It;
		Index |= Byte >> 4;
		Character = EncodingTable[Index];
		*OutputBufferIt++ = Character;

		Index = (Byte & 0x0F) << 2;
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

static size_t DecodeBase(const uint8_t* BufferBegin, const uint8_t* BufferEnd, size_t SizeMod, size_t OutputSize, uint8_t* OutputDataIt)
{
	const auto LoopEnd = BufferEnd - 4;
	auto It = BufferBegin;
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
		return ResizedLenght;
	}
	else
	{
		return OutputSize;
	}
}

std::string Base64::Decode(std::string_view BufferToDecode)
{
	const size_t BufferSize = BufferToDecode.size();
	const size_t SizeMod = BufferSize % 4;
	const size_t OutputSize = (BufferSize / 4) * 3 + (SizeMod ? 2 : 0);

	std::string OutputData(OutputSize, 0);
	auto OutputDataIt = reinterpret_cast<uint8_t*>(OutputData.data());

	const auto BufferBegin = reinterpret_cast<const uint8_t*>(BufferToDecode.data());
	const auto BufferEnd = reinterpret_cast<const uint8_t*>(BufferToDecode.data() + BufferSize);

	size_t FinalSize = DecodeBase(BufferBegin, BufferEnd, SizeMod, OutputSize, OutputDataIt);
	OutputData.resize(FinalSize);
	return OutputData;
}

void Base64::DecodeInPlace(std::string& BufferToDecode)
{
	const size_t BufferSize = BufferToDecode.size();
	const size_t SizeMod = BufferSize % 4;
	const size_t OutputSize = (BufferSize / 4) * 3 + (SizeMod ? 2 : 0);

	auto OutputDataIt = reinterpret_cast<uint8_t*>(BufferToDecode.data());

	const auto BufferBegin = reinterpret_cast<const uint8_t*>(BufferToDecode.data());
	const auto BufferEnd = reinterpret_cast<const uint8_t*>(BufferToDecode.data() + BufferSize);

	size_t FinalSize = DecodeBase(BufferBegin, BufferEnd, SizeMod, OutputSize, OutputDataIt);
	BufferToDecode.resize(FinalSize);
}
