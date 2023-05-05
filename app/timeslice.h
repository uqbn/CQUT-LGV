/*!
 @file timeslice.h
 @brief cooperative timeslice scheduler implementation.
*/

#ifndef TIMESLICE_H
#define TIMESLICE_H

#include <stddef.h>

// clang-format off
#define LIST_INIT(node) {&(node), &(node)}
// clang-format on

/*!
 @brief Instance structure for circular doubly linked list
*/
typedef struct list_s
{
    struct list_s *next, *prev;
} list_s;

/*!
 @brief Get the struct for this entry
 @param ptr the &list_s pointer
 @param type the type of the struct this is embedded in
 @param member the name of the list_s within the struct
*/
#define list_entry(ptr, type, member) ((type *)((char *)(ptr)-offsetof(type, member)))

/*!
 @brief Iterate over a list
 @param ptr the &list_s to use as a loop counter
 @param ctx points to circular doubly linked list
*/
#define list_foreach(ptr, ctx) \
    for ((ptr) = (ctx)->next; (ptr) != (ctx); (ptr) = (ptr)->next)

/*!
 @brief Iterate over a list safe against removal of list entry
 @param ptr the &list_s to use as a loop counter
 @param pre another &list_s to use as temporary storage
 @param ctx points to circular doubly linked list
*/
#define list_forsafe(ptr, pre, ctx)                \
    for ((ptr) = (ctx)->next, (pre) = (ptr)->next; \
         (ptr) != (ctx) && (ptr) != (ptr)->next;   \
         (ptr) = (pre), (pre) = (ptr)->next)

/*!
 @brief Testing whether a list is null
 @param[in] ctx points to circular doubly linked list
 @return int bool
  @retval 0 non-null
  @retval 1 null
*/
static inline int list_null(list_s const *const ctx) { return ctx->next == ctx || ctx->next->prev != ctx; }
/*!
 @brief Testing whether a list is used
 @param[in] ctx points to circular doubly linked list
 @return int bool
  @retval 0 unused
  @retval 1 used
*/
static inline int list_used(list_s const *const ctx) { return ctx->next != ctx && ctx->next->prev == ctx; }

/*!
 @brief initialize for circular doubly linked list
 @param[in,out] ctx points to circular doubly linked list
*/
static inline void list_init(list_s *const ctx) { ctx->prev = ctx->next = ctx; }

/*!
 @brief Link head node and tail node node
 @param[in,out] head The head node of a list
 @param[in,out] tail The tail node of a list
*/
static inline void list_link(list_s *const head, list_s *const tail)
{
    head->next = tail;
    tail->prev = head;
}

/*!
 @brief Insert a node to a list backward
 @param[in,out] ctx points to circular doubly linked list
 @param[in] node a list node
*/
static inline void list_join(list_s *const ctx, list_s *const node)
{
    list_link(ctx->prev, node);
    list_link(node, ctx);
}

/*!
 @brief Delete a node from a list
 @param[in] node a list node
*/
static inline void list_drop(list_s *const node)
{
    list_link(node->prev, node->next);
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

/*!
 @brief Instance structure for timeslice
*/
typedef struct timeslice_s
{
    list_s node[1];
    size_t slice;
    size_t timer;
    void (*exec)(void *);
    void *argv;
    int flag;
} timeslice_s;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 @brief A function to execute before the main loop
*/
void timeslice_init(void);
/*!
 @brief A function that need to be executed in the main loop
*/
void timeslice_exec(void);
/*!
 @brief A function that need to be executed in the tick timer
*/
void timeslice_tick(void);

#define timeslice() for (timeslice_init();; timeslice_exec())

/*!
 @brief Initialize as a cron task
 @param[in,out] ctx points to an instance of timeslice
 @param[in] exec A function that needs to be executed
 @param[in] argv Arguments to the executed function
 @param[in] slice The length of the time slice
*/
void timeslice_cron(timeslice_s *ctx, void (*exec)(void *), void *argv, size_t slice);
/*!
 @brief Initialize as a once task
 @param[in,out] ctx points to an instance of timeslice
 @param[in] exec A function that needs to be executed
 @param[in] argv Arguments to the executed function
 @param[in] delay The length of delayed execution
*/
void timeslice_once(timeslice_s *ctx, void (*exec)(void *), void *argv, size_t delay);

/*!
 @brief Set the execution function
 @param[in,out] ctx points to an instance of timeslice
 @param[in] exec A function that needs to be executed
*/
void timeslice_set_exec(timeslice_s *ctx, void (*exec)(void *));
/*!
 @brief Set the arguments
 @param[in,out] ctx points to an instance of timeslice
 @param[in] argv Arguments to the executed function
*/
void timeslice_set_argv(timeslice_s *ctx, void *argv);
/*!
 @brief Set the timer
 @param[in,out] ctx points to an instance of timeslice
 @param[in] timer Timer value
*/
void timeslice_set_timer(timeslice_s *ctx, size_t timer);
/*!
 @brief Set the slice
 @param[in,out] ctx points to an instance of timeslice
 @param[in] slice Slice value
*/
void timeslice_set_slice(timeslice_s *ctx, size_t slice);

/*!
 @brief Join a task to the time slice list
 @param[in,out] ctx points to an instance of timeslice
*/
void timeslice_join(timeslice_s *ctx);
/*!
 @brief Drop a task from the time slice list
 @param[in,out] ctx points to an instance of timeslice
*/
void timeslice_drop(timeslice_s *ctx);

/*!
 @brief Testing whether a task is in the time slice list
 @param[in] ctx points to an instance of timeslice
*/
int timeslice_exist(timeslice_s const *ctx);

/*!
 @brief Get the timer value for a task
 @param[in] ctx points to an instance of timeslice
 @return size_t The timer value
*/
size_t timeslice_timer(timeslice_s const *ctx);
/*!
 @brief Get the slice value for a task
 @param[in] ctx points to an instance of timeslice
 @return size_t The slice value
*/
size_t timeslice_slice(timeslice_s const *ctx);
/*!
 @brief Get the count of tasks in the time slice list
 @return size_t The count of tasks
*/
size_t timeslice_count(void);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* timeslice.h */
