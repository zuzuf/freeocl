/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Nikolay Nikolaev

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef __FREEOCL_UTILS_HASH_H__
#define __FREEOCL_UTILS_HASH_H__

#include <cstdint>
#include <string>

namespace FreeOCL
{
	inline uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
	{
		// 'm' and 'r' are mixing constants generated offline.
		// They're not really 'magic', they just happen to work well.

		const uint32_t m = 0x5bd1e995;
		const int r = 24;

		// Initialize the hash to a 'random' value

		uint32_t h = seed ^ len;

		// Mix 4 bytes at a time into the hash

		const unsigned char * data = (const unsigned char *)key;

		while(len >= 4)
		{
			uint32_t k = *(uint32_t*)data;

			k *= m;
			k ^= k >> r;
			k *= m;

			h *= m;
			h ^= k;

			data += 4;
			len -= 4;
		}

		// Handle the last few bytes of the input array

		switch(len)
		{
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0];
			h *= m;
		};

		// Do a few final mixes of the hash to ensure the last few
		// bytes are well-incorporated.

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}

	template<typename _Key>
	struct hash : public std::hash<_Key>
	{
	};

	template<>
	struct hash<std::string>
	{
		inline size_t operator()(const std::string &str) const
		{
			return MurmurHash2(str.data(), str.size(), 317U);
		}
	};
}

#endif /* __FREEOCL_UTILS_HASH_H__ */
