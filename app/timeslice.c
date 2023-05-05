#include "timeslice.h"

#define GET(ctx, bit) ((ctx)->flag & (bit))
#define SET(ctx, bit) ((ctx)->flag |= (bit))
#define CLR(ctx, bit) ((ctx)->flag &= ~(bit))

enum
{
    TIMESLICE_CTRL = 0x000F, // Register for control
    TIMESLICE_EXEC = 1 << 0, // Bit that the task needs to execute
    TIMESLICE_STAT = 0x00F0, // Register for status
    TIMESLICE_LOCK = 1 << 4, // Bit that the task has been locked
    TIMESLICE_TYPE = 0x0F00, // Register for type
    TIMESLICE_CRON = 1 << 8, // Bit for the cron task
    TIMESLICE_ONCE = 1 << 9, // Bit for the once task
};

#undef timeslice
static struct
{
    list_s service[1];
    timeslice_s *ctx;
    size_t counter;
} timeslice[1] = {{
    {LIST_INIT(*timeslice->service)},
    0,
    0,
}};

#define TIMESLICE(ctx) (ctx ? ctx : timeslice->ctx)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif /* __GNUC__ || __clang__ */

void timeslice_exec(void)
{
    list_s *node, *next;
    list_forsafe(node, next, timeslice->service)
    {
        timeslice->ctx = list_entry(node, timeslice_s, node);
        if (GET(timeslice->ctx, TIMESLICE_EXEC))
        {
            CLR(timeslice->ctx, TIMESLICE_EXEC);
            timeslice->ctx->exec(timeslice->ctx->argv);
            if (GET(timeslice->ctx, TIMESLICE_ONCE))
            {
                list_drop(timeslice->ctx->node);
                --timeslice->counter;
            }
        }
    }
}

void timeslice_tick(void)
{
    timeslice_s *ctx;
    list_s *node, *next;
    list_forsafe(node, next, timeslice->service)
    {
        ctx = list_entry(node, timeslice_s, node);
        if (ctx->timer && --ctx->timer == 0)
        {
            ctx->timer = ctx->slice;
            SET(ctx, TIMESLICE_EXEC);
        }
    }
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

void timeslice_cron(timeslice_s *const ctx, void (*const exec)(void *), void *const argv, size_t const slice)
{
    list_init(ctx->node);
    ctx->slice = slice;
    ctx->timer = slice;
    ctx->exec = exec;
    ctx->argv = argv ? argv : ctx;
    ctx->flag = TIMESLICE_CRON;
}

void timeslice_once(timeslice_s *const ctx, void (*const exec)(void *), void *const argv, size_t const delay)
{
    list_init(ctx->node);
    ctx->slice = delay;
    ctx->timer = delay;
    ctx->exec = exec;
    ctx->argv = argv ? argv : ctx;
    ctx->flag = TIMESLICE_ONCE;
}

void timeslice_set_exec(timeslice_s *const ctx, void (*const exec)(void *))
{
    TIMESLICE(ctx)->exec = exec;
}
void timeslice_set_argv(timeslice_s *ctx, void *const argv)
{
    ctx = TIMESLICE(ctx);
    ctx->argv = argv ? argv : ctx;
}
void timeslice_set_timer(timeslice_s *const ctx, size_t const timer)
{
    TIMESLICE(ctx)->timer = timer;
}
void timeslice_set_slice(timeslice_s *const ctx, size_t const slice)
{
    TIMESLICE(ctx)->slice = slice;
}

void timeslice_join(timeslice_s *ctx)
{
    ctx = TIMESLICE(ctx);
    if (list_null(ctx->node))
    {
        list_join(timeslice->service, ctx->node);
        ++timeslice->counter;
    }
}

void timeslice_drop(timeslice_s *ctx)
{
    ctx = TIMESLICE(ctx);
    if (list_used(ctx->node))
    {
        list_drop(ctx->node);
        --timeslice->counter;
    }
}

int timeslice_exist(timeslice_s const *const ctx)
{
    return list_used(TIMESLICE(ctx)->node);
}

size_t timeslice_timer(timeslice_s const *const ctx)
{
    return TIMESLICE(ctx)->timer;
}
size_t timeslice_slice(timeslice_s const *const ctx)
{
    return TIMESLICE(ctx)->slice;
}
size_t timeslice_count(void)
{
    return timeslice->counter;
}
