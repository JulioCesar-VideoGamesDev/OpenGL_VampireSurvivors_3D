#pragma once

#define ForAssetTypes(AssetType) \
    AssetType(Texture)           \
    AssetType(Shader)            \
    AssetType(Mesh)              \
    AssetType(Material)          \

enum Asset_Kind: u64 {
    Asset_Kind_None = 0,
    #define DeclareEnumEntry(AssetType) \
        Asset_Kind_##AssetType,    
    
    ForAssetTypes(DeclareEnumEntry)
    #undef DeclareEnumEntry
};

#define ForwardDeclare(AssetType) \
    struct AssetType;
    ForAssetTypes(ForwardDeclare)

#undef ForwardDeclare

struct Asset_Manager {
    #define DeclareStorage(AssetType) \
        Fixed_Handle_Array<AssetType> AssetType##_Storage;
    ForAssetTypes(DeclareStorage)
    #undef DeclareStorage
};

struct Asset_Handle {
    Asset_Kind kind;
    Array_Handle value;
};

fn asset_create(Asset_Kind kind) -> Asset_Handle;
fn asset_get(Asset_Handle handle) -> void*;
fn asset_destroy(Asset_Handle handle) -> void;
fn asset_free_all() -> void;