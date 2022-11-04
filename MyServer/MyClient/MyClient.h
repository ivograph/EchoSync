// MyServer.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/core/noncopyable.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::placeholders;

#define MEM_FN(x) boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x,y,z) boost::bind(&self_type::x, shared_from_this(), y, z)
