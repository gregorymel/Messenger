#include <error_codes.hpp>

const char* m_error::m_category_impl::name() const noexcept
{
    return "messenger";
}

std::string m_error::m_category_impl::message( int ev ) const noexcept
{
    switch ( ev )
    {
    case OK:
        return "OK";
    case FAILURE:
        return "failure";
    case REGISTRATION_REQUIRED:
        return "registration required";
    case NOT_AUTHORIZED:
        return "not authorized";
    case FEATURE_NOT_INPLEMENTED:
        return "feature not implemented";
    case DATA_BASE_TIMEOUT:
        return "data base access timed out";
    case CONNECTION_TIMEOUT:
        return "connection timed out";
    case SERVICE_UNAVAILABLE:
        return "service is currently unavailable";
    case BAD_REQUEST:
        return "bad request";
    case ITEM_NOT_FOUND:
        return "item not found";
    case ALREADY_EXIST:
        return "already exist";
    case INTERNAL_SERVER_ERROR:
        return "internal server error";
    case CORRUPTED_STANZA:
        return "corrupted stanza";
    case INVALID_STANZA:
        return "invalid stanza";
    default:
        return "Unknown error";
    }
}

std::error_code m_error::make_error_code( m_error_t e )
{
    return std::error_code( e, m_category() );
}

std::error_condition m_error::make_error_condition( m_error_t e )
{
    return std::error_condition( e, m_category() );
}

const std::error_category& m_category()
{
    static m_error::m_category_impl m_category_instance;
    return m_category_instance;
}

