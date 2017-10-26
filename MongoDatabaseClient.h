/******************************************************************************
 *
 * \file MongoDatabaseClient.h
 * \brief holds a mongo databse connection
 * \author Andrew Ferg
 *
 * Copyright Aqueti 2017
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 *****************************************************************************/

#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>

class MongoDatabaseClient
{
    public:
        MongoDatabaseClient(mongocxx::pool& pool, std::string database);

        mongocxx::database m_db;

    private:
        mongocxx::pool::entry m_client;
};
