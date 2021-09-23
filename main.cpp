#include <thread>
#include <mutex>
#include <cmath>
#include <stdint.h>

#define CHEVAN_UTILS_INCL
#define CHEVAN_UTILS_PRINT
#define CHEVAN_UTILS_bytes
#include <gmp.h>
#include <boost/multiprecision/cpp_int.hpp>
typedef __uint128_t bigUInt;
typedef boost::multiprecision::uint1024_t biggerUInt;
namespace chevan_utils
{
  static std::mutex mutex;
  static void print(bigUInt n)
  {
    if (n < UINT64_MAX)
    {
      std::cout << (uint64_t)n;
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
  static void print(biggerUInt n)
  {
    if (n < (biggerUInt)std::pow(2, 128))
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
using namespace chevan_utils;
template <typename... P>
static void atomic_println(P... p)
{
  mutex.lock();
  println(p...);
  mutex.unlock();
}
#define print atomic_println // whenever printing, use this function instead

#define THREAD_COUNT 32 // number of threads to use
#define MAX_K_TESTED 120

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
  if (!isPrime(p))
    return false;
  T s = 4;
  for (T i = 3; i <= p; i++)
  {
    s = (s * s - 2) % m;
  }
  return s == 0;
}
template <typename T>
static bool isMersennesPrime(mpz_t m, T p)
{
  if (!isPrime(p))
    return false;
  mpz_t s;
  mpz_init(s);
  mpz_set_ui(s, 4);
  for (biggerUInt i = 3; i <= p; i++)
  {
    mpz_mul(s, s, s);
    mpz_sub_ui(s, s, 2);
    mpz_mod(s, s, m);
    // s = (s * s - 2) % m;
  }
  return mpz_get_ui(s) == 0;
}

template <typename T>
static void euclid_euler(T k)
{
  T power = ((T)1) << (k - 1);    // 2 ^ (k-1)
  T primePart = (power << 1) - 1; // power * 2 - 1 or 2 ^ k - 1

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
    print("imprecise: ", power * primePart);
  }
  else
  {
    print(power * primePart);
  }
}
// 1<<(k-1) is faster but biggerUInt doesn't support it
static void euclid_euler(biggerUInt k)
{
  biggerUInt power = 1;
  // std::pow is borked and returns infinity
  for (u64 p = 0; p < k - 1; p++)
  {
    power *= 2;
  }
  biggerUInt primePart = power * 2 - 1;

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
    print("imprecise: ", power * primePart);
  }
  else
  {
    print(power * primePart);
  }
}
static void euclid_euler_gmp(biggerUInt k)
{
  mpz_t power;
  mpz_init(power);
  // mpz_set_ui(power,1);
  // mpn_lshift(power->_mp_d, power->_mp_d, power->_mp_alloc, (u64)(k - 1));
  mpz_set_ui(power, 2);
  mpz_pow_ui(power, power, (u64)k - 1);

  mpz_t primePart;
  mpz_init(primePart);
  mpz_mul_ui(primePart, power, 2);
  mpz_sub_ui(primePart, primePart, 1);

  if (!isMersennesPrime(primePart, k))
    return;

  mpz_mul(power, power, primePart);
  char *result = new char[1000000];
  mpz_get_str(result, 10, power);
  print(result);
  delete[] result;
}

static void threadLaunch(uchar t)
{
  if(t==0)
  {
    euclid_euler<u32>(2);
  }
  for (bigUInt k = t * 2 + 3; 1; k += THREAD_COUNT * 2)
  {
    if (k >= MAX_K_TESTED) // tested first
    {
      euclid_euler_gmp(k);
    }
    else if (k < 17)
      euclid_euler<u32>(k);
    else if (k < 33)
      euclid_euler<u64>(k);
    else if (k < 65)
      euclid_euler<bigUInt>(k);
    else if (k < MAX_K_TESTED)
      euclid_euler((biggerUInt)k);
  }
}

int main()
{
  std::vector<std::thread> threads = std::vector<std::thread>(THREAD_COUNT);
  if (THREAD_COUNT == 1)
  {
    threadLaunch(0);
  }
  else
  {
    for (uchar t = 0; t < THREAD_COUNT; t++)
    {
      threads[t] = std::thread(threadLaunch, t);
    }

    for (uchar i = 0; i < THREAD_COUNT; i++)
    {
      threads[i].join();
    }
  }
  return 0;
}