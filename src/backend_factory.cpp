
#include <vtapi_global.h>
#include <backends/vtapi_backendfactory.h>

using namespace vtapi;
    
    
BackendFactory vtapi::g_BackendFactory;


bool BackendFactory::initialize(const string& backendType) {
   if (backendType.compare("sqlite") == 0) {
       backend = SQLITE;
       return VT_OK;
   }
   else if (backendType.compare("postgres") == 0) {
       backend = POSTGRES;
       return VT_OK;
   }
   else {
       backend = UNKNOWN;
       return VT_FAIL;
   }
};


Connection* BackendFactory::createConnection(fmap_t *fmap, const string& connectionInfo, Logger *logger) {
    Connection *connection = NULL;
    switch (backend) {
        case POSTGRES:
#ifdef HAVE_POSTGRESQL
            connection = new PGConnection(fmap, connectionInfo, logger);
#endif
            break;
        case SQLITE:
#ifdef HAVE_SQLITE
       connection = new SLConnection(fmap, connectionInfo, logger);
#endif
            break;
        default:
            break;
    }
    return connection;
};

TypeManager* BackendFactory::createTypeManager(fmap_t *fmap, Connection *connection, Logger *logger) {
    TypeManager *typeManager = NULL;
    switch (backend) {
        case POSTGRES:
#ifdef HAVE_POSTGRESQL
       typeManager = new PGTypeManager(fmap, connection, logger);
 #endif
           break;
        case SQLITE:
#ifdef HAVE_SQLITE
        typeManager = new SLTypeManager(fmap, connection, logger);
#endif
            break;
        default:
            break;
    }
    return typeManager;
};

QueryBuilder* BackendFactory::createQueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger, const string& initString) {
    QueryBuilder *queryBuilder = NULL;
    switch (backend) {
        case POSTGRES:
#ifdef HAVE_POSTGRESQL
       queryBuilder = new PGQueryBuilder(fmap, connection, logger, initString);
#endif
          break;
        case SQLITE:
#ifdef HAVE_SQLITE
            queryBuilder = new SLQueryBuilder(fmap, connection, logger, initString);
#endif
            break;
        default:
            break;
    }
    return queryBuilder;
};

ResultSet* BackendFactory::createResultSet(fmap_t *fmap, TypeManager *typeManager, Logger *logger) {
    ResultSet *resultSet = NULL;
    switch (backend) {
        case POSTGRES:
#ifdef HAVE_POSTGRESQL
     resultSet = new PGResultSet(fmap, typeManager, logger);
#endif
         break;
        case SQLITE:
#ifdef HAVE_SQLITE
            resultSet = new SLResultSet(fmap, typeManager, logger);
#endif
            break;
        default:
            break;
    }
    return resultSet;
};

LibLoader* BackendFactory::createLibLoader(Logger *logger) {
    LibLoader *libLoader = NULL;
    switch (backend) {
        case POSTGRES:
#ifdef HAVE_POSTGRESQL
      libLoader = new PGLibLoader(logger);
 #endif
           break;
        case SQLITE:
#ifdef HAVE_SQLITE
            libLoader = new SLLibLoader(logger);
#endif
            break;
        default:
            break;
    }
    return libLoader;
};