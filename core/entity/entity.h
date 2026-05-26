// ===============================================================================
// @Region: Entity Kind
// ===============================================================================

#ifndef ENTITY_KIND_H
#define ENTITY_KIND_H

// Entity type list.
// ---------------------------------------------------

#define ForEntityTypes(EntityType)  \
    EntityType(Player)              \

// ---------------------------------------------------

#define EntityKind(EntityType) \
    Entity_Kind_##EntityType   \

// Kind enum.
enum Entity_Kind : u64 {
    Entity_Kind_None = 0,
    #define DeclareEnumEntry(EntityType) \
        EntityKind(EntityType),

        ForEntityTypes(DeclareEnumEntry)
    
    #undef DeclareEnumEntry
};

inline fn to_string(Entity_Kind kind) -> const char* {
    switch (kind) {
        #define DeclareCaseEntry(EntityType) \
            case EntityKind(EntityType): return Stringify(EntityKind(EntityType));

            ForEntityTypes(DeclareCaseEntry)
        #undef DeclareCaseEntry
        
        default: return "";
    }
}

inline fn from_string(std::string_view str, Entity_Kind* pKind) {
    #define DeclareIfEntry(EntityType)                  \
        if (str == Stringify(EntityKind(EntityType))) { \
           *pKind = EntityKind(EntityType);             \
            return;                                     \
        }

        ForEntityTypes(DeclareIfEntry)
        #undef DeclareIfEntry
}

// Forward declarations.
#define ForwardDeclare(EntityType) \
    struct EntityType;
    
    ForEntityTypes(ForwardDeclare)

#undef ForwardDeclare

#endif //ENTITY_KIND_H

// ===============================================================================
// @Region: Entity Base Type
// ===============================================================================

// ===============================
// @Region: Declaration
// ===============================

#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

// Base entity type.
struct Entity {
    Entity_Kind kind = Entity_Kind_None;
    bool enabled = true;
    bool visible = true;
    Vec3 pos = F32.Zero;
    Vec3 rot = F32.Zero;
    Vec3 scl = F32.One;
    Vec4 tint = Color.White;
    s32  sprite = 0;
    struct Texture* tex = nullptr; // @Pending: This should be an asset handle.
};

fn serialize(Serializer* s, const Entity& e) -> void;
fn deserialize(Deserializer* d, Entity* e) -> void;

fn serialize_fields_base_entity(Serializer* s, const Entity& e) -> void;
fn deserialize_fields_base_entity(Deserializer* d, Entity* e) -> bool;

#endif

// ===============================
// @Region: Implementation
// ===============================

#ifdef ENTITY_IMPL

fn serialize(Serializer* s, const Entity& e) -> void {
    serialize_block_init(s);
    serialize_fields_base_entity(s, e);
    serialize_block_done(s);
}

fn deserialize(Deserializer* d, Entity* e) -> void {
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        if (!deserialize_fields_base_entity(d, e)) {
            deserialize_skip_line(d);
        }
    }
    deserialize_block_done(d);
}

fn serialize_fields_base_entity(Serializer* s, const Entity& e) -> void {
    serialize_field(s, "kind", to_string(e.kind));
    serialize_field(s, "enabled", e.enabled);
    serialize_field(s, "visible", e.visible);
    serialize_field(s, "pos", e.pos);
    serialize_field(s, "rot", e.rot);
    serialize_field(s, "scl", e.scl);
    serialize_field(s, "tint", e.tint);
    serialize_field(s, "sprite", e.sprite);
}

fn deserialize_fields_base_entity(Deserializer* d, Entity* e) -> bool {
    std::string_view key = deserialize_read_key(d);
    if (key == "enabled") {
        deserialize_value(d, e->enabled);
        return true;
    }
    if (key == "visible") {
        deserialize_value(d, e->visible);
        return true;
    }
    // @Note: Enums are a bit anoying to deserialize.
    if (key == "kind") {
        std::string str_Kind;
        deserialize_value(d, str_Kind);
        from_string(str_Kind, &e->kind);
        return true;
    }
    if (key == "pos") {
        deserialize_value(d, e->pos);
        return true;
    }
    if (key == "rot") {
        deserialize_value(d, e->rot);
        return true;
    }
    if (key == "scl") {
        deserialize_value(d, e->scl);
        return true;
    }
    if (key == "tint") {
        deserialize_value(d, e->tint);
        return true;
    }
    if (key == "sprite") {
        deserialize_value(d, e->sprite);
        return true;
    }
    return false;
}

#endif

// ===============================================================================
// @Region: Entity Storage
// ===============================================================================

// ===============================
// @Region: Declaration
// ===============================

#ifndef ENTITY_STORAGE_H
#define ENTITY_STORAGE_H

// Entity handle.
struct Entity_Handle {
    Entity_Kind kind = Entity_Kind_None;
    Array_Handle value;
};

#define EntityStorage(EntityType) \
    EntityType##_Storage

// Entity_Storage: Entity manager.
struct Entity_Storage {
    #define DeclareStorageVar(EntityType) \
        Fixed_Handle_Array<EntityType> EntityStorage(EntityType);
        
        ForEntityTypes(DeclareStorageVar)

    #undef DeclareStorageVar
};

fn entity_storage_init() -> void;
fn entity_storage_done() -> void;
fn entity_create(Entity_Kind kind) -> Entity_Handle;
fn entity_destroy(Entity_Handle handle) -> void; // @Pending: Save the entities to a cleanup list and wait till the frame ends.
fn entity_get(Entity_Handle handle) -> Entity*;
fn entity_pass(void (*update)(Entity*)) -> void;

#define EntityGet(EntityType, EntityHandle) \
    (EntityType*) entity_get(EntityHanle)

#endif

// ===============================
// @Region: Implementation
// ===============================

#ifdef ENTITY_IMPL

static Entity_Storage* world = nullptr;

fn entity_storage_init() -> void {

    if (world) {
        entity_storage_done();
    }
    
    world = new Entity_Storage();
}

fn entity_storage_done() -> void {
    if (!world) {
        return;
    }
    #define FreeStorage(EntityType) \
        reset(&world->EntityStorage(EntityType));
    
        ForEntityTypes(FreeStorage)
    #undef FreeStorage

    delete world;
    world = nullptr;
}

fn entity_create(Entity_Kind kind) -> Entity_Handle {
    switch (kind) {
        #define ChooseStorage(EntityType)                                      \
            case EntityKind(EntityType):                                       \
            {                                                                  \
                auto handle = append(&world->EntityStorage(EntityType));       \
                auto* derived = get(world->EntityStorage(EntityType), handle); \
                auto* base = (Entity*) derived;                                \
                base->kind = EntityKind(EntityType);                           \
                return { base->kind, handle };                                 \
            }

            ForEntityTypes(ChooseStorage)

        #undef ChooseStorage

        case Entity_Kind_None:
        default:
            return {};
    }
}

fn entity_destroy(Entity_Handle handle) -> void {
    switch (handle.kind) {
        #define ChooseStorage(EntityType)                                    \
            case EntityKind(EntityType):                                     \
                    remove(&world->EntityStorage(EntityType), handle.value); \

            ForEntityTypes(ChooseStorage)
        #undef ChooseStorage
        case Entity_Kind_None:
        default:
            return;
    }
}

fn entity_get(Entity_Handle handle) -> Entity* {
    static Entity def;
    switch (handle.kind) {
        #define ChooseStorage(EntityType)                                                   \
            case EntityKind(EntityType):                                                    \
                return (Entity*) get(world->EntityStorage(EntityType), handle.value);       \
                
                ForEntityTypes(ChooseStorage)
        
        #undef ChooseStorage
            case Entity_Kind_None:
            default:                                                
                return &def;           
    }
}

fn entity_pass(void (*update)(Entity*)) -> void {
    #define DoPass(EntityType)                                       \
        for(EntityType& entity: world->EntityStorage(EntityType)) {  \
            auto* base = (Entity*) &entity;                          \
            if (base->enabled) {                                     \
                update(base);                                        \
            }                                                        \
        }

    ForEntityTypes(DoPass)
}

#endif