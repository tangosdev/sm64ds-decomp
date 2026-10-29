#ifndef ASYNC_JOB_H
#define ASYNC_JOB_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*AsyncJobCallback)(int argument);

/* Runs immediately when the worker is already occupied. Returns 1 when the
 * callback completed synchronously, or 0 when a worker was started.
 * stackBase is a target address; zero requests an allocated stack. */
int func_0201a244(AsyncJobCallback callback, int argument, int priority, int stackBase,
                  int stackSize);

/* These jobs ignore the worker argument. */
void func_0201a2f8(int unused);
void func_02034fbc(int unused);

#ifdef __cplusplus
}
#endif
#endif
