/* 
 * File:   vtapi_query.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:59 PM
 */

#ifndef VTAPI_QUERY_H
#define	VTAPI_QUERY_H

namespace vtapi {
    class Query;
}

#include "../vtapi_commons.h"
#include "../vtapi_backends.h"

namespace vtapi {
    

/**
 * @brief Base query class
 *
 * TODO: It will be used for delayed queries (store())
 *
 * @note Error codes 20*
 */
class Query : public Commons {
public:

    QueryBuilder        *queryBuilder;  /**< Object implementing interface for building queries */
    ResultSet           *resultSet;     /**< Object implementing result set interface */
    bool                executed;       /**< Flag, disable on any change to query, enable on execute */

public:

    /**
     * Construct a query object
     * @param commons pointer of the existing commons object
     * @param query query string
     * @param param parameters for passing to the query
     */
    Query(const Commons& commons, const string& initString = "");//, PGparam *param = NULL);
    /**
     * Destructor
     */
    ~Query();
    /**
     * This expands the query, so you can check it before the execution
     * @return string value with the query
     */
    string getQuery();

    /**
     * This will commit your query
     * @return success of the query
     */
    bool execute();
    /**
     * Clears the query object to its original state
     * @return
     */
    bool reset();

};

} // namespace vtapi

#endif	/* VTAPI_QUERY_H */
