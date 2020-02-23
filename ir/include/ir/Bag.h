#pragma once

#include <mn/Buf.h>

namespace ir
{
	template<typename T>
	struct Bag
	{
		struct Handle
		{
			size_t ix;

			bool operator==(Handle other) const { return ix == other.ix; }
			bool operator!=(Handle other) const { return ix != other.ix; }
			operator bool() const { return ix != 0; }
		};
		mn::Buf<T> buf;
	};

	template<typename T>
	inline static Bag<T>
	bag_new()
	{
		Bag<T> self{};
		self.buf = mn::buf_new<T>();
		return self;
	}

	template<typename T>
	inline static void
	bag_free(Bag<T>& self)
	{
		destruct(self.buf);
	}

	template<typename T>
	inline static void
	destruct(Bag<T>& self)
	{
		bag_free(self);
	}

	template<typename T, typename R>
	inline static typename Bag<T>::Handle
	bag_insert(Bag<T>& self, R&& v)
	{
		auto h = typename Bag<T>::Handle{self.buf.count + 1};
		mn::buf_push(self.buf, T{v});
		return h;
	}

	template<typename T>
	inline static T*
	bag_lookup(Bag<T>& self, typename Bag<T>::Handle h)
	{
		if(h.ix == 0 || h.ix > self.buf.count)
			return nullptr;
		return self.buf.ptr + h.ix - 1;
	}

	template<typename T>
	inline static const T*
	bag_lookup(const Bag<T>& self, typename Bag<T>::Handle h)
	{
		if(h.ix == 0 || h.ix > self.buf.count)
			return nullptr;
		return self.buf.ptr + h.ix - 1;
	}
}