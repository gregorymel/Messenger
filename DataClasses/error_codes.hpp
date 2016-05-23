#ifndef ERROR_CODES_HPP
#define ERROR_CODES_HPP

#include <system_error>

namespace m_error
{
    enum m_error_t
    {
        OK                      = 0,
        FAILURE,
        REGISTRATION_REQUIRED,
        NOT_AUTHORIZED,
        FEATURE_NOT_INPLEMENTED,
        DATA_BASE_TIMEOUT,
        CONNECTION_TIMEOUT,
        SERVICE_UNAVAILABLE,
        BAD_REQUEST,
        ITEM_NOT_FOUND,
        ALREADY_EXIST,
        INTERNAL_SERVER_ERROR,
        CORRUPTED_STANZA,
        INVALID_STANZA
    };

    class m_category_impl: public std::error_category
    {
    public:

        virtual const char* name() const noexcept;
        virtual std::string message( int ev ) const noexcept;
    };

    std::error_code make_error_code( m_error_t e );
    std::error_condition make_error_condition( m_error_t e );
}

const std::error_category& m_category();

namespace std
{
    template <>
    struct is_error_code_enum<m_error::m_error_t>: public true_type {};
}

#endif // ERROR_CODES_HPP
