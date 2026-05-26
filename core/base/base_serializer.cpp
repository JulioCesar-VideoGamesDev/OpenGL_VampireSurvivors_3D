#include "base_serializer.h"

fn serialize_indent(Serializer* s) -> void {
    s32 indent = s->indent_level * s->indent_size;
    s->out.append(indent, ' ');
}

fn serialize_block_init(Serializer* s) -> void {
    serialize_indent(s);
    s->out += "{\n";
    s->indent_level++;
}

fn serialize_block_done(Serializer* s) -> void {
    s->indent_level--;
    serialize_indent(s);
    s->out += "}\n";
}

fn serialize_field(Serializer* s, std::string_view field) -> void {
    serialize_indent(s);
    s->out += field;
    s->out += ": ";
}

fn serialize_new_line(Serializer* s) -> void {
    s->out += "\n";
}

fn serialize(Serializer* s, const Vec2& v) -> void {
    serialize_block_init(s);
    serialize_field(s, "x", v.x);
    serialize_field(s, "y", v.y);
    serialize_block_done(s);
}

fn serialize(Serializer* s, const Vec3& v) -> void {
    serialize_block_init(s);
    serialize_field(s, "x", v.x);
    serialize_field(s, "y", v.y);
    serialize_field(s, "z", v.z);
    serialize_block_done(s);
}

fn serialize(Serializer* s, const Vec4& v) -> void {
    serialize_block_init(s);
    serialize_field(s, "x", v.x);
    serialize_field(s, "y", v.y);
    serialize_field(s, "z", v.z);
    serialize_field(s, "w", v.w);
    serialize_block_done(s);
}

fn deserialize_skip_whitespace(Deserializer* d) -> void {
    while (d->cursor < (s32)d->src.size()) {
        char c = d->src[d->cursor];
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t') break;
        ++d->cursor;
    }
}

fn deserialize_block_init(Deserializer* d) -> bool {
    deserialize_skip_whitespace(d);
    if (d->cursor >= (s32)d->src.size() || d->src[d->cursor] != '{') return false;
    ++d->cursor;
    return true;
}

fn deserialize_peek_block_done(Deserializer* d) -> bool {
    // Peek ahead (skip whitespace) to check if next non-whitespace char is '}'
    s32 tmp = d->cursor;
    while (tmp < (s32)d->src.size()) {
        char c = d->src[tmp];
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t') break;
        ++tmp;
    }
    return tmp < (s32)d->src.size() && d->src[tmp] == '}';
}

fn deserialize_block_done(Deserializer* d) -> bool {
    deserialize_skip_whitespace(d);
    if (d->cursor >= (s32)d->src.size() || d->src[d->cursor] != '}') return false;
    ++d->cursor;
    return true;
}

fn deserialize_read_key(Deserializer* d) -> std::string_view {
    deserialize_skip_whitespace(d);
    
    // Read until ':' 
    s32 start = d->cursor;
    while (d->cursor < (s32)d->src.size() && d->src[d->cursor] != ':' && d->src[d->cursor] != '\n') {
        ++d->cursor;
    }
    
    // Trim trailing spaces from key
    s32 end = d->cursor;
    while (end > start && d->src[end - 1] == ' ') {
        --end;
    }
    
    std::string_view key = d->src.substr(start, end - start);
    
    // Consume the ':'
    if (d->cursor < (s32)d->src.size() && d->src[d->cursor] == ':') {
        ++d->cursor;
    }
    
    // Skip space after ':'
    while (d->cursor < (s32)d->src.size() && d->src[d->cursor] == ' ') {
        ++d->cursor;
    }
    
    return key;
}

fn deserialize_line_value(Deserializer* d) -> std::string_view {
    s32 start = d->cursor;
    while (d->cursor < (s32)d->src.size() && d->src[d->cursor] != '\n') {
        ++d->cursor;
    }
    std::string_view value = d->src.substr(start, d->cursor - start);
    if (d->cursor < (s32)d->src.size()) ++d->cursor; // consume '\n'
    return value;
}

fn deserialize_skip_line(Deserializer* d) -> void {
    // Check if the rest of the current line is empty (complex type on next line)
    // or has a value (simple type)
    s32 tmp = d->cursor;
    while (tmp < (s32)d->src.size() && d->src[tmp] == ' ') ++tmp;
    
    bool is_complex = (tmp >= (s32)d->src.size() || d->src[tmp] == '\n');
    
    if (is_complex) {
        // Skip to end of current line
        while (d->cursor < (s32)d->src.size() && d->src[d->cursor] != '\n') ++d->cursor;
        if (d->cursor < (s32)d->src.size()) ++d->cursor; // consume '\n'
        
        // Skip the entire block
        deserialize_skip_whitespace(d);
        if (d->cursor < (s32)d->src.size() && d->src[d->cursor] == '{') {
            s32 depth = 0;
            while (d->cursor < (s32)d->src.size()) {
                char c = d->src[d->cursor++];
                if      (c == '{') ++depth;
                else if (c == '}') { --depth; if (depth == 0) break; }
            }
            // consume trailing newline
            if (d->cursor < (s32)d->src.size() && d->src[d->cursor] == '\n') ++d->cursor;
        }
    } else {
        // Simple value - skip to end of line
        while (d->cursor < (s32)d->src.size() && d->src[d->cursor] != '\n') ++d->cursor;
        if (d->cursor < (s32)d->src.size()) ++d->cursor; // consume '\n'
    }
}

fn deserialize(Deserializer* d, Vec2* v) -> void {
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        std::string_view key = deserialize_read_key(d);
        if      (key == "x") deserialize_value(d, v->x);
        else if (key == "y") deserialize_value(d, v->y);
        else deserialize_skip_line(d); // Unknown field, skip it
    }
    deserialize_block_done(d);
}

fn deserialize(Deserializer* d, Vec3* v) -> void {
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        std::string_view key = deserialize_read_key(d);
        if      (key == "x") deserialize_value(d, v->x);
        else if (key == "y") deserialize_value(d, v->y);
        else if (key == "z") deserialize_value(d, v->z);
        else deserialize_skip_line(d); // Unknown field, skip it
    }
    deserialize_block_done(d);
}

fn deserialize(Deserializer* d, Vec4* v) -> void {
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        std::string_view key = deserialize_read_key(d);
        if      (key == "x") deserialize_value(d, v->x);
        else if (key == "y") deserialize_value(d, v->y);
        else if (key == "z") deserialize_value(d, v->z);
        else if (key == "w") deserialize_value(d, v->w);
        else deserialize_skip_line(d); // Unknown field, skip it
    }
    deserialize_block_done(d);
}