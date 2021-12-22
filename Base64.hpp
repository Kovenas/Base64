#pragma once

#include <string>

namespace Base64
{
	constexpr auto GetEncodedDataSize(size_t BufferSize)
	{
		size_t Mod = BufferSize % 3;
		size_t Size = (BufferSize / 3) * 4;
		Size += Mod ? 4 : 0; // Include padding symbols
		return Size;
	}

	constexpr auto GetDecodedDataSize(size_t EncodedBufferSize)
	{
		size_t Mod = EncodedBufferSize % 4;
		size_t Size = (EncodedBufferSize / 4) * 3;
		Size += Mod ? 2 : 0; // Include last chars
		return Size;
	}

	std::string Encode(std::string_view BufferToEncode);

	std::string Decode(std::string_view BufferToDecode);

	// Decodes Buffer and modifies it on the fly
	// Doesn't allocate memory
	void DecodeRef(std::string& BufferToDecode);
}
