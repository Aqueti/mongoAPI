/******************************************************************************
 *
 * \file MongoDatabaseClient.cpp
 * \brief 
 * \author Andrew Ferg
 *
 * Copyright Aqueti 2017
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 *****************************************************************************/

#include "MongoDatabaseClient.h"

MongoDatabaseClient::MongoDatabaseClient(mongocxx::pool& pool, std::string database)
    : m_client(pool.acquire())
{
    m_db = (*m_client)[database];
}
