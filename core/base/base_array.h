#pragma once

// @Note: This is just a simple C-style resizeable array.
template<typename T>
struct Array {
    // @Robustness: (we could do this)
    // static_assert(std::is_trivially_copyable_v<T>, "Array only supports trivially copyable types");
    
    // @Note: C++ forces you to do freaking methods for some stuff.
    using Iterator = T *;
    using Const_Iterator = const T *;

    Iterator begin() noexcept { return data; }
    Iterator end() noexcept { return data + count; }
    Const_Iterator begin() const noexcept { return data; }
    Const_Iterator end() const noexcept { return data + count; }

    fn operator[](u32 index) -> T& {
        checkf(index < count, "Array out of bounds!");
        return data[index];
    }

    T* data = nullptr;
    u32 count = 0;
    u32 cap = 0;
};

template<typename T>
fn count(Array<T> array) -> u32 {
    return array.count;
}

template<typename T>
fn append(Array<T>* array, const T& elem = {}) -> T& {
    try_grow(array, 1);
    auto& last = array->data[array->count];
    last = elem;
    ++array->count;
    return last;
}

template<typename T>
fn reserve(Array<T>* array, u32 items) -> void {
    if (items <= array->cap) {
        return;
    }
    T* prev_data = array->data;
    u32 prev_cap = array->cap;
    array->data = new T[items]();
    array->cap = items;
    if (prev_cap == 0) {
        return;
    }
    memcpy(array->data, prev_data, (u64) sizeof(T) * array->count);
    delete [] prev_data;
}

template<typename T>
fn try_grow(Array<T>* array, u32 items) -> bool {
    u32 needed_cap = array->count + items;
    if (needed_cap <= array->cap) {
        return false;
    }
    constexpr u32 MIN_CAP = 8u;
    // Reserve at least double the current count, but ensure we have space for needed_cap
    u32 to_reserve = std::max(MIN_CAP, std::max(2 * array->count, needed_cap));
    reserve(array, to_reserve);
    return true;
}

template<typename T>
fn reset(Array<T>* array) -> void {
    delete[] array->data;
    array->data = nullptr;
    array->count = 0;
    array->cap = 0;
}

template<typename T>
fn reset_keeping_memory(Array<T>* array) -> void {
    array->count = 0;
}

template<typename T>
fn find(Array<T> array, const T& elem, u32* index = nullptr) -> bool {
    u32 i = 0;
    for (auto& it : array) {
        if (elem == it) {
            if (index) {
                *index = i;
            }
            return true;
        }
        ++i;
    }
    return false;
}

template<typename T>
fn unordered_remove(Array<T>* array, u32 index) -> bool {
    if (index >= array->count) {
        return false;
    }
    u32 last_index = array->count - 1;
    if (index != last_index) {
        array->data[index] = array->data[last_index];
    }
    --array->count;
    return true;
}

template<typename T>
fn ordered_remove(Array<T>* array, u32 index) -> bool {
    if (index >= array->count) {
        return false;
    }
    for (u32 i = index; i < array->count - 1; ++i) {
        array->data[i] = array->data[i + 1];
    }
    --array->count;
    return true;
}