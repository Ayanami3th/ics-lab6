/*
 *csim.c-使用C编写一个Cache模拟器，它可以处理来自Valgrind的跟踪和输出统计
 *息，如命中、未命中和逐出的次数。更换政策是LRU。
 * 设计和假设:
 *  1. 每个加载/存储最多可导致一个缓存未命中。（最大请求是8个字节。）
 *  2. 忽略指令负载（I），因为我们有兴趣评估trace.c内容中数据存储性能。
 *  3.
 *数据修改（M）被视为加载，然后存储到同一地址。因此，M操作可能导致两次缓存命中，或者一次未命中和一次命中，外加一次可能的逐出。
 * 使用函数printSummary() 打印输出，输出hits, misses and evictions
 *的数，这对结果评估很重要
 */
#include "cachelab.h"
#include <getopt.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//                    请在此处添加代码
//****************************Begin*********************
typedef unsigned long long addr_t;
typedef unsigned long long uint64;

typedef struct cache_line {
  char valid;
  uint64 lru;
  addr_t tag;
} cache_line_t;

typedef cache_line_t *cache_group_t;
typedef cache_group_t *cache_t;

int miss_count, hit_count, eviction_count;
uint64 addr_mask;

// print help doc
void printHelpUsage(char *file_name) {
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", file_name);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n");
  printf("\nExamples:\n");
  printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", file_name);
  printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", file_name);
  exit(0);
}

void getData(cache_t cache, addr_t addr, int s, int E, int b, int verbosity) {
  addr_t cache_index = (addr >> b) & addr_mask;
  addr_t cache_tag = addr >> (s + b);

  cache_group_t cache_group = cache[cache_index];
  for (int i = 0; i < E; i++) {
    if (cache_group[i].tag == cache_tag) {
      if (cache_group[i].valid) {
        hit_count++;
        cache_group[i].lru = 0;
        if (verbosity)
          printf("hit ");
        return;
      }
    }
  }

  for (int i = 0; i < E; i++) {
    if (cache_group[i].valid == 0) {
      miss_count++;
      cache_group[i].tag = cache_tag;
      cache_group[i].valid = 1;
      cache_group[i].lru = 0;
      if (verbosity)
        printf("miss ");
      return;
    }
  }

  miss_count++;
  eviction_count++;
  int eviction_index = 0;
  for (int i = 0; i < E; i++)
    if (cache_group[i].lru > cache_group[eviction_index].lru)
      eviction_index = i;
  cache_group[eviction_index].tag = cache_tag;
  cache_group[eviction_index].valid = 1;
  cache_group[eviction_index].lru = 0;
  if (verbosity)
    printf("eviction ");
  return;
}

void excuteTrace(int s, int E, int b, char *t, int verbosity) {
  FILE *t_fp = fopen(t, "r");

  addr_mask = (addr_t)((1 << s) - 1);

  int S = (unsigned int)1 << s;
  cache_t cache = (cache_group_t*)malloc(sizeof(cache_group_t) * S);
  for (int i = 0; i < S; i++) {
    cache[i] = (cache_line_t*)malloc(sizeof(cache_line_t) * E);
    for (int j = 0; j < E; j++) {
      cache[i][j].valid = 0;
      cache[i][j].tag = 0;
      cache[i][j].lru = 0;
    }
  }

  char operation;
  unsigned int len;
  uint64 addr;
  while (fscanf(t_fp, " %c %llx,%u\n", &operation, &addr, &len) > 0) {
    switch (operation) {
    case 'L':
      if (verbosity)
        printf("%c %llx,%u ", operation, addr, len);
      getData(cache, addr, s, E, b, verbosity);
      if(verbosity) printf("\n");
      break;
    case 'M':
      if (verbosity)
        printf("%c %llxu,%u ", operation, addr, len);
      getData(cache, addr, s, E, b, verbosity);
      getData(cache, addr, s, E, b, verbosity);
      if(verbosity) printf("\n");
      break;
    case 'S':
      if (verbosity)
        printf("%c %llx,%u ", operation, addr, len);
      getData(cache, addr, s, E, b, verbosity);
      if(verbosity) printf("\n");
      break;
    default:
      continue;
    }

    for (int i = 0; i < S; i++)
      for (int j = 0; j < E; j++)
        if (cache[i][j].valid)
          cache[i][j].lru++;
  }

  fclose(t_fp);
  for (int i = 0; i < S; i++)
    free(cache[i]);
  free(cache);
}

int main(int argc, char **argv) {
  int verbosity = 0;
  int s = 0, E = 0, b = 0;
  char *t = NULL;

  char c;
  while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
    switch (c) {
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      t = optarg;
      break;
    case 'v':
      verbosity = 1;
      break;
    case 'h':
      printHelpUsage(argv[0]);
      exit(0);
    default:
      printHelpUsage(argv[0]);
      exit(-1);
    }
  }

  if (s == 0 || E == 0 || b == 0 || t == NULL) {
    printHelpUsage(argv[0]);
    exit(-1);
  }
  
  excuteTrace(s, E, b, t, verbosity);

  printSummary(hit_count, miss_count,
               eviction_count); //输出hit、miss和evictions数量统计
  return 0;
}
//****************************End**********************#