//
// detail/winapp_thread.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_DETAIL_WINAPP_THREAD_HPP
#define NET_TS_DETAIL_WINAPP_THREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>

#if defined(NET_TS_WINDOWS) && defined(NET_TS_WINDOWS_APP)

#include <experimental/__net_ts/detail/memory.hpp>
#include <experimental/__net_ts/detail/noncopyable.hpp>
#include <experimental/__net_ts/detail/socket_types.hpp>
#include <experimental/__net_ts/detail/throw_error.hpp>
#include <experimental/__net_ts/error.hpp>

#include <experimental/__net_ts/detail/push_options.hpp>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {
namespace detail {

DWORD WINAPI winapp_thread_function(LPVOID arg);

class winapp_thread
  : private noncopyable
{
public:
  // Constructor.
  template <typename Function>
  winapp_thread(Function f, unsigned int = 0)
  {
    std::auto_ptr<func_base> arg(new func<Function>(f));
    DWORD thread_id = 0;
    thread_ = ::CreateThread(0, 0, winapp_thread_function,
        arg.get(), 0, &thread_id);
    if (!thread_)
    {
      DWORD last_error = ::GetLastError();
      std::error_code ec(last_error,
          std::experimental::net::error::get_system_category());
      std::experimental::net::detail::throw_error(ec, "thread");
    }
    arg.release();
  }

  // Destructor.
  ~winapp_thread()
  {
    ::CloseHandle(thread_);
  }

  // Wait for the thread to exit.
  void join()
  {
    ::WaitForSingleObjectEx(thread_, INFINITE, false);
  }

  // Get number of CPUs.
  static std::size_t hardware_concurrency()
  {
    SYSTEM_INFO system_info;
    ::GetNativeSystemInfo(&system_info);
    return system_info.dwNumberOfProcessors;
  }

private:
  friend DWORD WINAPI winapp_thread_function(LPVOID arg);

  class func_base
  {
  public:
    virtual ~func_base() {}
    virtual void run() = 0;
  };

  template <typename Function>
  class func
    : public func_base
  {
  public:
    func(Function f)
      : f_(f)
    {
    }

    virtual void run()
    {
      f_();
    }

  private:
    Function f_;
  };

  ::HANDLE thread_;
};

inline DWORD WINAPI winapp_thread_function(LPVOID arg)
{
  std::auto_ptr<winapp_thread::func_base> func(
      static_cast<winapp_thread::func_base*>(arg));
  func->run();
  return 0;
}

} // namespace detail
} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#include <experimental/__net_ts/detail/pop_options.hpp>

#endif // defined(NET_TS_WINDOWS) && defined(NET_TS_WINDOWS_APP)

#endif // NET_TS_DETAIL_WINAPP_THREAD_HPP
