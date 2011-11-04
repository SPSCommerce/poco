//
// SharedLibrary_HPUX.cpp
//
// $Id: //poco/svn/Foundation/src/SharedLibrary_HPUX.cpp#2 $
//
// Library: Foundation
// Package: SharedLibrary
// Module:  SharedLibrary
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/SharedLibrary_HPUX.h"
#include <dl.h>


namespace Poco {


FastMutex SharedLibraryImpl::_mutex;


SharedLibraryImpl::SharedLibraryImpl()
{
	_handle = 0;
}


SharedLibraryImpl::~SharedLibraryImpl()
{
}


void SharedLibraryImpl::loadImpl(const std::string& path, int /*flags*/)
{
        FastMutex::ScopedLock lock(_mutex);

	if (_handle) throw LibraryAlreadyLoadedException(path);
	_handle = shl_load(path.c_str(), BIND_DEFERRED, 0);
	if (!_handle) throw LibraryLoadException(path);
	_path = path;
}


void SharedLibraryImpl::unloadImpl()
{
	FastMutex::ScopedLock lock(_mutex);

	if (_handle)
	{
		shl_unload(_handle);
		_handle = 0;
		_path.clear();
	}
}


bool SharedLibraryImpl::isLoadedImpl() const
{
	return _handle != 0; 
}


void* SharedLibraryImpl::findSymbolImpl(const std::string& name)
{
	FastMutex::ScopedLock lock(_mutex);

	void* result = 0;
	if (_handle && shl_findsym(&_handle, name.c_str(), TYPE_UNDEFINED, &result) !=  -1)
		return result;
	else
		return 0;
}


const std::string& SharedLibraryImpl::getPathImpl() const
{
	return _path;
}


std::string SharedLibraryImpl::suffixImpl()
{
#if defined(_DEBUG)
	return "d.sl";
#else
	return ".sl";
#endif
}


} // namespace Poco
