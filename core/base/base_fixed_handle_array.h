#pragma once

// @Note: This is just a C-style ZII Fixed Generational Array 
struct Array_Handle {
    u32 index = 0u;
    u32 generation = 0u;
};

// Handle utility functions
inline fn is_valid(Array_Handle handle) -> bool {
    return handle.index != 0u || handle.generation != 0u;
}

inline fn operator==(const Array_Handle& a, const Array_Handle& b) -> bool {
    return a.index == b.index && a.generation == b.generation;
}

inline fn operator!=(const Array_Handle& a, const Array_Handle& b) -> bool {
    return !(a == b);
}

inline static constexpr u32 _default_cap_fixed_handle_array = 30u;

template<typename T, u32 _cap = _default_cap_fixed_handle_array>
struct Fixed_Handle_Array {
    
    struct Elem_Info {
        u32 generation = 0u;
        bool occupied = false;
    };
    
    // User specifies usable capacity, we add +1 for dummy slot (ZII)
    static constexpr u32 cap = _cap + 1u;
    
    u32 count = 0u;
    T* data = nullptr;
    Elem_Info* info_data = nullptr;
    
    // Free list for O(1) add/remove
    u32* free_list = nullptr;
    u32 free_list_head = 0u;  // Index into free_list, 0 means empty (ZII)
    
    // Iterator for traversing only valid elements
    struct Iterator {
        Fixed_Handle_Array* array;
        u32 current_index;
        
        Iterator(Fixed_Handle_Array* arr, u32 idx) 
            : array(arr), current_index(idx) {
            advance_to_next_valid();
        }
        
        fn advance_to_next_valid() -> void {
            while (current_index < cap && !array->info_data[current_index].occupied) {
                ++current_index;
            }
        }
        
        fn operator*() -> T& {
            return array->data[current_index];
        }
        
        fn operator->() -> T* {
            return &array->data[current_index];
        }
        
        fn operator++() -> Iterator& {
            ++current_index;
            advance_to_next_valid();
            return *this;
        }
        
        fn operator++(int) -> Iterator {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        fn operator==(const Iterator& other) const -> bool {
            return current_index == other.current_index;
        }
        
        fn operator!=(const Iterator& other) const -> bool {
            return current_index != other.current_index;
        }
        
        fn handle() const -> Array_Handle {
            return {current_index, array->info_data[current_index].generation};
        }
    };
    
    // Const iterator
    struct Const_Iterator {
        const Fixed_Handle_Array* array;
        u32 current_index;
        
        Const_Iterator(const Fixed_Handle_Array* arr, u32 idx)
            : array(arr), current_index(idx) {
            advance_to_next_valid();
        }
        
        fn advance_to_next_valid() -> void {
            while (current_index < cap && !array->info_data[current_index].occupied) {
                ++current_index;
            }
        }
        
        fn operator*() const -> const T& {
            return array->data[current_index];
        }
        
        fn operator->() const -> const T* {
            return &array->data[current_index];
        }
        
        fn operator++() -> Const_Iterator& {
            ++current_index;
            advance_to_next_valid();
            return *this;
        }
        
        fn operator++(int) -> Const_Iterator {
            Const_Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        fn operator==(const Const_Iterator& other) const -> bool {
            return current_index == other.current_index;
        }
        
        fn operator!=(const Const_Iterator& other) const -> bool {
            return current_index != other.current_index;
        }
        
        fn handle() const -> Array_Handle {
            return {current_index, array->info_data[current_index].generation};
        }
    };
};

// Iterator interface (kept as members because required for range-based for)
template<typename T, u32 _cap>
fn begin(Fixed_Handle_Array<T, _cap>& array) -> typename Fixed_Handle_Array<T, _cap>::Iterator {
    return typename Fixed_Handle_Array<T, _cap>::Iterator(&array, 1u);
}

template<typename T, u32 _cap>
fn end(Fixed_Handle_Array<T, _cap>& array) -> typename Fixed_Handle_Array<T, _cap>::Iterator {
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    return typename Fixed_Handle_Array<T, _cap>::Iterator(&array, total_cap);
}

template<typename T, u32 _cap>
fn begin(Fixed_Handle_Array<T, _cap> const& array) -> typename Fixed_Handle_Array<T, _cap>::Const_Iterator {
    return typename Fixed_Handle_Array<T, _cap>::Const_Iterator(&array, 1u);
}

template<typename T, u32 _cap>
fn end(Fixed_Handle_Array<T, _cap> const& array) -> typename Fixed_Handle_Array<T, _cap>::Const_Iterator {
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    return typename Fixed_Handle_Array<T, _cap>::Const_Iterator(&array, total_cap);
}

// Core operations as free functions

template<typename T, u32 _cap>
fn init(Fixed_Handle_Array<T, _cap>* array) -> void {
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    array->data = new T[total_cap]{};  // ZII: slot 0 is dummy element
    array->info_data = new typename Fixed_Handle_Array<T, _cap>::Elem_Info[total_cap]{};
    array->free_list = new u32[_cap]{};  // _cap usable slots (slot 0 excluded)
    array->count = 0u;
    array->free_list_head = 0u;
    
    // Initialize free list with all available slots (1 to total_cap-1)
    // Store in reverse order so we pop from the front efficiently
    for (u32 i = 1u; i < total_cap; ++i) {
        array->free_list[i - 1u] = i;
    }
    array->free_list_head = _cap;  // Points to last valid index in free_list
}

template<typename T, u32 _cap>
fn reset(Fixed_Handle_Array<T, _cap>* array) -> void {
    if (array->data) {
        delete[] array->data;
        array->data = nullptr;
    }
    if (array->info_data) {
        delete[] array->info_data;
        array->info_data = nullptr;
    }
    if (array->free_list) {
        delete[] array->free_list;
        array->free_list = nullptr;
    }
    array->count = 0u;
    array->free_list_head = 0u;
}

template<typename T, u32 _cap>
fn reset_keeping_memory(Fixed_Handle_Array<T, _cap>* array) -> void {
    // Don't deallocate, just reset state
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    
    // Reset all occupied slots
    for (u32 i = 1u; i < total_cap; ++i) {
        if (array->info_data[i].occupied) {
            array->info_data[i].occupied = false;
            ++array->info_data[i].generation;
            array->data[i] = T{};
        }
    }
    
    // Rebuild free list
    for (u32 i = 1u; i < total_cap; ++i) {
        array->free_list[i - 1u] = i;
    }
    array->free_list_head = _cap;
    array->count = 0u;
}

template<typename T, u32 _cap>
fn append(Fixed_Handle_Array<T, _cap>* array, const T& element = {}) -> Array_Handle {
    // Auto-initialize if not initialized (ZII: nullptr means uninitialized)
    if (array->data == nullptr) {
        init(array);
    }
    
    // Check if we have free slots (ZII: 0 means empty)
    if (array->free_list_head == 0u) {
        return {0u, 0u};  // Array is full
    }
    
    // Pop from free list (O(1))
    --array->free_list_head;
    u32 index = array->free_list[array->free_list_head];
    
    // Add element
    array->data[index] = element;
    array->info_data[index].occupied = true;
    ++array->count;
    
    return {index, array->info_data[index].generation};
}

template<typename T, u32 _cap>
fn remove(Fixed_Handle_Array<T, _cap>* array, Array_Handle handle) -> bool {
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    if (handle.index == 0u || handle.index >= total_cap) {
        return false;
    }
    
    typename Fixed_Handle_Array<T, _cap>::Elem_Info& info = array->info_data[handle.index];
    
    // Check if generation matches (handle is still valid)
    if (!info.occupied || info.generation != handle.generation) {
        return false;
    }
    
    // Mark as unoccupied and increment generation
    info.occupied = false;
    ++info.generation;
    --array->count;
    
    // Reset data to default constructed state
    array->data[handle.index] = T{};
    
    // Push to free list (O(1))
    array->free_list[array->free_list_head] = handle.index;
    ++array->free_list_head;
    
    return true;
}

template<typename T, u32 _cap>
fn get_default(Fixed_Handle_Array<T, _cap> array) -> T* {
    return &array.data[0];
}

template<typename T, u32 _cap>
fn get(Fixed_Handle_Array<T, _cap> array, Array_Handle handle) -> T* {
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    if (handle.index == 0u || handle.index >= total_cap) {
        return get_default(array);
    }
    
    const typename Fixed_Handle_Array<T, _cap>::Elem_Info& info = array.info_data[handle.index];
    if (!info.occupied || info.generation != handle.generation) {
        return get_default(array);
    }
    
    return &array.data[handle.index];
}

template<typename T, u32 _cap>
fn is_valid(Fixed_Handle_Array<T, _cap> array, Array_Handle handle) -> bool {
    constexpr u32 total_cap = Fixed_Handle_Array<T, _cap>::cap;
    if (handle.index == 0u || handle.index >= total_cap) {
        return false;
    }
    
    const typename Fixed_Handle_Array<T, _cap>::Elem_Info& info = array.info_data[handle.index];
    return info.occupied && info.generation == handle.generation;
}

template<typename T, u32 _cap>
fn count(Fixed_Handle_Array<T, _cap> array) -> u32 {
    return array.count;
}

template<typename T, u32 _cap>
fn is_full(Fixed_Handle_Array<T, _cap> array) -> bool {
    return array.free_list_head == 0u;  // No free slots available
}

template<typename T, u32 _cap>
fn is_empty(Fixed_Handle_Array<T, _cap> array) -> bool {
    return array.count == 0u;
}