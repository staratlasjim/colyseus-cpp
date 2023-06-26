/**
 * Created by Jim Carter personal on 6/26/23.
 *
 * This is an attempt at using Trait based programming vs
 * inheritance based programming.
 *
 * Typically, I would simply have an Interface HasIsSchema, which
 * would define a method "bool IsSchema()" and specific classes
 * would inherit from that via Polymorphism.
 */

#ifndef COLYSEUSCPP_HASISSCHEMA_HPP
#define COLYSEUSCPP_HASISSCHEMA_HPP

#include <type_traits>

template<typename... Ts> struct make_void { typedef void type;};
template<typename... Ts> using void_t = typename make_void<Ts...>::type;

// Base case: Assume type T does not have method IsSchema
template <typename T, typename = void>
struct has_IsSchema : std::false_type {};

// Specialization: true if type T has method IsSchema
template <typename T>
struct has_IsSchema<T, void_t<decltype(std::declval<T>().IsSchema())>> : std::true_type {};

class HasIsSchema {
public:
    HasIsSchema(): m_isSchema(false) {}
    inline bool IsSchema() const { return m_isSchema; };

protected:
    inline void SetIsSchema(bool isSchema) { m_isSchema = isSchema; };

private:
    bool m_isSchema;
};


#endif //COLYSEUSCPP_HASISSCHEMA_HPP
