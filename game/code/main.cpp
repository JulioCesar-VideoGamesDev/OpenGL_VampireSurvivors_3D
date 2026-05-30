#include <iostream>
#include <cmath>
#include <random>

#include "app.h"
#include "draw.h"
#include "graphics.h"
#include "asset.h"
#include "entity.h"

struct Player : Entity {
    f32 speed = 10.f;
    u32 hp = 1;

    Vec3 dir = { 0.f, 0.f, 0.f };
    Vec3 lastDir = { 1.f, 0.f, 0.f };  // Última dirección válida

    f32 shootCooldown = 0.5f;
    f32 shootTimer = 0.f;

    AABB3D boxCollision3D{};
};

struct Bullet : Entity {
    f32 speed;
    Vec3 dir = { 0.f, 0.f, 0.f };
    f32 lifetime = 5;
    u32 damage = 1;

    AABB3D boxCollision3D{};
};

struct Enemy : Entity {
    f32 speed;
    u32 hp;
    Entity_Handle target_handle;
    Player* target;

    AABB3D boxCollision3D{};
};

#define ENTITY_IMPL
#include "entity.h"

// General Variables
f32 box_spin;

f32 enemySpawningRadius{ 50.f };

const u32 enemyPoolNumber{ 10 }; // Max amount of enemies at the same time.
f32 enemySpeed{ 8.f };

const u32 bulletPoolNumber{ 100 }; // Max amount of enemies at the same time.
f32 bulletSpeed{ 20.f };
f32 bulletLifetime{ 0.5f };

// Function to create multiple entities of the same type.
void entity_create_many(Entity_Kind kind, u32 count, Entity_Handle* out)
{
    for (u32 i = 0; i < count; i++)
    {
        out[i] = entity_create(kind);
    }
}

Vec2 GetRandomPointOnCircle(const Vec2& center, float radius)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Random angle between 0 and 2*PI
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.1415);
    float angle = angleDist(gen);

    Vec2 point;
    point.x = center.x + radius * std::cos(angle);
    point.y = center.y + radius * std::sin(angle);

    return point;
}

Enemy* updateEnemy(Enemy* e)
{
    if (e->enabled)
    {
        // Check overlap
        if (e->boxCollision3D.overlap(e->boxCollision3D, e->target->boxCollision3D))
        {
            e->enabled = false;
            //printf("NO");
            return e;
        }

        Vec3 delta = e->target->pos - e->pos;
        if (delta.lenght() > 0.0001f)
        {
            e->pos += delta.normalized() * e->speed * os_delta_time();
        }

        e->boxCollision3D = updateBoxCollisionPosition3D(e->pos, e->boxCollision3D);
    }
    else
    {
        Vec2 newPosition = GetRandomPointOnCircle({ e->target->pos.x, e->target->pos.y }, enemySpawningRadius);

        e->pos = { newPosition.x, 0, newPosition.y }; // If the enemy is disable, then enable it and place it in a random position of a circumference around the player. If not then move to the player.
        e->boxCollision3D = updateBoxCollisionPosition3D(e->pos, e->boxCollision3D);

        e->enabled = true;
    }
    return e;
}

void ShootBullet(Player* p, Entity_Handle* bulletHandle, u32 bulletPoolNumber)
{
    for (u32 i = 0; i < bulletPoolNumber; i++)
    {
        Bullet* b = EntityGet(Bullet, bulletHandle[i]);

        if (!b->enabled)
        {
            b->enabled = true;

            b->pos = p->pos;

            b->dir.x = p->lastDir.x;
            b->dir.z = p->lastDir.z;

            b->lifetime = 3.f;

            b->boxCollision3D =
                updateBoxCollisionPosition3D(
                    b->pos,
                    b->boxCollision3D
                );

            break;
        }
    }
}

Bullet* UpdateBullet(Bullet* b)
{
    if (!b->enabled) return b;

    b->pos += {b->dir.x * b->speed * os_delta_time(), 0, b->dir.z * b->speed * os_delta_time()};

    b->lifetime -= os_delta_time();

    b->boxCollision3D = updateBoxCollisionPosition3D(b->pos, b->boxCollision3D);

    if (b->lifetime <= 0.f)
    {
        b->enabled = false;
    }

    return b;
}

void CheckBulletEnemyCollision(
    Entity_Handle* bulletsHandle,
    u32 bulletCount,
    Entity_Handle* enemiesHandle,
    u32 enemyCount)
{
    for (u32 i = 0; i < bulletCount; i++)
    {
        Bullet* b = EntityGet(Bullet, bulletsHandle[i]);

        if (!b->enabled)
            continue;

        for (u32 j = 0; j < enemyCount; j++)
        {
            Enemy* e = EntityGet(Enemy, enemiesHandle[j]);

            if (!e->enabled)
                continue;

            if (AABB3D::overlap(
                b->boxCollision3D,
                e->boxCollision3D))
            {
                b->enabled = false;
                e->enabled = false;

                break;
            }
        }
    }
}

fn main() -> s32 {

    App_Desc desc;
    desc.window.title = L"Survive 3D";
    app_init(desc);
    draw_init();

    Texture monk_run_texture;
    {
        Texture_Def def;
        def.kind = Texture_Kind_Multiple;
        def.subtex_size = 192;
        def.filename = "Monk.png";
        texture_init(&monk_run_texture, def);
    }

    s32 frame_count = monk_run_texture.subtexs.count;
    s32 anim_frames = 12;
    s32 curr_frame = 0;
    s32 last_frame = frame_count - 1;
    f32 frame_duration = 1.0f / (f32)anim_frames;
    f32 frame_timer = 0.0f;

    Mesh box_stack;
    Asset_Handle mesh_shader = asset_create(Asset_Kind_Shader);
    Shader* mesh_shader_data = (Shader*) asset_get(mesh_shader);
    Shader_Def mesh_shader_def {"shader_mesh_lit.glsl"};
    shader_init(mesh_shader_data, mesh_shader_def);

    mesh_init(&box_stack, "box_stack/box_stack.obj", mesh_shader);

    set_depth_test_enabled();

    float box_spin = 0.f;

    entity_storage_init();

    // Create Player
    Entity_Handle playerHandle[1];
    playerHandle[0] = entity_create(Entity_Kind_Player);

    Player* p = EntityGet(Player, playerHandle[0]);
    p->enabled = true;
    p->tex = &monk_run_texture;
    p->tint = Color.White;
    p->frame_count = monk_run_texture.subtexs.count;
    p->pos = { 0.f, 0.f, 0.f };
    p->scl = { 1.f, 1.f, 1.f };

    p->boxCollision3D = setBoxCollisionSize3D(p->boxCollision3D, 1.f, 1.f, 1.f /* I should not be  hard coding this but I will leave it like this for now */, p->scl);
    p->boxCollision3D = updateBoxCollisionPosition3D(p->pos, p->boxCollision3D);

    // Create Bullets
    Entity_Handle bulletHandle[bulletPoolNumber];
    entity_create_many(Entity_Kind_Bullet, bulletPoolNumber, bulletHandle);

    for (int i = 0; i < bulletPoolNumber; i++)
    {

        Bullet* b = EntityGet(Bullet, bulletHandle[i]);

        b->enabled = false;

        b->tex = &monk_run_texture;

        b->tint = Color.Blue;

        b->frame_count = monk_run_texture.subtexs.count;

        b->frame_duration = frame_duration;

        b->pos = { p->pos.x, 0, p->pos.y }; // If the enemy is disable, then enable it and place it in a random position of a circumference around the player. If not then move to the player.

        b->scl = Vec3{ 1.f, 1.f, 1.f };

        b->boxCollision3D = setBoxCollisionSize3D(b->boxCollision3D, 1.f, 1.f, 1.f, b->scl);
        b->boxCollision3D = updateBoxCollisionPosition3D(b->pos, b->boxCollision3D);

        b->speed = bulletSpeed;
    }

    // Create Enemies
    Entity_Handle enemiesHandle[enemyPoolNumber];
    entity_create_many(Entity_Kind_Enemy, enemyPoolNumber, enemiesHandle);

    for (int i = 0; i < enemyPoolNumber; i++)
    {

        Enemy* e = EntityGet(Enemy, enemiesHandle[i]);

        e->enabled = true;

        e->tex = &monk_run_texture;

        e->tint = Color.Red;

        e->frame_count = monk_run_texture.subtexs.count;

        e->frame_duration = frame_duration;

        e->target_handle = playerHandle[0];

        e->target = p;

        Vec2 newPosition{};

        newPosition = GetRandomPointOnCircle({ e->target->pos.x, e->target->pos.y }, enemySpawningRadius);

        e->pos = { newPosition.x, 0, newPosition.y }; // If the enemy is disable, then enable it and place it in a random position of a circumference around the player. If not then move to the player.

        e->scl = Vec3{ 1.f, 1.f, 1.f };

        e->boxCollision3D = setBoxCollisionSize3D(e->boxCollision3D, 1.f, 1.f, 1.f, e->scl);
        e->boxCollision3D = updateBoxCollisionPosition3D(e->pos, e->boxCollision3D);

        e->speed = enemySpeed;
    }

    while (app_running())
    {
        box_spin += 30 * os_delta_time();
        frame_timer += os_delta_time();

        while (frame_timer >= frame_duration) { // Instead of having one current frame, update the current frame of all entities.
            frame_timer -= frame_duration;
            curr_frame++;
            if (curr_frame >= frame_count) {
                curr_frame = 0;
            }
        }

        p->shootTimer -= os_delta_time();

        if (p->shootTimer < 0.f)
        {
            p->shootTimer = 0.f;
        }

        draw_update(os_delta_time()); // Update "camera" position.

        // Start to draw
        clear_back_buffer(); // We clear the back buffer to draw in it.

        // UpdatePlayer
        os_set_cursor_mode(Cursor_Mode::Hidden);
        if (os_key_down('W'))
        {
            p->pos.z -= p->speed * os_delta_time();
        }
        if (os_key_down('S'))
        {
            p->pos.z += p->speed * os_delta_time();
        }
        if (os_key_down('D'))
        {
            p->pos.x += p->speed * os_delta_time();
        }
        if (os_key_down('A'))
        {
            p->pos.x -= p->speed * os_delta_time();
        }

        p->dir = { 0.f, 0.f, 0.f };

        if (os_key_down('I'))
        {
            p->dir.z = -1;
        }
        if (os_key_down('K'))
        {
            p->dir.z = 1;
        }
        if (os_key_down('L'))
        {
            p->dir.x = 1;
        }
        if (os_key_down('J'))
        {
            p->dir.x = -1;
        }

        if (p->dir.lenght() > 0.001f && p->shootTimer <= 0)
        {
            p->dir = p->dir.normalized();

            p->lastDir = p->dir;

            ShootBullet(p, bulletHandle, bulletPoolNumber);
            p->shootTimer = p->shootCooldown;
        }

        draw_mesh(&box_stack, Mat4::transform(p->pos, Vec3(0.0f, F32.to_radians(box_spin), 0.0f), p->scl));
        //draw_sprite(p->tex, curr_frame, p->tint, Mat4::transform(p->pos, p->rot, p->scl));
        p->boxCollision3D = updateBoxCollisionPosition3D(p->pos, p->boxCollision3D);


        // UpdateBullets
        for (int i = 0; i < bulletPoolNumber; i++)
        {
            Bullet* b = EntityGet(Bullet, bulletHandle[i]);

            b = UpdateBullet(b);

            if (b->enabled)
            {
                draw_mesh(&box_stack, Mat4::transform(b->pos, Vec3(0.0f, F32.to_radians(box_spin), 0.0f), b->scl));
            }
        }

        // UpdateEnemies
        for (int i = 0; i < enemyPoolNumber; i++)
        {
            Enemy* e = EntityGet(Enemy, enemiesHandle[i]);

            e = updateEnemy(e);

            if (e->enabled)
            {
                draw_mesh(&box_stack, Mat4::transform(e->pos, Vec3(0.0f, F32.to_radians(box_spin), 0.0f), e->scl));
            }
        }

        CheckBulletEnemyCollision(
            bulletHandle,
            bulletPoolNumber,
            enemiesHandle,
            enemyPoolNumber
        );

        // 2D
        //draw_sprite(&monk_run_texture, curr_frame, Color.White, Mat4::transform(F32.Zero, F32.Zero, Vec3(F32.One) * 3.0f));

        // 3D
        //box_spin += 30 * os_delta_time();
        //draw_mesh(&box_stack, Mat4::transform(Vec3(F32.Front) * 20.f, Vec3(0.0f, F32.to_radians(box_spin), 0.0f), Vec3(F32.One)));
        os_swap_buffers(); // Now that we drew everything we need in the back buffer we swap it with the front one to show it.
    }

    texture_done(&monk_run_texture);
    asset_free_all();
    draw_done();
    app_done();
}