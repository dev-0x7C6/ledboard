#pragma once

template <typename... bit_types>
constexpr auto bitcalc(bit_types&&... values) {
	return (... | (1 << values)); // fold expression
}

template< class T > struct remove_reference {typedef T type;};
template< class T > struct remove_reference<T&> {typedef T type;};
template< class T > struct remove_reference<T&&> {typedef T type;};

template< class T > constexpr typename remove_reference<T>::type&& move( T&& t ) noexcept;
template<typename T, typename U> constexpr decltype(auto) forward(U &&u) noexcept { return static_cast<T &&>(u); }


template <typename type>
class pair
{
public:
	constexpr pair(type first, type second) : m_first(first), m_second(second) {}
	
	constexpr pair(const pair &) = default;
	
	constexpr auto p1() const noexcept { return m_first; }
	constexpr auto p2() const noexcept { return m_second; }
	
private:
	const type m_first;
	const type m_second;
};
