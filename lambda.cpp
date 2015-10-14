struct False {};
struct True {};

template<typename T, typename E1, typename E2>
struct If;

template<typename E1, typename E2>
struct If<True, E1, E2> {
    typedef E1 type;
};

template<typename E1, typename E2>
struct If<False, E1, E2> {
    typedef E2 type;
};

template<typename M, typename N>
struct same_type {
    typedef False type;
};

template<typename M>
struct same_type<M, M> {
    typedef True type;
};

// only formal
template<typename X, typename M>
struct abst {};

template<typename M, typename N>
struct appl {};

// substitution
template<typename M, typename X, typename N>
struct subst {
    typedef M type;
};

template<typename X, typename N>
struct subst<X, X, N> {
    typedef N type;
};

template<typename S, typename T, typename X, typename N>
struct subst<appl<S, T>, X, N> {
    typedef appl<typename subst<S, X, N>::type, typename subst<T, X, N>::type> type;
};

// avoid capture
template<typename S, typename X, typename N>
struct subst<abst<X, S>, X, N> {
    typedef abst<X, S> type;
};

template<typename Y, typename S, typename X, typename N>
struct subst<abst<Y, S>, X, N> {
    typedef abst<Y, typename subst<S, X, N>::type> type;
};

// beta
template<typename M>
struct beta {
    typedef M type;
};

template<typename X, typename M, typename N>
struct beta<appl<abst<X, M>, N> > {
    typedef typename subst<M, X, N>::type type;
};

template<typename X, typename M>
struct beta<abst<X, M> > {
    typedef abst<X, typename beta<M>::type> type;
};

template<typename M, typename N>
struct beta<appl<M, N> > {
    typedef appl<typename beta<M>::type, typename beta<N>::type> type;
};

template<typename M, typename Z>
struct reduce_impl;

template<typename M>
struct reduce_impl<M, True> {
    typedef M type;
};

template<typename M>
struct reduce_impl<M, False> {
    typedef typename beta<M>::type impl;
    typedef typename reduce_impl<impl, typename same_type<M, impl>::type>::type type;
};

// The reducer
template<typename M>
struct reduce {
    typedef typename reduce_impl<M, False>::type type;
};

// TEST

template<int X>
struct Integer {
    static const int value = X;
};

struct IntegerAddOneTag {};

template<int Z>
struct beta<appl<IntegerAddOneTag, Integer<Z> > > {
    typedef Integer<Z + 1> type;
};

struct Var_A {};
struct Var_B {};
struct Var_F {};
struct Var_N {};
struct Var_M {};
struct Var_X {};
struct Var_Y {};
struct Var_Self {};
typedef abst<Var_X, abst<Var_Y, Var_Y> > LC_False;
typedef abst<Var_X, abst<Var_Y, Var_X> > LC_True;

typedef abst<Var_A, Var_A> LC_Id;

typedef abst<Var_A, abst<Var_B, appl<appl<Var_A, LC_True>, Var_B> > > LC_or;

typedef appl<abst<Var_X, appl<abst<Var_X, Var_X>, Integer<3> > >, Integer<7> > LC_Capture_Test;

// church numeral
typedef abst<Var_F, abst<Var_X, Var_X> > LC_zero;
typedef abst<Var_N, abst<Var_F, abst<Var_X, appl<Var_F, appl<appl<Var_N, Var_F>, Var_X> > > > > LC_succ;
typedef appl<LC_succ, LC_zero> LC_one;
typedef appl<LC_succ, LC_one> LC_two;
typedef appl<LC_succ, LC_two> LC_three;

typedef abst<Var_M, abst<Var_N, appl<appl<Var_M, LC_succ>, Var_N> > > LC_add;
typedef appl<appl<LC_add, LC_three>, LC_three> LC_six;

typedef abst<Var_M, abst<Var_N, appl<appl<Var_M, appl<LC_add, Var_N> >, LC_zero> > > LC_mul;
typedef appl<appl<LC_mul, LC_six>, LC_six> LC_thirtysix;

// strict Y
// Z = λf.(λx.f (λv.((x x) v))) (λx.f (λv.((x x) v)))
typedef abst<Var_X, appl<Var_F, abst<Var_Y, appl<appl<Var_X, Var_X>, Var_Y> > > > LC_Z_impl;
typedef abst<Var_F, appl<LC_Z_impl, LC_Z_impl> > Y;

#include <cstdio>

int main() {
    printf("%d\n", reduce<appl<appl<LC_thirtysix, IntegerAddOneTag>, Integer<0> > >::type::value);
    return 0;
}
