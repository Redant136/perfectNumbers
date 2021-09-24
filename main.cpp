#include <thread>
#include <mutex>
#include <cmath>
#include <stdint.h>
#include <chrono>
#include <ctime>

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
  std::string to_string(bigUInt n)
  {
    if (n < UINT64_MAX)
    {
      return std::to_string((uint64_t)n);
    }

    char str[50] = {0};
    char *s = str + sizeof(str) - 1;
    while (n > 0)
    {
      if (s == str)
        std::cout << "too many characters" << std::endl;
      *--s = "0123456789"[n % 10UL];
      n /= 10;
    }
    return s;
  }
  std::string to_string(biggerUInt n) {
    if (n < (biggerUInt)std::pow(2, 128))
    {
      return to_string((bigUInt)n);
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
    return std::string(s);
  }
  std::string to_string(mpz_t n)
  {
    char *result = new char[1000000];
    mpz_get_str(result, 10, n);
    std::string str = std::string(result);
    delete[] result;
    return str;
  }

  static void print(bigUInt n)
  {
    std::cout << to_string(n);
  }
  static void print(biggerUInt n)
  {
    std::cout << to_string(n);
  }
  static void print(mpz_t n)
  {
    std::cout << to_string(n);
  }
}
#include "utils.hpp"
using namespace chevan_utils;
static uint countFound = 0;
static auto startTime = std::chrono::high_resolution_clock::now();
template <typename... P>
static void atomic_println(P... p)
{
  mutex.lock();
  println(p...);
  mutex.unlock();
}

#define MAX_TERM_LENGTH 5
#define MAX_NUM_LENGTH 32
#define MAX_PRE_DOT_LENGTH 11
#define MAX_POST_DOT_LENGTH 6
#define MAX_NUM_PRETTY_PRINT_LENGTH (MAX_PRE_DOT_LENGTH + MAX_POST_DOT_LENGTH + 3)
#define MAX_TIME_LENGTH 32
static std::string pretty_print_time(ulong time)
{
  struct {
    uint ratio;
    std::string unit;
  }units[]={
    {1,"ms"},
    {1000,"s"},
    {60,"m"},
    {60,"h"},
    {24,"d"}
  };
  std::string str = "";
  ulong mainVal = time, secondVal = 0;
  for (uint i = 0; i < sizeof(units) / sizeof(units[0]); i++)
  {
    if (mainVal / units[i].ratio == 0 && i != 0)
      break;
    mainVal = mainVal / units[i].ratio;
    secondVal = mainVal % units[i].ratio;
    if (i > 0)
    {
      str = std::to_string(mainVal) + units[i].unit + std::to_string(secondVal) + units[i - 1].unit;
    }
    else
    {
      str = std::to_string(mainVal) + units[i].unit;
    }
  }

  return str;
}
template <typename P>
static void perfectNumber_println(P p)
{
  auto current = std::chrono::high_resolution_clock::now();

  std::string res="";

  std::string longNum = to_string(p);
  std::string num = "";
  if (longNum.length() < MAX_NUM_PRETTY_PRINT_LENGTH)
  {
    num = longNum;
  }
  else
  {
    for (uint i = 0; i < MAX_PRE_DOT_LENGTH && i < longNum.length(); i++)
    {
      num += longNum[i];
    }
    if (longNum.length() > MAX_PRE_DOT_LENGTH - 1)
    {
      num += "...";
      std::string postDot="";
      for (uint i = 0; i < MAX_POST_DOT_LENGTH && longNum.length() - i > 11; i++)
      {
        postDot = longNum[longNum.length() - 1 - i] + postDot;
      }
      num += postDot;
    }
  }
  for (uint i = 0; i < MAX_NUM_LENGTH - num.length(); i++)
  {
    res += ' ';
  }
  res += num;

  // std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
  ulong timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - startTime).count();

  std::string time = pretty_print_time(timeElapsed);
  for (uint i = 0; i < MAX_TIME_LENGTH - time.length(); i++)
  {
    res+=' ';
  }
  res += time;

  mutex.lock();

  countFound++;
  std::string count = std::to_string(countFound) + ":";
  for (uint i = 0; i < MAX_TERM_LENGTH - count.length(); i++)
  {
    count += ' ';
  }
  res = count + res;

  println(res);
  mutex.unlock();
}
#define print perfectNumber_println // whenever printing, use this function instead

#define THREAD_COUNT 32 // number of threads to use
#define MAX_K_TESTED 120

namespace PrimesSearch
{
  const uint precision = 10;
  template <typename N>
  static bool isPrimeOld(N n)
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
  T modulo(T a, T b, T c)
  {
    T x = 1;
    T y = a;
    while (b > 0)
    {
      if (b & 1)
        x = (x * y) % c;
      y = (y * y) % c;
      b = b >> 1;
    }
    return x % c;
  }
  template <typename T>
  static bool isPrime(T p)
  {
    if (p == 1)
      return 0;
    for (uint i = 0; i < precision; i++)
    {
      T a = rand() % (p - 1) + 1;
      if (modulo(a, p - 1, p) != 1)
        return 0;
    }
    return 1;
  }
};
using namespace PrimesSearch;

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

  print(power * primePart);
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

  print(power * primePart);
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
  print(power);
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
  // 4+32+32 = 68
  std::string header = "Term";
  for (uint i = 0; i < (MAX_TERM_LENGTH - sizeof("Term")) + (MAX_NUM_LENGTH - sizeof("Perfect Number")) + 2; i++)
  {
    header += ' ';
  }
  header += "Perfect Number";
  for (uint i = 0; i < MAX_TIME_LENGTH - sizeof("Time") + 1; i++)
  {
    header += ' ';
  }
  header += "Time";
  println(header);
  startTime = std::chrono::high_resolution_clock::now();
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