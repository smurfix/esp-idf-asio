//
// ip/impl/address.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IP_IMPL_ADDRESS_IPP
#define ASIO_IP_IMPL_ADDRESS_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/config.hpp"
#include <typeinfo>
#include "asio/detail/throw_error.hpp"
#include "asio/detail/throw_exception.hpp"
#include "asio/error.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/bad_address_cast.hpp"
#include "asio/system_error.hpp"

#include "asio/detail/push_options.hpp"

namespace asio {
namespace ip {

address::address() ASIO_NOEXCEPT
  : type_(ipv4)
  , ipv4_address_()
#ifdef CONFIG_LWIP_IPV6
  , ipv6_address_()
#endif
{
}

address::address(
    const asio::ip::address_v4& ipv4_address) ASIO_NOEXCEPT
  : type_(ipv4)
  , ipv4_address_(ipv4_address)
#ifdef CONFIG_LWIP_IPV6
  , ipv6_address_()
#endif
{
}

#ifdef CONFIG_LWIP_IPV6
address::address(
    const asio::ip::address_v6& ipv6_address) ASIO_NOEXCEPT
  : type_(ipv6)
  , ipv4_address_()
  , ipv6_address_(ipv6_address)
{
}
#endif

address::address(const address& other) ASIO_NOEXCEPT
  : type_(other.type_)
  , ipv4_address_(other.ipv4_address_)
#ifdef CONFIG_LWIP_IPV6
  , ipv6_address_(other.ipv6_address_)
#endif
{
}

#if defined(ASIO_HAS_MOVE)
address::address(address&& other) ASIO_NOEXCEPT
  : type_(other.type_)
  , ipv4_address_(other.ipv4_address_)
#ifdef CONFIG_LWIP_IPV6
  , ipv6_address_(other.ipv6_address_)
#endif
{
}
#endif // defined(ASIO_HAS_MOVE)

address& address::operator=(const address& other) ASIO_NOEXCEPT
{
  type_ = other.type_;
  ipv4_address_ = other.ipv4_address_;
#ifdef CONFIG_LWIP_IPV6
  ipv6_address_ = other.ipv6_address_;
#endif
  return *this;
}

#if defined(ASIO_HAS_MOVE)
address& address::operator=(address&& other) ASIO_NOEXCEPT
{
  type_ = other.type_;
  ipv4_address_ = other.ipv4_address_;
#ifdef CONFIG_LWIP_IPV6
  ipv6_address_ = other.ipv6_address_;
#endif
  return *this;
}
#endif // defined(ASIO_HAS_MOVE)

address& address::operator=(
    const asio::ip::address_v4& ipv4_address) ASIO_NOEXCEPT
{
  type_ = ipv4;
  ipv4_address_ = ipv4_address;
#ifdef CONFIG_LWIP_IPV6
  ipv6_address_ = asio::ip::address_v6();
#endif
  return *this;
}

#ifdef CONFIG_LWIP_IPV6
address& address::operator=(
    const asio::ip::address_v6& ipv6_address) ASIO_NOEXCEPT
{
  type_ = ipv6;
  ipv4_address_ = asio::ip::address_v4();
  ipv6_address_ = ipv6_address;
  return *this;
}
#endif

address make_address(const char* str)
{
  asio::error_code ec;
  address addr = make_address(str, ec);
  asio::detail::throw_error(ec);
  return addr;
}

address make_address(const char* str,
    asio::error_code& ec) ASIO_NOEXCEPT
{
#ifdef CONFIG_LWIP_IPV6
  asio::ip::address_v6 ipv6_address =
    asio::ip::make_address_v6(str, ec);
  if (!ec)
    return address(ipv6_address);
#endif

  asio::ip::address_v4 ipv4_address =
    asio::ip::make_address_v4(str, ec);
  if (!ec)
    return address(ipv4_address);

  return address();
}

address make_address(const std::string& str)
{
  return make_address(str.c_str());
}

address make_address(const std::string& str,
    asio::error_code& ec) ASIO_NOEXCEPT
{
  return make_address(str.c_str(), ec);
}

#if defined(ASIO_HAS_STRING_VIEW)

address make_address(string_view str)
{
  return make_address(static_cast<std::string>(str));
}

address make_address(string_view str,
    asio::error_code& ec) ASIO_NOEXCEPT
{
  return make_address(static_cast<std::string>(str), ec);
}

#endif // defined(ASIO_HAS_STRING_VIEW)

asio::ip::address_v4 address::to_v4() const
{
  if (type_ != ipv4)
  {
    bad_address_cast ex;
    asio::detail::throw_exception(ex);
  }
  return ipv4_address_;
}

#ifdef CONFIG_LWIP_IPV6
asio::ip::address_v6 address::to_v6() const
{
  if (type_ != ipv6)
  {
    bad_address_cast ex;
    asio::detail::throw_exception(ex);
  }
  return ipv6_address_;
}
#endif

std::string address::to_string() const
{
#ifdef CONFIG_LWIP_IPV6
  if (type_ == ipv6)
    return ipv6_address_.to_string();
#endif
  return ipv4_address_.to_string();
}

#if !defined(ASIO_NO_DEPRECATED)
std::string address::to_string(asio::error_code& ec) const
{
#ifdef CONFIG_LWIP_IPV6
  if (type_ == ipv6)
    return ipv6_address_.to_string(ec);
#endif
  return ipv4_address_.to_string(ec);
}
#endif // !defined(ASIO_NO_DEPRECATED)

bool address::is_loopback() const ASIO_NOEXCEPT
{
#ifdef CONFIG_LWIP_IPV6
  return (type_ == ipv4)
    ? ipv4_address_.is_loopback()
    : ipv6_address_.is_loopback();
#else
  return ipv4_address_.is_loopback();
#endif
}

bool address::is_unspecified() const ASIO_NOEXCEPT
{
#ifdef CONFIG_LWIP_IPV6
  return (type_ == ipv4)
    ? ipv4_address_.is_unspecified()
    : ipv6_address_.is_unspecified();
#else
  return ipv4_address_.is_unspecified();
#endif
}

bool address::is_multicast() const ASIO_NOEXCEPT
{
#ifdef CONFIG_LWIP_IPV6
  return (type_ == ipv4)
    ? ipv4_address_.is_multicast()
    : ipv6_address_.is_multicast();
#else
  return ipv4_address_.is_multicast();
#endif
}

bool operator==(const address& a1, const address& a2) ASIO_NOEXCEPT
{
  if (a1.type_ != a2.type_)
    return false;
#ifdef CONFIG_LWIP_IPV6
  if (a1.type_ == address::ipv6)
    return a1.ipv6_address_ == a2.ipv6_address_;
#endif
  return a1.ipv4_address_ == a2.ipv4_address_;
}

bool operator<(const address& a1, const address& a2) ASIO_NOEXCEPT
{
  if (a1.type_ < a2.type_)
    return true;
  if (a1.type_ > a2.type_)
    return false;
#ifdef CONFIG_LWIP_IPV6
  if (a1.type_ == address::ipv6)
    return a1.ipv6_address_ < a2.ipv6_address_;
#endif
  return a1.ipv4_address_ < a2.ipv4_address_;
}

} // namespace ip
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_IP_IMPL_ADDRESS_IPP
