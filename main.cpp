#include <thread>
#include <mutex>
#include <cmath>
#include <stdint.h>

#define CHEVAN_UTILS_INCL
#define CHEVAN_UTILS_PRINT
#define CHEVAN_UTILS_bytes
#include <boost/multiprecision/cpp_int.hpp>
typedef __uint128_t bigUInt;
typedef boost::multiprecision::uint1024_t hugeUInt;
namespace chevan_utils
{
  static std::mutex mutex;
  static void print(bigUInt n)
  {
    if (n < UINT64_MAX)
    {
      std::cout << (uint64_t)n << std::endl;
      return;
    }
    char str[40] = {0};
    char *s = str + sizeof(str) - 1;
    while (n != 0)
    {
      if (s == str)
        std::cout << "too many characters" << std::endl;
      *--s = "0123456789"[n % 10];
      n /= 10;
    }
    std::cout << s;
  }
  static void print(hugeUInt n)
  {
    if (n < UINT64_MAX)
    {
      std::cout << (uint64_t)n << std::endl;
      return;
    }
    if (n < (hugeUInt)std::pow(2, 128))
    {
      print((bigUInt)n);
      return;
    }
    char str[400] = {0};
    char *s = str + sizeof(str) - 1;
    while (n != 0)
    {
      if (s == str)
        std::cout << "too many characters" << std::endl;
      *--s = "0123456789"[(uint)(n % 10)];
      n /= 10;
    }
    std::cout << s;
  }
}
#include "utils.hpp"
namespace chevan_utils
{
  template <typename... P>
  static void atomic_println(P... p)
  {
    mutex.lock();
    println(p...);
    mutex.unlock();
  }
}
#define print atomic_println
using namespace chevan_utils;

#define THREAD_COUNT 36
#define MAX_K_TESTED 130

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

template <typename T>
static bool isMersennesPrime(T m, T p)
{
  T s = 4;
  for (T i = 3; i <= p; i++)
  {
    s = (s * s - 2) % m;
  }
  return s == 0;
}

template <typename T>
static void euclid_euler(T k)
{
  T power = 1;
  // std::pow is borked and returns infinity
  for (u64 p = 0; p < k - 1; p++)
  {
    power *= 2;
  }
  T primePart = power * 2 - 1;
  
  // the result must end with 6 or 8
  if (((power % 10) * (primePart % 10) % 10) != 8 && ((power % 10) * (primePart % 10) % 10) != 6)
    return;

  if (k > 2)
  {
    if (!isMersennesPrime(primePart, k))
      return;
  }
  else
  {
    if (!isPrime(primePart))
      return;
  }

  if (k >= MAX_K_TESTED)
  {
    print("imprecise: ",power * primePart);
  }
  else
  {
    print(power * primePart);
  }
}

static void threadLaunch(uchar t)
{
  for (bigUInt k = t + 2; 1; k += THREAD_COUNT)
  {
    if (k < 17)
      euclid_euler<u32>(k);
    else if (k < 33)
      euclid_euler<u64>(k);
    else if (k < 65)
      euclid_euler<bigUInt>(k);
    else 
      euclid_euler<hugeUInt>(k);
  }
}

int main()
{
  std::vector<std::thread> threads = std::vector<std::thread>(THREAD_COUNT);
  for (uchar t = 0; t < THREAD_COUNT; t++)
  {
    threads[t] = std::thread(threadLaunch, t);
  }

  for (uchar i = 0; i < THREAD_COUNT; i++)
  {
    threads[i].join();
  }
}