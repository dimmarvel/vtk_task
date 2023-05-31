#pragma once
#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace app
{
    class tlv;
    typedef std::function<std::string(const tlv&, size_t&)> handler;
    typedef std::pair<std::string, handler> descr_handler;

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
    descr_handler param_to_string(uint8_t id);

    template<typename T>
    bool is_bit(T t, uint16_t pos)
    {
        return (t & (1 << pos)) ? true : false;
    }

    struct tag_descr
    {
        tag_2b b2 = UNKNOWN_TAG;
        bool type; //Package nesting is not implemented
        uint16_t number;

        std::string info();
    };

    class tlv
    {
    public:
        tlv() = default;
        tlv(uint16_t tag, uint16_t length, const std::vector<uint8_t>& value);

        void write(FILE* file);
        void read(FILE* file);
        std::string info();

        //Return description
        std::string handle_0x01(size_t& i) const;
        std::string empty_handler(size_t& i) const { return ""; }

    private:
        tag_descr tag_info();
        tag_2b get_tag_2b(uint8_t t);


    private:
        uint16_t _tag = 0;
        uint16_t _length = 0;
        std::vector<uint8_t> _value{};
    };

    // Empty handler need implement
    #define EMPTY_HANDLER &tlv::empty_handler
    static const std::unordered_map<uint8_t, descr_handler> DESCR_PARAMS = 
    {
        { 0x01, {"Message name", &tlv::handle_0x01}},
        { 0x03, {"Operation number", EMPTY_HANDLER}}, 
        { 0x04, {"Amount in minor currency unit", EMPTY_HANDLER}}, 
        { 0x05, {"Keepalive interval in seconds", EMPTY_HANDLER}}, 
        { 0x06, {"Operation timeout in seconds", EMPTY_HANDLER}}, 
        { 0x07, {"Event name", EMPTY_HANDLER}},
        { 0x08, {"Event number", EMPTY_HANDLER}}, 
        { 0x09, {"Product id", EMPTY_HANDLER}}, 
        { 0x0A, {"QR-Code data", EMPTY_HANDLER}}, 
        { 0x0B, {"TCP/IP destination", EMPTY_HANDLER}}, 
        { 0x0C, {"Outgoing byte counter", EMPTY_HANDLER}}, 
        { 0x0D, {"Simple data block", EMPTY_HANDLER}}, 
        { 0x0E, {"Confirmable data block", EMPTY_HANDLER}}, 
        { 0x0F, {"Product name", EMPTY_HANDLER}}, 
        { 0x10, {"POS management data", EMPTY_HANDLER}}, 
        { 0x11, {"Local time", EMPTY_HANDLER}}, 
        { 0x12, {"System information", EMPTY_HANDLER}}, 
        { 0x13, {"Banking receipt", EMPTY_HANDLER}}, 
        { 0x14, {"Display time in milliseconds", EMPTY_HANDLER}}, 
        { 0x15, {"Image name", EMPTY_HANDLER}},
        { 0x16, {"Image flags", EMPTY_HANDLER}}, 
        { 0x17, {"Display text", EMPTY_HANDLER}}, 
        { 0x18, {"Coordinates and color", EMPTY_HANDLER}}, 
        { 0x19, {"Host Id", EMPTY_HANDLER}}
    };
}
