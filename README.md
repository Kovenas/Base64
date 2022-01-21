# Base64

C++ Base64 library

# Usage

Include files to your project and that is it.

```cpp
const auto Data = "Many hands make light work.";
auto EncodedData = Base64::Encode(Data);
auto DecodedData = Base64::Decode(EncodedData);
EXPECT_EQ(Data, DecodedData);
```

The padding(`=`) is optional for the Decoder, e.g.:

```cpp
auto Output1 = Base64::Decode("TWFueSBoYW5kcw");
auto Output2 = Base64::Decode("TWFueSBoYW5kcw==");
auto ExpectedOutput = "Many hands";
EXPECT_EQ(Output1, ExpectedOutput);
EXPECT_EQ(Output2, ExpectedOutput);
```

Decoding and writing to the same buffer without allocating more memory

```cpp
std::string Data = "TWFueSBoYW5kcw==";
Base64::DecodeInPlace(Data);
EXPECT_EQ(Data, "Many hands");
```

# Tests

Tests for these functions exists in other my project, but if you need them please create an issue or contact me and I will add them.
