#define CHEVAN_UTILS_INCL
#define CHEVAN_UTILS_PRINT
#define CHEVAN_UTILS_bytes
#include <cmath>
#include "utils.hpp"
#define print atomic_println
// #define print println
using namespace chevan_utils;

#define THREAD_COUNT 32
#include <thread>
#include <mutex>
static std::mutex mutex;

template <typename N>
static bool isPrime(N n)
{
  if (n % 2 == 0)
    return 0;
  for (N i = 3; i * i <= n; i += 2)
  {
    if ((n % i) == 0)
      return 0;
  }
  return 1;
}

template <typename... P>
static void atomic_println(P... p)
{
  mutex.lock();
  println(p...);
  mutex.unlock();
}

int main()
{
  u64 start = 6;
  std::vector<std::thread> threads = std::vector<std::thread>(THREAD_COUNT);
  for (uchar t = 0; t < THREAD_COUNT; t++)
  {
    threads[t] = std::thread([start, t]()
                             {
                               for (u64 k = t + 2; 1; k += THREAD_COUNT)
                               {
                                 u64 power = 1;
                                 // std::pow is borked and returns infinity
                                 for (u64 p = 0; p < k-1; p++)
                                 {
                                   power *= 2;
                                 }
                                 u64 primePart=power*2-1;
                                 if(!isPrime(primePart))
                                  continue;
                                 print(power*primePart);
                               }
                             });
  }

  for (uchar i = 0; i < THREAD_COUNT; i++)
  {
    threads[i].join();
  }
}