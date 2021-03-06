/*
	Copyright (C) 2014-2020 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
	To compile use:

	g++ -O3 -fpermissive bench.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include "quadsort.h"

//#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC(const void *a, const void *b);

typedef void SRTFUNC(void *array, size_t nmemb, size_t size, CMPFUNC *ignore);

// benchmarking utilities

// Must prevent inlining so the benchmark is fair.
// Remove __attribute__ ((noinline)) and counter++ for full throttle.

unsigned int counter;

__attribute__((noinline)) int cmp_int(const void *a, const void *b)
{
  counter++;

  return *(int *)a - *(int *)b;
}

__attribute__((noinline)) int cmp_stable(const void *a, const void *b)
{
  counter++;

  return *(int *)a / 1000 - *(int *)b / 1000;
}

__attribute__((noinline)) int cmp_long(const void *a, const void *b)
{
  counter++;

  if (*(long long *)a < *(long long *)b)
  {
    return -1;
  }

  return *(long long *)a > *(long long *)b;
}

int cmp_str(const void *a, const void *b)
{
  return strcmp(*(const char **)a, *(const char **)b);
}

int cmp_float(const void *a, const void *b)
{
  return *(float *)a - *(float *)b;
}

long long utime()
{
  struct timeval now_time;

  gettimeofday(&now_time, NULL);

  return now_time.tv_sec * 1000000LL + now_time.tv_usec;
}

void test_sort(void *array, void *unsorted, void *valid, int minimum, int maximum, int samples, int repetitions, SRTFUNC *srt, const char *name, const char *desc, size_t size, CMPFUNC *cmpf)
{
  double comps;
  long long start, end, total, best, average;
  size_t rep, sam, max;
  long long *ptla = array, *ptlv = valid;
  int *pta = array, *ptv = valid, cnt;

  best = average = comps = start = 0;

  if (maximum == 10 && size == sizeof(int))
  {
    memcpy(array, unsorted, maximum * size);

    printf("\e[1;32m in: \e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6], pta[7], pta[8], pta[9]);
  }

  for (sam = 0; sam < samples; sam++)
  {
    total = 0;

    max = minimum;

    if (repetitions > 100)
    {
      start = utime();
    }

    for (rep = 0; rep < repetitions; rep++)
    {
      memcpy(array, unsorted, max * size);

      counter = 0;

      if (repetitions <= 100)
      {
        start = utime();
      }

      switch (*name)
      {
      case 'q':
        if (name[1] == 'u')
        {
          quadsort(array, max, size, cmpf);
        }
        else
        {
          qsort(array, max, size, cmpf);
        }
        break;

      default:
        return;
      }

      if (repetitions <= 100)
      {
        end = utime();

        total += end - start;
      }
      comps += counter;

      if (minimum < maximum)
      {
        max++;

        if (max > maximum)
        {
          max = minimum;
        }
      }
    }

    if (repetitions > 100)
    {
      end = utime();

      total = end - start;
    }

    if (!best || total < best)
    {
      best = total;
    }
    average += total;
  }

  if (maximum == 10 && size == sizeof(int))
  {
    printf("\e[1;32mout: \e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6], pta[7], pta[8], pta[9]);
  }

  if (repetitions == 0)
  {
    return;
  }

  comps /= samples * repetitions;
  average /= samples;

  if (cmpf == cmp_stable)
  {
    for (cnt = 1; cnt < maximum; cnt++)
    {
      if (pta[cnt - 1] > pta[cnt])
      {
        printf("|%10s | %8d |  i%d | %f | %f | %11.1f | %16s |\n", name, maximum, (int)size * 8, best / 1000000.0, average / 1000000.0, comps, "unstable");

        return;
      }
    }
  }

  if (!strcmp(name, "quadsort"))
  {
    if (!strcmp(desc, "random order") || !strcmp(desc, "random 1-4"))
    {
      printf("%s\n", "|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |");
      printf("%s\n", "| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |");
    }
    else
    {
      printf("%s\n", "|           |          |      |          |          |             |                  |");
    }
  }

  if (counter)
  {
    printf("|%10s | %8d |  i%d | %f | %f | %11.1f | %16s |\n", name, maximum, (int)size * 8, best / 1000000.0, average / 1000000.0, comps, desc);
  }
  else
  {
    printf("|%10s | %8d |  i%d | %f | %f | %11s | %16s |\n", name, maximum, (int)size * 8, best / 1000000.0, average / 1000000.0, "     ", desc);
  }

  if (minimum != maximum)
  {
    return;
  }

  for (cnt = 1; cnt < maximum; cnt++)
  {
    if (size == sizeof(int))
    {
      if (pta[cnt - 1] > pta[cnt])
      {
        printf("%17s: not properly sorted at index %d. (%d vs %d\n", name, cnt, pta[cnt - 1], pta[cnt]);
        break;
      }
    }
    else if (size == sizeof(long long))
    {
      if (ptla[cnt - 1] > ptla[cnt])
      {
        printf("%17s: not properly sorted at index %d. (%016lld vs %016lld\n", name, cnt, ptla[cnt - 1], ptla[cnt]);
        break;
      }
    }
  }

  for (cnt = 1; cnt < maximum; cnt++)
  {
    if (size == sizeof(int))
    {
      if (pta[cnt] != ptv[cnt])
      {
        printf("         validate: array[%d] != valid[%d]. (%d vs %d\n", cnt, cnt, pta[cnt], ptv[cnt]);
        break;
      }
    }
    else if (size == sizeof(long long))
    {
      if (ptla[cnt] != ptlv[cnt])
      {
        printf("         validate: array[%d] != valid[%d]. (%lld vs %lld\n", cnt, cnt, ptla[cnt], ptlv[cnt]);
        break;
      }
    }
  }
}

int validate()
{
  int seed = time(NULL);
  int cnt, val, max = 2000000;

  int *a_array, *r_array, *v_array;

  srand(seed);

  a_array = (int *)malloc(max * sizeof(int));
  r_array = (int *)malloc(max * sizeof(int));
  v_array = (int *)malloc(max * sizeof(int));

  for (cnt = 0; cnt < max; cnt++)
  {
    r_array[cnt] = rand();
  }

  for (cnt = 1; cnt < 100; cnt++)
  {
    memcpy(a_array, r_array, max * sizeof(int));
    memcpy(v_array, r_array, max * sizeof(int));

    quadsort(a_array, cnt, sizeof(int), cmp_int);
    qsort(v_array, cnt, sizeof(int), cmp_int);

    for (val = 0; val < cnt; val++)
    {
      if (val && v_array[val - 1] > v_array[val])
      {
        printf("\e[1;31mvalidate rand: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
        return 0;
      }

      if (a_array[val] != v_array[val])
      {
        printf("\e[1;31mvalidate rand: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
        return 0;
      }
    }
  }

  // ascending saw

  for (cnt = 0; cnt < 1000; cnt++)
  {
    r_array[cnt] = rand();
  }

  quadsort(r_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
  quadsort(r_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
  quadsort(r_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
  quadsort(r_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);

  for (cnt = 1; cnt < 1000; cnt += 7)
  {
    memcpy(a_array, r_array, max * sizeof(int));
    memcpy(v_array, r_array, max * sizeof(int));

    quadsort(a_array, cnt, sizeof(int), cmp_int);
    qsort(v_array, cnt, sizeof(int), cmp_int);

    for (val = 0; val < cnt; val++)
    {
      if (val && v_array[val - 1] > v_array[val])
      {
        printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
        return 0;
      }

      if (a_array[val] != v_array[val])
      {
        printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
        return 0;
      }
    }
  }

  // descending saw

  for (cnt = 0; cnt < 1000; cnt++)
  {
    r_array[cnt] = (max - cnt - 1) % 100000;
  }

  for (cnt = 1; cnt < 1000; cnt += 7)
  {
    memcpy(a_array, r_array, max * sizeof(int));
    memcpy(v_array, r_array, max * sizeof(int));

    quadsort(a_array, cnt, sizeof(int), cmp_int);
    qsort(v_array, cnt, sizeof(int), cmp_int);

    for (val = 0; val < cnt; val++)
    {
      if (val && v_array[val - 1] > v_array[val])
      {
        printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
        return 0;
      }

      if (a_array[val] != v_array[val])
      {
        printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
        return 0;
      }
    }
  }

  // random tail

  for (cnt = 0; cnt < max * 3 / 4; cnt++)
  {
    r_array[cnt] = cnt;
  }

  for (cnt = max * 3 / 4; cnt < max; cnt++)
  {
    r_array[cnt] = rand();
  }

  for (cnt = 1; cnt < 1000; cnt += 7)
  {
    memcpy(a_array, r_array, max * sizeof(int));
    memcpy(v_array, r_array, max * sizeof(int));

    quadsort(a_array, cnt, sizeof(int), cmp_int);
    qsort(v_array, cnt, sizeof(int), cmp_int);

    for (val = 0; val < cnt; val++)
    {
      if (val && v_array[val - 1] > v_array[val])
      {
        printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
        return 0;
      }

      if (a_array[val] != v_array[val])
      {
        printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
        return 0;
      }
    }
  }

  free(a_array);
  free(r_array);
  free(v_array);

  return 1;
}

int main(int argc, char **argv)
{
  int max = 10000;
  int samples = 10;
  int repetitions = 1;
  long long *la_array, *lr_array, *lv_array;
  int *a_array, *r_array, *v_array;
  int cnt, rnd;

  if (argc >= 1 && argv[1] && *argv[1])
  {
    max = atoi(argv[1]);
  }

  if (argc >= 2 && argv[2] && *argv[2])
  {
    samples = atoi(argv[2]);
  }

  if (argc >= 3 && argv[3] && *argv[3])
  {
    repetitions = atoi(argv[3]);
  }

  if (argc > 4 && argv[4] && *argv[4])
  {
    rnd = atoi(argv[4]);
  }
  else
  {
    rnd = time(NULL);
  }

  printf("benchmark: array size: %d, sample size: %d, repetitions: %d, seed: %d\n\n", max, samples, repetitions, rnd);

  // random
  // 32 bit

  a_array = (int *)malloc(max * sizeof(int));
  r_array = (int *)malloc(max * sizeof(int));
  v_array = (int *)malloc(max * sizeof(int));

  // random

  srand(rnd);

  for (cnt = 0; cnt < max; cnt++)
  {
    r_array[cnt] = rand();
  }

  memcpy(v_array, r_array, max * sizeof(int));
  quadsort(v_array, max, sizeof(int), cmp_int);

  test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort, "quadsort", "random order", sizeof(int), cmp_int);

  free(a_array);
  free(r_array);
  free(v_array);

  return 0;
}
