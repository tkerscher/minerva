#pragma once

#include <array>

//Since we're targeting C++17 we dont have access to std::to_array. Fortunately,
//it's really simple so we're providing it ourselves.

namespace minerva {

	namespace detail {
		template<class T, std::size_t N, std::size_t... I>
		constexpr auto _to_array_impl(T(&a)[N], std::index_sequence<I...>)
			->std::array<std::remove_cv_t<T>, N>
		{
			return { { a[I]... } };
		}

		template<class T, std::size_t N, std::size_t... I>
		constexpr auto _to_array_impl(T(&& a)[N], std::index_sequence<I...>)
			->std::array<std::remove_cv_t<T>, N>
		{
			return { { std::move(a[I])... } };
		}
	}

	template<class T, std::size_t N>
	constexpr auto to_array(T(&a)[N]) {
		return detail::_to_array_impl(a, std::make_index_sequence<N>());
	}

	template<class T, std::size_t N>
	constexpr auto to_array(T(&& a)[N]) {
		return detail::_to_array_impl(std::move(a), std::make_index_sequence<N>());
	}
}
