#include "asset.h"
#include "graphics.h"
#include "draw.h"

Asset_Manager asset_manager;

fn asset_create(Asset_Kind kind) -> Asset_Handle {
    switch (kind) {
        #define DeclareCase(AssetType) \
        case Asset_Kind_##AssetType: { \
            Array_Handle handle_value = append(&asset_manager.AssetType##_Storage); \
            Asset_Handle handle; \
            handle.kind = Asset_Kind_##AssetType; \
            handle.value = handle_value; \
            return handle; \
        }
        ForAssetTypes(DeclareCase)
        
        #undef DeclareCase

        case Asset_Kind_None: 
        default: return {};
    }
}

fn asset_get(Asset_Handle handle) -> void* {
    switch (handle.kind) {
        #define DeclareCase(AssetType) \
        case Asset_Kind_##AssetType: { \
            return get(asset_manager.AssetType##_Storage, handle.value); \
        }
        ForAssetTypes(DeclareCase)
        #undef DeclareCase

        case Asset_Kind_None: 
        default: return {};
    }
}

fn asset_destroy(Asset_Handle handle) -> void {
    switch (handle.kind) {
        #define DeclareCase(AssetType) \
        case Asset_Kind_##AssetType: { \
            remove(&asset_manager.AssetType##_Storage, handle.value); \
            return; \
        }
        ForAssetTypes(DeclareCase)
        #undef DeclareCase

        case Asset_Kind_None: 
        default: return;
    }
}

fn asset_free_all() -> void {
    #define ResetStorage(AssetType) \
        reset(&asset_manager.AssetType##_Storage); \

    ForAssetTypes(ResetStorage)
    #undef ResetStorage
}