/*
 * MIT License
 * 
 * Copyright (c) 2017-2019 Mikhail Pilin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <ww898/utf_selector.hpp>
#include <ww898/utf_config.hpp>
#include <ww898/utf_iter.hpp>

#include <cstdint>
#include <iterator>
#include <string>

#if __cpp_lib_string_view >= 201606
#include <string_view>
#endif

namespace ww898 {
namespace utf {

namespace detail {

enum struct convz_impl { normal, binary_copy };

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit,
    convz_impl>
struct convz_strategy
{
    Oit operator()(It it, Oit oit) const
    {
        // The read wrapper needs to check if it reached zero since we can't rely on read() to detect the null-term since it's reading chunks at a time
        ReadIterZChecked<It, typename Utf::char_type> read_fn(it);
        WriteIter<Oit, typename Outf::char_type> write_fn(oit);
        while (true)
        {
            auto const cp = Utf::read(read_fn);
            if (!cp)
                return write_fn.oit;
            Outf::write(cp, write_fn);
        }
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct convz_strategy<Utf, Outf, It, Oit, convz_impl::binary_copy>
{
    Oit operator()(It it, Oit oit) const
    {
        while (true)
        {
            auto const ch = *it++;
            if (!ch)
                return oit;
            *oit++ = ch;
        }
    }
};

}

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
Oit convz(It && it, Oit && oit)
{
    return detail::convz_strategy<Utf, Outf,
            typename std::decay<It>::type,
            typename std::decay<Oit>::type,
            std::is_same<Utf, Outf>::value
                ? detail::convz_impl::binary_copy
                : detail::convz_impl::normal>()(
        std::forward<It>(it),
        std::forward<Oit>(oit));
}

namespace detail {

enum struct conv_impl { normal, random_interator, binary_copy };

// normal - non random-access iterator or direct copy
template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit,
    conv_impl>
struct conv_strategy final
{
    Oit operator()(It it, It const eit, Oit oit) const
    {
        ReadIterSeqChecked<It, typename Utf::char_type> read_fn(it, eit);
        WriteIter<Oit, typename Outf::char_type> write_fn(oit);
        while (read_fn.it != eit)
            Outf::write(Utf::read(read_fn), write_fn);
        return oit;
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct conv_strategy<Utf, Outf, It, Oit, conv_impl::random_interator> final
{
    Oit operator()(It it, It const eit, Oit oit) const
    {
        WriteIter<Oit, typename Outf::char_type> write_fn{ oit };
        ReadIterRnd<It, typename Utf::char_type> fast_read_fn(it);
        // end iterator in terms of ReadIter's internal iterator type. Need C-style case since this cast is either no-op or a reinterpret_cast
        auto eff_eit = (typename ReadIterRnd<It, typename Utf::char_type>::InternalIt)eit;
        constexpr int offset_from_end = Utf::max_supported_symbol_size;

        if (eff_eit - fast_read_fn.it >= static_cast<typename std::iterator_traits<It>::difference_type>(offset_from_end))
        {
            auto fast_eit = eff_eit - offset_from_end;
            while (fast_read_fn.it < fast_eit) {
                auto c1 = Utf::read(fast_read_fn);
                Outf::write(c1, write_fn);
            }
        }

        ReadIterRndChecked<It, typename Utf::char_type> checked_read_it(fast_read_fn.get(), eit);
        while (checked_read_it.it != eff_eit)
            Outf::write(Utf::read(checked_read_it), write_fn);

        return write_fn.oit;
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct conv_strategy<Utf, Outf, It, Oit, conv_impl::binary_copy> final
{
    Oit operator()(It it, It const eit, Oit oit) const
    {
        while (it != eit)
            *oit++ = *it++;
        return oit;
    }
};

}

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Eit,
    typename Oit>
Oit conv(It && it, Eit && eit, Oit && oit)
{
    return detail::conv_strategy<Utf, Outf,
            typename std::decay<It>::type,
            typename std::decay<Oit>::type,
            std::is_same<Utf, Outf>::value
                ? detail::conv_impl::binary_copy
                : std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<typename std::decay<It>::type>::iterator_category>::value
                    ? detail::conv_impl::random_interator
                    : detail::conv_impl::normal>()(
        std::forward<It>(it),
        std::forward<Eit>(eit),
        std::forward<Oit>(oit));
}

template<
    typename Outf,
    typename Ch,
    typename Oit>
Oit convz(Ch const * const str, Oit && oit)
{
    return convz<utf_selector_t<Ch>, Outf>(str, std::forward<Oit>(oit));
}

template<
    typename Och,
    typename Str>
std::basic_string<Och> convz(Str && str)
{
    std::basic_string<Och> res;
    convz<utf_selector_t<Och>>(std::forward<Str>(str), std::back_inserter(res));
    return res;
}

template<
    typename Outf,
    typename Ch,
    typename Oit>
Oit conv(std::basic_string<Ch> const & str, Oit && oit)
{
    return conv<utf_selector_t<Ch>, Outf>(str.cbegin(), str.cend(), std::forward<Oit>(oit));
}

#if __cpp_lib_string_view >= 201606
template<
    typename Outf,
    typename Ch,
    typename Oit>
Oit conv(std::basic_string_view<Ch> const & str, Oit && oit)
{
    return conv<utf_selector_t<Ch>, Outf>(str.cbegin(), str.cend(), std::forward<Oit>(oit));
}
#endif

template<
    typename Och,
    typename Str,
    typename std::enable_if<!std::is_same<typename std::decay<Str>::type, std::basic_string<Och>>::value, void *>::type = nullptr>
std::basic_string<Och> conv(Str && str)
{
    std::basic_string<Och> res;
    conv<utf_selector_t<Och>>(std::forward<Str>(str), std::back_inserter(res));
    return res;
}

template<
    typename Ch>
std::basic_string<Ch> conv(std::basic_string<Ch> str) throw()
{
    return str;
}

}}
