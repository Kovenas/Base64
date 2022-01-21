#pragma once

#include <string>

namespace Base64
{
	size_t GetEncodedBufferSize(size_t BufferSize);

	size_t GetDecodedBufferSize(size_t BufferSize);

	std::string Encode(std::string_view BufferToEncode);

	std::string Decode(std::string_view BufferToDecode);

	// Decoding and writing output to the same buffer.
	// Decoded data always takes less memory, so calling std::string::resize
	// the buffer shouldn't be reallocated (depends on std::string::resize implementation).
	void DecodeInPlace(std::string& Buffer);
}
