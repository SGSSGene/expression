#include <tuple>
#include <memory>
#include <functional>
#include <map>

namespace expression {
namespace details {

template <typename T, typename ...Args>
struct Unpack {
    auto operator()(std::conditional_t<true, std::function<T()>, Args> const&... args) {
        return std::make_tuple(args()...);
    }
};

template <typename T>
struct none {
    T operator()() {
        return T{};
    }
};
struct identity {
    template <typename T1>
    auto operator()(T1 v1) {
        return v1;
    }
};

struct multiplies {
    template <typename T1, typename T2>
    auto operator()(T1 l, T2 r) {
        return l*r;
    }
};

struct plus {
    template <typename T1, typename T2>
    auto operator()(T1 l, T2 r) {
        return l+r;
    }
};
struct minus {
    template <typename T1, typename T2>
    auto operator()(T1 l, T2 r) {
        return l-r;
    }
};
struct devide {
    template <typename T1, typename T2>
    auto operator()(T1 l, T2 r) {
        return l/r;
    }
};

template <typename T, typename OP, typename ...Args>
class ExpressionImpl {
    std::tuple<std::conditional_t<true, std::function<T()>, Args>...> mSources;

    mutable bool mCached{false};
    mutable T mCache{0.};

    std::tuple<std::shared_ptr<Args>...> mDepends;

 public:
    mutable std::map<void*, std::function<void()>> onChange;

    ExpressionImpl(T _value)
        : mCached{true}
        , mCache{_value}
    {}

    ExpressionImpl(std::shared_ptr<Args>... _args) {
        mDepends = std::make_tuple(_args...);

        auto helperF1 = [=](auto t) {
            t->onChange[(void*)this] = [this]() {
                mCached = false;
                for (auto const& f : onChange) {
                    f.second();
                }
            };
        };
        (helperF1(_args), ...);

        auto helperF2 = [](auto t) {
            return [t]() -> T {
                return t->getValue();
            };
        };

        mSources = std::make_tuple(helperF2(_args)...);
    }

    ~ExpressionImpl() {
        auto helperF1 = [=](auto t) {
            t->onChange.erase((void*)this);
        };
        auto helperF2 = [=](auto... args) {
            (helperF1(args), ...);
        };
        std::apply(helperF2, mDepends);
    }

    auto getValue() const {
        if (not mCached) {
            auto unpack = Unpack<T, Args...>();


            auto evaluatedSources = std::apply(unpack, mSources);

            auto op = OP();
            mCache = std::apply(op, evaluatedSources);

            mCached = true;
        }
        return mCache;
    }
    auto operator=(T _value) -> ExpressionImpl& {
        mCache = _value;
        mCached = true;
        for (auto const& f : onChange) {
            f.second();
        }
        return *this;
    }

};

template <typename T, typename OP = none<T>, typename ...Args>
class Expression {
public:
    using Impl = ExpressionImpl<T, OP, typename Args::Impl...>;
    std::shared_ptr<Impl> mImpl;

public:
    Expression(T _value)
        : mImpl { std::make_shared<Impl>(_value)}
    {}

    Expression(Args... _args)
        : mImpl { std::make_shared<Impl>(_args.mImpl...)}
    {}

    auto getValue() const {
        return mImpl->getValue();
    }
    auto operator=(T _value) -> Expression& {
        *mImpl = _value;
        return *this;
    }
};

#define OPERATOR(op, opClass) \
template <typename T1, typename T2, typename ...Args2> \
auto operator op(T1 _value, Expression<T2, Args2...> r) { \
    Expression<T1> l {_value}; \
    using ResultT = decltype(opClass()(std::declval<T1>(), std::declval<T2>())); \
    return Expression<ResultT, opClass, Expression<T1>, Expression<T2, Args2...>>{l, r}; \
} \
template <typename T1, typename ...Args1, typename T2> \
auto operator op(Expression<T1, Args1...> l, T2 _value) { \
    Expression<T2> r {_value}; \
    using ResultT = decltype(opClass()(std::declval<T1>(), std::declval<T2>())); \
    return Expression<ResultT, opClass, Expression<T1, Args1...>, Expression<T2>>{l, r}; \
} \
\
template <typename T1, typename ...Args1, typename T2, typename ...Args2> \
auto operator op(Expression<T1, Args1...> l, Expression<T2, Args2...> r) { \
    using ResultT = decltype(opClass()(std::declval<T1>(), std::declval<T2>())); \
    return Expression<ResultT, opClass, Expression<T1, Args1...>, Expression<T2, Args2...>>{l, r}; \
} \



OPERATOR(*, multiplies)
OPERATOR(+, plus)
OPERATOR(-, minus)
OPERATOR(/, devide)

Expression<double> operator "" _v(long double x) {
    return Expression<double>{static_cast<double>(x)};
}
} // namespace details

using details::Expression;
using details::operator ""_v;
} // namespace expression
