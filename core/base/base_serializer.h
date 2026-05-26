#pragma once

struct Serializer {
    static constexpr s32 indent_size = 4;
    std::string out;
    s32 indent_level = 0;
};

fn serialize_indent(Serializer* s) -> void;
fn serialize_block_init(Serializer* s) -> void;
fn serialize_block_done(Serializer* s) -> void;
fn serialize_field(Serializer* s, std::string_view field) -> void;
fn serialize_new_line(Serializer* s) -> void;

template<typename T>
fn serialize_field(Serializer* s, std::string_view field, const T& value) -> void {
    serialize_field(s, field);
    if constexpr (std::is_arithmetic_v<T>) {
        s->out.append(std::to_string(value));
    } else if constexpr (std::is_same_v<T, std::string>) {
        s->out.append(value);
    } else if constexpr (std::is_same_v<T, const char*>) {
        s->out.append(value);
    } else { // Here we asume that it is a complex type.
        serialize_new_line(s);
        serialize(s, value);
    }
    s->out += "\n";
}

fn serialize(Serializer* s, const Vec2& v) -> void;
fn serialize(Serializer* s, const Vec3& v) -> void;
fn serialize(Serializer* s, const Vec4& v) -> void;

struct Deserializer {
    std::string_view src;
    s32 cursor = 0;
};

// Core parsing primitives
fn deserialize_skip_whitespace(Deserializer* d) -> void;
fn deserialize_block_init(Deserializer* d) -> bool;
fn deserialize_block_done(Deserializer* d) -> bool;
fn deserialize_read_key(Deserializer* d) -> std::string_view;
fn deserialize_skip_line(Deserializer* d) -> void;
fn deserialize_peek_block_done(Deserializer* d) -> bool;

// Read a raw value from the current line
fn deserialize_line_value(Deserializer* d) -> std::string_view;

// Read a typed value from the current line
template<typename T>
fn deserialize_value(Deserializer* d, T& value) -> void {
    std::string_view raw = deserialize_line_value(d);
    if constexpr (std::is_same_v<T, float>) {
        value = std::stof(std::string(raw));
    } else if constexpr (std::is_same_v<T, s32>) {
        value = std::stoi(std::string(raw));
    } else if constexpr (std::is_same_v<T, u32>) {
        value = (u32)std::stoul(std::string(raw));
    } else if constexpr (std::is_same_v<T, bool>) {
        value = (raw == "true" || raw == "1");
    } else if constexpr (std::is_same_v<T, std::string>) {
        value = std::string(raw);
    } else { // Complex type - recurse
        deserialize(d, &value);
    }
}

fn deserialize(Deserializer* d, Vec3* v) -> void;
fn deserialize(Deserializer* d, Vec4* v) -> void;