#include "co.h"

static void* stack_backup;
static int co_cnt = 0;
static jmp_buf start_buf;
static jmp_buf wait_buf;

struct co* head = NULL;
struct co* current = NULL;

void co_init() {
  head = NULL;
  current = NULL;
}

void co_gc() {
  if (!head) return;
  if (head->state == ST_R) {
    struct co* next = head->next;
    free(head);
    head = next;
  }
  struct co* cp = head;
  while (cp->next) {
    if (cp->next->state == ST_R) {
      struct co* next = cp->next->next;
      free(cp->next);
      cp->next = next;
    }
    cp = cp->next;
  }
  co_print();
}

struct co* co_create(const char *name, func_t func, void* arg) {
  struct co* ret = (struct co*) malloc(sizeof(struct co));
  ret->pid = ++co_cnt;
  ret->state = ST_I; // init state
  strncpy(ret->name, name, sizeof(ret->name));
  ret->func = func;
  ret->arg = arg;
  ret->next = NULL;
  ret->stack_ptr = (void*) ret->stack + SZ_STACK;
  if (head) {
    struct co* cp = head;
    while (cp->next) {
      Log("cp => %p", cp);
      cp = cp->next;
    }
    cp->next = ret;
  } else {
    head = ret;
  }
  co_print();
  return ret;
}

struct co* co_start(const char* name, func_t func, void* arg) {
  Log("%s START!", name);
  current = co_create(name, func, arg);
  if (!setjmp(start_buf)) {
    stackEX(current->stack_ptr, stack_backup);
    current->func(current->arg);
    longjmp(wait_buf, 1);
  } else {
    Log("init finished");
  }
  /* continue from co_yield */
  return current;
}

void co_yield() {
  if (!setjmp(current->buf)) {
    if (current->state == ST_I) {
      stackEX(stack_backup, current->stack_ptr);
      current->state = ST_S;
      longjmp(start_buf, 1);
      /* go back to co_start */
    } else {
      struct co* next = current->next ? current->next : head;
      current->state = ST_S;
      stackEX(next->stack_ptr, current->stack_ptr);
      current = next;
      longjmp(current->buf, 1);
    }
  } else {
    stackEX(current->stack_ptr, stack_backup);
    current->state = ST_R;
    Log("return to %s", current->name);
  }
}

void co_wait(struct co *thd) {
  Log("co_wait for %s!", thd->name);
  while (thd->state != ST_R) {
    if (!setjmp(wait_buf)) {
      current = thd;
      longjmp(current->buf, 1);
      /* will continue in co_start */
    }
    Log("One thread is finished!!");
    if (current == thd) break;
    else {
      Log("bad luck!");
      /* one thread finished, but not thd */
      co_gc();
    }
  }
  /* thd is finished */
  co_gc();
  return;
}

void co_print() {
  Log("=====THREADS=====");
  for (struct co* cp = head; cp != NULL; cp = cp->next) {
    Log("%p => %d: [%s] %d", cp, cp->pid, cp->name, cp->state);
  }
  Log("=================");
}
