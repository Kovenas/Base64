#pragma once

#include <string>

namespace Base64
{
	size_t GetEncodedBufferSize(size_t BufferSize);

	size_t GetDecodedBufferSize(size_t BufferSize);

	std::string Encode(std::string_view BufferToEncode);

	std::string Decode(std::string_view BufferToDecode);

	// Decodes Buffer and modifies it on the fly
	// Doesn't allocate memory
	void DecodeRef(std::string& BufferToDecode);
}
