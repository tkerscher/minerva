#pragma once

//Implementation of std::span for C++17

#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace minerva {

    // constants
    inline constexpr std::size_t dynamic_extent = -1;

    // class template span
    template<class ElementType, std::size_t Extent = dynamic_extent>
    class span;

    //type traits
    namespace detail {
        template<class T> using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

        //is span
        template<class> struct is_span : std::false_type {};
        template<class T, std::size_t N> struct is_span<span<T, N>> : std::true_type {};
        template<class T> inline constexpr bool is_span_v = is_span<T>::value;

        //is std array
        template<class> struct is_std_array : std::false_type {};
        template<class T, std::size_t N> struct is_std_array<std::array<T, N>> : std::true_type {};
        template<class T> inline constexpr bool is_std_array_v = is_std_array<T>::value;

        //support data() and size()
        template<class, class = void> struct supports_data_size : std::false_type {};
        template<class T> struct supports_data_size <T, std::void_t<
            decltype(std::data(std::declval<T>())),
            decltype(std::size(std::declval<T>()))
            >> : std::true_type {};
        template<class T> inline constexpr bool supports_data_size_v = supports_data_size<T>::value;

        //is container
        template<class C, class T = remove_cvref_t<C>> struct is_container {
            static constexpr bool value =
                !is_span_v<T> && !is_std_array_v<T> && !std::is_array_v<T> && supports_data_size_v<C>;
        };
        template<class C> inline constexpr bool is_container_v = is_container<C>::value;

        //container element convertible
        template<class, class, class = void>
        struct is_container_element_convertible : std::false_type {};
        template<class T, class E>
        struct is_container_element_convertible<T, E, std::void_t<decltype(std::data(std::declval<T>()))>>
            : std::is_convertible<
            std::remove_pointer_t<decltype(std::data(std::declval<T>()))>(*)[],
            E(*)[]
            >
        {};
        template<class T, class E> inline constexpr bool is_container_element_convertible_v = is_container_element_convertible<T, E>::value;
    }

    template<class ElementType, std::size_t Extent>
    class span {
    public:
        // constants and types
        using element_type = ElementType;
        using value_type = std::remove_cv_t<ElementType>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using pointer = element_type*;
        using const_pointer = const element_type*;
        using reference = element_type&;
        using const_reference = const element_type&;

        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        static constexpr size_type extent = Extent;

        // constructors, copy, and assignment
        template<std::size_t E = Extent,
            std::enable_if_t<E == dynamic_extent || E == 0, int> = 0>
        constexpr span() noexcept
            : data_(nullptr), size_(0)
        {}
        constexpr span(pointer ptr, size_type count)
            : data_(ptr), size_(count)
        {
            assert(extent == dynamic_extent || count == extent);
        }
        constexpr span(pointer first, pointer last)
            : data_(first), size_(last - first)
        {
            assert(extent == dynamic_extent || (last - first) == static_cast<std::ptrdiff_t>(extent));
        }

        template<std::size_t N, std::size_t E = Extent,
            typename std::enable_if_t<
            (E == dynamic_extent || N == E)
            && detail::is_container_element_convertible_v<element_type(&)[N], ElementType>
            , int> = 0>
        constexpr span(element_type(&arr)[N]) noexcept
            : data_(arr), size_(N)
        {}
        template<std::size_t N, std::size_t E = Extent,
            typename std::enable_if_t<
            (E == dynamic_extent || N == E)
            && detail::is_container_element_convertible_v<std::array<value_type, N>&, ElementType>
            , int> = 0>
        constexpr span(std::array<value_type, N>& arr) noexcept
            : data_(arr.data()), size_(N)
        {}
        template<std::size_t N, std::size_t E = Extent,
            typename std::enable_if_t<
            (E == dynamic_extent || N == E)
            && detail::is_container_element_convertible_v<const std::array<value_type, N>&, ElementType>
            , int> = 0>
        constexpr span(const std::array<value_type, N>& arr) noexcept
            : data_(arr.data()), size_(N)
        {}

        template<class Container, std::size_t E = Extent,
            typename std::enable_if_t<
            detail::is_container_v<Container>
            && detail::is_container_element_convertible_v<Container&, ElementType>
            , int> = 0>
        constexpr span(Container& cont)
            : data_(std::data(cont)), size_(std::size(cont))
        {
            assert(E == dynamic_extent || E == std::size(cont));
        }
        template<class Container, std::size_t E = Extent,
            typename std::enable_if_t<
            detail::is_container_v<Container>
            && detail::is_container_element_convertible_v<const Container&, ElementType>
            , int> = 0>
        constexpr span(const Container& cont)
            : data_(std::data(cont)), size_(std::size(cont))
        {
            assert(E == dynamic_extent || E == std::size(cont));
        }

        constexpr span(const span& other) noexcept = default;

        template<class OtherElementType, std::size_t OtherExtent,
            typename std::enable_if_t<
            (Extent == OtherExtent || Extent == dynamic_extent)
            && std::is_convertible_v<OtherElementType(*)[], ElementType(*)[]>
            , int> = 0>
        constexpr span(const span<OtherElementType, OtherExtent>& s) noexcept
            : data_(s.data()), size_(s.size())
        {}

        ~span() noexcept = default;

        constexpr span& operator=(const span& other) noexcept = default;

        //subviews
        template<size_t Count>
        constexpr span<element_type, Count> first() const {
            assert(Count <= size());
            return { data(), Count };
        }
        template<size_t Count>
        constexpr span<element_type, Count> last() const {
            assert(Count <= size());
            return { data() + (size() - Count), Count };
        }
        template<size_t Offset, size_t Count = dynamic_extent>
        constexpr span<ElementType,
            Count != dynamic_extent ? Count
            : (Extent != dynamic_extent ? Extent - Offset
                : dynamic_extent)>
            subspan() const
        {
            assert(Offset <= size() && (Count == dynamic_extent || Offset + Count <= size()));
            return { data() + Offset, Count != dynamic_extent ? Count : size() - Offset };
        }

        constexpr span<element_type, dynamic_extent> first(size_type count) const {
            assert(count <= size());
            return { data(), count };
        }
        constexpr span<element_type, dynamic_extent> last(size_type count) const {
            assert(count <= size());
            return { data() + (size() - count), count };
        }
        constexpr span<element_type, dynamic_extent> subspan(
            size_type offset, size_type count = dynamic_extent) const
        {
            assert(offset <= size() && (count == dynamic_extent || offset + count <= size()));
            return { data() + offset, count == dynamic_extent ? size() - offset : count };
        }

        // observers
        constexpr size_type size() const noexcept { return size_; }
        constexpr size_type size_bytes() const noexcept { return size_ * sizeof(ElementType); }
        [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

        // element access
        constexpr reference operator[](size_type idx) const {
            assert(idx < size());
            return *(data() + idx);
        }
        constexpr reference front() const {
            assert(!empty());
            return *data();
        }
        constexpr reference back() const {
            assert(!empty());
            return *(data() + size() - 1);
        }
        constexpr pointer data() const noexcept { return data_; }

        // iterator support
        constexpr iterator       begin()  const noexcept { return data(); }
        constexpr iterator       end()    const noexcept { return data() + size(); }
        constexpr const_iterator cbegin() const noexcept { return begin(); }
        constexpr const_iterator cend()   const noexcept { return end(); }

        constexpr reverse_iterator rbegin()        const noexcept { return reverse_iterator(end()); }
        constexpr reverse_iterator rend()          const noexcept { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
        constexpr const_reverse_iterator crend()   const noexcept { return const_reverse_iterator(cbegin()); }

        friend constexpr iterator begin(span s) noexcept { return s.begin(); }
        friend constexpr iterator end(span s)   noexcept { return s.end(); }

    private:
        pointer data_;    // exposition only
        size_type size_; // exposition only
    };

    template<class T, size_t N> span(T(&)[N])->span<T, N>;
    template<class T, size_t N> span(std::array<T, N>&)->span<T, N>;
    template<class T, size_t N> span(const std::array<T, N>&)->span<const T, N>;
    template<class Container>   span(Container&)->span<typename Container::value_type>;
    template<class Container>   span(const Container&)->span<const typename Container::value_type>;

    // views of object representation
    template<class ElementType, size_t Extent>
    span<const std::byte, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent>
        as_bytes(span<ElementType, Extent> s) noexcept
    {
        return { reinterpret_cast<const std::byte*>(s.data()), s.size_bytes() };
    }

    template<class ElementType, size_t Extent>
    span<std::byte, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent>
        as_writable_bytes(span<ElementType, Extent> s) noexcept
    {
        return { reinterpret_cast<std::byte*>(s.data()), s.size_bytes() };
    }

}

namespace std {
    // tuple interface
    template<class ElementType, size_t Extent>
    struct tuple_size<minerva::span<ElementType, Extent>>
        : public std::integral_constant<size_t, Extent> {};

    template<class ElementType>
    struct tuple_size<minerva::span<ElementType, minerva::dynamic_extent>>;       // not defined

    template<size_t I, class ElementType, size_t Extent>
    struct tuple_element<I, minerva::span<ElementType, Extent>>
    {
        static_assert(Extent != minerva::dynamic_extent && I < Extent);
        using type = ElementType;
    };

    template<size_t I, class ElementType, size_t Extent>
    constexpr ElementType& get(minerva::span<ElementType, Extent> s) noexcept {
        return s[I];
    }
}
