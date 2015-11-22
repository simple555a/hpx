//  Copyright (c) 2011 Thomas Heller
//  Copyright (c) 2013 Hartmut Kaiser
//  Copyright (c) 2014 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_DETAIL_FUNCTION_REGISTRATION_HPP
#define HPX_UTIL_DETAIL_FUNCTION_REGISTRATION_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/serialization/detail/polymorphic_intrusive_factory.hpp>
#include <hpx/traits/needs_automatic_registration.hpp>
#include <hpx/util/demangle_helper.hpp>
#include <hpx/util/detail/vtable/vtable.hpp>

#include <boost/mpl/bool.hpp>

#include <string>

namespace hpx { namespace util { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Function>
    struct get_function_name_impl
    {
        static char const* call()
#ifndef HPX_HAVE_AUTOMATIC_SERIALIZATION_REGISTRATION
        ;
#else
        {
            // If you encounter this assert while compiling code, that means
            // that you have a HPX_UTIL_REGISTER_[UNIQUE_]FUNCTION macro
            // somewhere in a source file, but the header in which the function
            // is defined misses a HPX_UTIL_REGISTER_[UNIQUE_]FUNCTION_DECLARATION
            static_assert(
                traits::needs_automatic_registration<Function>::value,
                "HPX_UTIL_REGISTER_FUNCTION_DECLARATION missing");
            return util::type_id<Function>::typeid_.type_id();
        }
#endif
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Function>
    char const* get_function_name()
    {
        return get_function_name_impl<Function>::call();
    }

    ///////////////////////////////////////////////////////////////////////////
    struct HPX_EXPORT function_registration_info_base
    {
        virtual void const* get_vtable() const = 0;

        virtual ~function_registration_info_base() {}
    };

    template <typename VTable, typename T>
    struct function_registration_info : function_registration_info_base
    {
        virtual void const* get_vtable() const
        {
            return detail::get_vtable<VTable, T>();
        }
    };

    template <typename VTablePair>
    struct function_registration
    {
        typedef function_registration_info_base base_type;

        static void * create()
        {
            static function_registration_info<
                typename VTablePair::first_type,
                typename VTablePair::second_type
            > ri;
            return &ri;
        }

        function_registration()
        {
            hpx::serialization::detail::polymorphic_intrusive_factory::instance().
                register_class(
                    detail::get_function_name<VTablePair>()
                  , &function_registration::create
                );
        }
    };

    template <typename VTable>
    VTable const* get_vtable(std::string const& name)
    {
        detail::function_registration_info_base *
            p(
                hpx::serialization::detail::polymorphic_intrusive_factory::instance().
                    create<function_registration_info_base>(name)
            );

        return static_cast<VTable const*>(p->get_vtable());
    }

    template <
        typename VTablePair
      , typename Enable =
            typename traits::needs_automatic_registration<VTablePair>::type
    >
    struct automatic_function_registration
    {
        automatic_function_registration()
        {
            function_registration<VTablePair> auto_register;
        }

        automatic_function_registration& register_function()
        {
            return *this;
        }
    };

    template <typename VTablePair>
    struct automatic_function_registration<VTablePair, boost::mpl::false_>
    {
        automatic_function_registration()
        {}

        automatic_function_registration& register_function()
        {
            return *this;
        }
    };
}}}

#endif
