#pragma once
#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>

namespace app
{

    enum tag_2b : uint8_t
    {
        UNIVERSAL = 0x0,
        APPLIED = 0x40,
        CONTEXT_SENSITIVE = 0x80,
        PRIVATE = 0xC0,
        UNKNOWN_TAG = 0xFF
    };

    std::string tag_to_string(tag_2b t);
    std::string vtk_stringify(uint8_t id);
    bool is_2byte_tag(uint8_t tag);

    template<typename T>
    bool is_bit(T t, uint16_t pos)
    {
        return (t & (1 << pos)) ? true : false;
    }

    struct tag_descr
    {
        tag_2b b2 = UNKNOWN_TAG;
        bool type;
        uint16_t number;

        std::string info();
    };

    class tlv
    {
    public:
        tlv() = default;
        tlv(uint16_t tag, uint16_t length, const std::string& value);

        void write(FILE* file);
        void read(FILE* file);
        std::string info();

    private:
        tag_descr tag_info();
        tag_2b get_tag_2b(uint8_t t);

    private:
        uint16_t _tag = 0;
        uint16_t _length = 0;
        std::string _value{};
    };
}
