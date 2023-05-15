#pragma once
#include <algorithm>
#include <typeinfo>
#define boost _boost_func_of_SQLMsg
#define func_arg_type SQLMsg*
#define func_arg pmsg
#include "any_function.inc"
#undef func_arg
#undef func_arg_type
#undef boost

typedef _boost_func_of_SQLMsg::any any_function;

#define boost _boost_func_of_void
#define func_arg_type 
#define func_arg 
#include "any_function.inc"
#undef func_arg
#undef func_arg_type
#undef boost

typedef _boost_func_of_void::any any_void_function;

template <class F>
class void_binder
{
	protected:
		F f;
		typename F::argument_type value;
	public:
		void_binder(const F& f, const typename F::argument_type x)
			: f(f), value(x) {}
		void operator()() const {
			return f(value);
		}
};

	template <class F, class Arg> 
inline void_binder<F> void_bind(const F& f, const Arg& arg)
{
	typedef typename F::argument_type arg_type;
	return void_binder<F>(f, arg_type(arg));
}
