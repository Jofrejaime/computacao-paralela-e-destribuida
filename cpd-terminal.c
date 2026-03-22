#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "commandlinereader/commandlinereader.h"

#define MAX_ARGS_OPTIONAL 5
#define MAX_ARG_VECTOR (MAX_ARGS_OPTIONAL + 2)
#define COMMAND_BUFFER_SIZE 1024

typedef struct ProcessInfo {
  pid_t pid;
  time_t startTime;
  time_t endTime;
  int status;
  int finished;
  struct ProcessInfo *next;
} ProcessInfo;

static ProcessInfo *processListHead = NULL;
static int numChildren = 0;
static int exitRequested = 0;
static pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;

static int status_to_exit_code(int status) {
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    return 128 + WTERMSIG(status);
  }
  return -1;
}

static void add_process_record(pid_t pid, time_t startTime) {
  ProcessInfo *node = (ProcessInfo *)malloc(sizeof(ProcessInfo));
  if (node == NULL) {
    fprintf(stderr, "malloc failed while storing process %d\n", (int)pid);
    return;
  }

  node->pid = pid;
  node->startTime = startTime;
  node->endTime = 0;
  node->status = 0;
  node->finished = 0;
  node->next = processListHead;
  processListHead = node;
}

static int update_terminated_process(pid_t pid, int status, time_t endTime) {
  ProcessInfo *it = processListHead;
  while (it != NULL) {
    if (it->pid == pid) {
      it->status = status;
      it->endTime = endTime;
      it->finished = 1;
      return 1;
    }
    it = it->next;
  }
  return 0;
}

static void print_final_report(void) {
  ProcessInfo *it = processListHead;
  printf("===== Process Report =====\n");
  while (it != NULL) {
    long elapsed = 0;
    int exitCode = -1;

    if (it->finished) {
      elapsed = (long)(it->endTime - it->startTime);
      exitCode = status_to_exit_code(it->status);
    }

    printf("pid=%d status=%d exec_time=%ld\n", (int)it->pid, exitCode, elapsed);
    it = it->next;
  }
}

static void free_process_list(void) {
  ProcessInfo *it = processListHead;
  while (it != NULL) {
    ProcessInfo *next = it->next;
    free(it);
    it = next;
  }
  processListHead = NULL;
}

static void *monitor_thread_fn(void *arg) {
  (void)arg;

  while (1) {
    int localNumChildren;
    int localExitRequested;

    pthread_mutex_lock(&stateMutex);
    localNumChildren = numChildren;
    localExitRequested = exitRequested;
    pthread_mutex_unlock(&stateMutex);

    if (localNumChildren == 0) {
      if (localExitRequested) {
        break;
      }
      sleep(1);
      continue;
    }

    {
      int status;
      pid_t childPid = wait(&status);
      if (childPid < 0) {
        if (errno == EINTR) {
          continue;
        }
        if (errno == ECHILD) {
          sleep(1);
          continue;
        }
        perror("wait failed in monitor thread");
        sleep(1);
        continue;
      }

      pthread_mutex_lock(&stateMutex);
      if (!update_terminated_process(childPid, status, time(NULL))) {
        fprintf(stderr, "warning: finished child %d not found in shared list\n", (int)childPid);
      }
      if (numChildren > 0) {
        numChildren--;
      }
      pthread_mutex_unlock(&stateMutex);
    }
  }

  return NULL;
}

int main(void) {
  pthread_t monitorThread;
  char buffer[COMMAND_BUFFER_SIZE];
  char *args[MAX_ARG_VECTOR];

  if (pthread_create(&monitorThread, NULL, monitor_thread_fn, NULL) != 0) {
    perror("pthread_create failed");
    return EXIT_FAILURE;
  }

  while (1) {
    int argc = readLineArguments(args, MAX_ARG_VECTOR, buffer, COMMAND_BUFFER_SIZE);

    if (argc < 0) {
      pthread_mutex_lock(&stateMutex);
      exitRequested = 1;
      pthread_mutex_unlock(&stateMutex);
      break;
    }

    if (argc == 0) {
      continue;
    }

    if (strcmp(args[0], "exit") == 0) {
      pthread_mutex_lock(&stateMutex);
      exitRequested = 1;
      pthread_mutex_unlock(&stateMutex);
      break;
    }

    if (argc - 1 > MAX_ARGS_OPTIONAL) {
      fprintf(stderr, "Too many arguments: maximum is %d optional args\n", MAX_ARGS_OPTIONAL);
      continue;
    }

    {
      pid_t pid = fork();
      if (pid < 0) {
        perror("fork failed");
        continue;
      }

      if (pid == 0) {
        execvp(args[0], args);
        perror("execvp failed");
        _exit(EXIT_FAILURE);
      }

      pthread_mutex_lock(&stateMutex);
      add_process_record(pid, time(NULL));
      numChildren++;
      pthread_mutex_unlock(&stateMutex);
    }
  }

  if (pthread_join(monitorThread, NULL) != 0) {
    perror("pthread_join failed");
    free_process_list();
    return EXIT_FAILURE;
  }

  pthread_mutex_lock(&stateMutex);
  print_final_report();
  pthread_mutex_unlock(&stateMutex);

  free_process_list();
  pthread_mutex_destroy(&stateMutex);
  return EXIT_SUCCESS;
}
