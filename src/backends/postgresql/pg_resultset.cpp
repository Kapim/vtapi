
#include <libpq-fe.h>
#include <vtapi/common/vtapi_global.h>
#include <vtapi/common/vtapi_serialize.h>
#include "pg_resultset.h"

#define PGRES ((PGresult *)_res)


using namespace std;

namespace vtapi {


PGResultSet::PGResultSet(DBTYPES_MAP *dbtypes)
    : ResultSet(dbtypes)
{
}

PGResultSet::~PGResultSet()
{
    clear();
}

void PGResultSet::newResult(void *res)
{
    clear();
    _res = res;
}

int PGResultSet::countRows()
{
    return PGRES ? PQntuples(PGRES) : -1;
}

int PGResultSet::countCols()
{
    return PGRES ? PQnfields(PGRES) : -1;
}

bool PGResultSet::isOk()
{
    return PGRES ? true : false;
}

void PGResultSet::clear()
{
    if (PGRES) {
        PQclear(PGRES);
        _res = NULL;
    }
}

TKey PGResultSet::getKey(int col)
{
    string name = PQfname(PGRES, col);
    string type = getKeyType(col);

    return TKey(type, name, 0, "");
}

TKeys* PGResultSet::getKeys()
{
    TKeys* keys = new TKeys;

    int cols = PQnfields(PGRES);
    for (int col = 0; col < cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string PGResultSet::getKeyType(const int col)
{
    string type;
    if (_pdbtypes) {
        DBTYPES_MAP::iterator it = _pdbtypes->find(PQftype(PGRES, col));
        if (it != _pdbtypes->end()) {
            type = (*it).second.name;
        }
    }

    return type;
}

short PGResultSet::getKeyTypeLength(const int col, const short def)
{
    short length = def;
    if (_pdbtypes) {
        DBTYPES_MAP_IT it = _pdbtypes->find(PQftype(PGRES, col));
        if (it != _pdbtypes->end()) {
            length = DBTYPE_GETLENGTH((*it).second.type);
        }
    }

    return length;
}

int PGResultSet::getKeyIndex(const string& key)
{
    return PGRES ? PQfnumber(PGRES, key.c_str()) : -1;
}

// =============== SINGLE VALUES / ARRAYS / VECTORS TEMPLATES ===================

template<typename TDB, typename TOUT>
TOUT PGResultSet::getSingleValue(const int col, const char *def)
{
    TDB value = { 0 };
    if (!PQgetf(PGRES, _pos, def, col, &value)) {
        VTLOG_WARNING(string("Value is not a ") + def);
    }

    return (TOUT) value;
}

template<typename TDB, typename TOUT>
TOUT *PGResultSet::getArray(const int col, int& size, const char *def)
{
    TOUT *values = NULL;
    PGarray tmp = { 0 };

    do {
        char defArr[128];
        sprintf(defArr, "%s[]", def);
        if (!PQgetf(PGRES, _pos, defArr, col, &tmp)) {
            VTLOG_WARNING( string("Value is not an array of ") + def);
            break;
        }

        size = PQntuples(tmp.res);
        if (size == 0) break;

        values = new TOUT [size];
        if (!values) break;

        for (int i = 0; i < size; i++) {
            TDB value = { 0 };
            if (!PQgetf(tmp.res, i, def, 0, &value)) {
                VTLOG_WARNING( string("Unexpected value in array of ") + def);
                vt_destructall(values);
                break;
            }
            else {
                values[i] = (TOUT) value;
            }
        }
    } while (0);

    if (!values) size = 0;
    if (tmp.res) PQclear(tmp.res);

    return values;
}

template<typename TDB, typename TOUT>
vector<TOUT> *PGResultSet::getVector(const int col, const char *def)
{
    vector<TOUT> *values = NULL;
    PGarray tmp = { 0 };

    do {
        char defArr[128];
        sprintf(defArr, "%s[]", def);
        if (!PQgetf(PGRES, _pos, defArr, col, &tmp)) {
            VTLOG_WARNING( string("Value is not an array of ") + def);
            break;
        }

        int size = PQntuples(tmp.res);
        if (size == 0) break;

        values = new vector<TOUT>;
        if (!values) break;

        for (int i = 0; i < size; i++) {
            TDB value = { 0 };
            if (!PQgetf(tmp.res, i, def, 0, &value)) {
                VTLOG_WARNING( string("Unexpected value in array of ") + def);
                vt_destruct(values);
                break;
            }
            else {
                values->push_back((TOUT) value);
            }
        }
    } while (0);

    if (tmp.res) PQclear(tmp.res);

    return values;
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================

char PGResultSet::getChar(const int col)
{
    return getSingleValue<PGchar, char>(col, "%char");
}

string PGResultSet::getString(const int col)
{
    // no conversions with libpqtypes, just get the value
    char *value = PQgetvalue(PGRES, _pos, col);
    return value ? value : "";
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool PGResultSet::getBool(const int col)
{
    return getSingleValue<PGbool, bool>(col, "%bool");
}

int PGResultSet::getInt(const int col)
{
    return (int) getInt8(col);
}

long long PGResultSet::getInt8(const int col)
{
    long long value = 0;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        PGnumeric val = getSingleValue<PGnumeric, PGnumeric>(col, "%numeric");
        if (val) value = atoi(val);
        break;
    }
    case 2:
    {
        value = getSingleValue<PGint2, long long>(col, "%int2");
        break;
    }
    case 4:
    {
        value = getSingleValue<PGint4, long long>(col, "%int4");
        break;
    }
    case 8:
    {
        value = getSingleValue<PGint8, long long>(col, "%int8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return value;
}

int* PGResultSet::getIntA(const int col, int& size)
{
    int *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new int[size])) break;
            for (int i = 0; i < size; i++) {
                values[i] = atoi(vals[i]);
            }
            vt_destructall(vals);
        }
        break;
    }
    case 2:
    {
        values = getArray<PGint2, int>(col, size, "%int2");
        break;
    }
    case 4:
    {
        values = getArray<PGint4, int>(col, size, "%int4");
        break;
    }
    case 8:
    {
        values = getArray<PGint8, int>(col, size, "%int8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

vector<int>* PGResultSet::getIntV(const int col)
{
    vector<int> *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        int size = 0;
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new vector<int>)) break;
            for (int i = 0; i < size; i++) {
                values->push_back(atoi(vals[i]));
            }
            vt_destructall(vals);
        }
        break;
    }
    case 2:
    {
        values = getVector<PGint2, int>(col, "%int2");
        break;
    }
    case 4:
    {
        values = getVector<PGint4, int>(col, "%int4");
        break;
    }
    case 8:
    {
        values = getVector<PGint8, int>(col, "%int8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

long long* PGResultSet::getInt8A(const int col, int& size)
{
    long long *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1: // numeric
    {
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new long long[size])) break;
            for (int i = 0; i < size; i++) {
                values[i] = atoll(vals[i]);
            }
            vt_destructall(vals);
        }
        break;
    }
    case 2:
    {
        values = getArray<PGint2, long long>(col, size, "%int2");
        break;
    }
    case 4:
    {
        values = getArray<PGint4, long long>(col, size, "%int4");
        break;
    }
    case 8:
    {
        values = getArray<PGint8, long long>(col, size, "%int8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

vector<long long>* PGResultSet::getInt8V(const int col)
{
    vector<long long> *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1: // numeric
    {
        int size = 0;
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new vector<long long>)) break;
            for (int i = 0; i < size; i++) {
                values->push_back(atoll(vals[i]));
            }
            vt_destructall(vals);
        }
        break;
    }
    case 2:
    {
        values = getVector<PGint2, long long>(col, "%int2");
        break;
    }
    case 4:
    {
        values = getVector<PGint4, long long>(col, "%int4");
        break;
    }
    case 8:
    {
        values = getVector<PGint8, long long>(col, "%int8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float PGResultSet::getFloat(const int col)
{
    return (float) getFloat8(col);
}

double PGResultSet::getFloat8(const int col)
{
    double value = 0.0;

    switch (getKeyTypeLength(col, 4))
    {
    case -1: // numeric
    {
        PGnumeric val = getSingleValue<PGnumeric, PGnumeric>(col, "%numeric");
        if (val) value = atof(val);
        break;
    }
    case 4:
    {
        value = getSingleValue<PGfloat4, double>(col, "%float4");
        break;
    }
    case 8:
    {
        value = getSingleValue<PGfloat8, double>(col, "%float8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return value;
}

float* PGResultSet::getFloatA(const int col, int& size)
{
    float *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new float[size])) break;
            for (int i = 0; i < size; i++) {
                values[i] = (float) atof(vals[i]);
            }
            vt_destructall(vals);
        }
        break;
    }
    case 4:
    {
        values = getArray<PGfloat4, float>(col, size, "%float4");
        break;
    }
    case 8:
    {
        values = getArray<PGfloat8, float>(col, size, "%float8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

vector<float>* PGResultSet::getFloatV(const int col)
{
    vector<float> *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        int size = 0;
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new vector<float>)) break;
            for (int i = 0; i < size; i++) {
                values->push_back((float) atof(vals[i]));
            }
            vt_destructall(vals);
        }
        break;
    }
    case 4:
    {
        values = getVector<PGfloat4, float>(col, "%float4");
        break;
    }
    case 8:
    {
        values = getVector<PGfloat8, float>(col, "%float8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

double* PGResultSet::getFloat8A(const int col, int& size)
{
    double *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new double[size])) break;
            for (int i = 0; i < size; i++) {
                values[i] = atof(vals[i]);
            }
            vt_destructall(vals);
        }
        break;
    }
    case 4:
    {
        values = getArray<PGfloat4, double>(col, size, "%float4");
        break;
    }
    case 8:
    {
        values = getArray<PGfloat8, double>(col, size, "%float8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

vector<double>* PGResultSet::getFloat8V(const int col)
{
    vector<double> *values = NULL;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        int size = 0;
        PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
        if (vals) {
            if (!(values = new vector<double>)) break;
            for (int i = 0; i < size; i++) {
                values->push_back(atof(vals[i]));
            }
            vt_destructall(vals);
        }
        break;
    }
    case 4:
    {
        values = getVector<PGfloat4, double>(col, "%float4");
        break;
    }
    case 8:
    {
        values = getVector<PGfloat8, double>(col, "%float8");
        break;
    }
    default:
    {
        VTLOG_WARNING("Value has unknown length");
        break;
    }
    }

    return values;
}

cv::Mat *PGResultSet::getCvMat(const int col)
{
    PGresult *matres    = NULL;
    cv::Mat *mat        = NULL;
    PGarray mat_dims_arr = { 0 };
    int *mat_dim_sizes  = NULL;

    do {
        // get cvmat structure
        matres = getSingleValue<PGresult*, PGresult*>(col, "%public.cvmat");
        if (!matres) break;

        // get cvmat members
        PGint4 mat_type = 0;
        PGbytea mat_data_bytea = { 0 };
        if (! PQgetf(matres, 0, "%int4 %int4[] %bytea",
                         0, &mat_type, 1, &mat_dims_arr, 2, &mat_data_bytea)) {
            VTLOG_WARNING("Cannot get cvmat header");
            break;
        }

        // create dimensions array
        int mat_dims = PQntuples(mat_dims_arr.res);
        if (!mat_dims) break;

        mat_dim_sizes = new int[mat_dims];
        if (!mat_dim_sizes) break;

        for (int i = 0; i < mat_dims; i++) {
            PQgetf(mat_dims_arr.res, i, "%int4", 0, &mat_dim_sizes[i]);
        }

        // create matrix
        mat = new cv::Mat(mat_dims, mat_dim_sizes, mat_type);
        if (!mat) {
            VTLOG_WARNING("Failed to create cv::Mat");
            break;
        }

        // copy matrix data
        memcpy(mat->data, mat_data_bytea.data, mat_data_bytea.len);
    } while (0);

    if (mat_dims_arr.res) PQclear(mat_dims_arr.res);
    vt_destructall(mat_dim_sizes);
    if (matres) PQclear(matres);

    return mat;
}

// =============== GETTERS - GEOMETRIC TYPES ===============================
Point PGResultSet::getPoint(const int col)
{
    PGpoint pt = getSingleValue<PGpoint, PGpoint>(col, "%point");
    return *(reinterpret_cast<Point*>(&pt));
}

Point* PGResultSet::getPointA(const int col, int& size)
{
    return reinterpret_cast<Point*>(getArray<PGpoint, PGpoint>(col, size, "%point"));
}

vector<Point>*  PGResultSet::getPointV(const int col)
{
    return reinterpret_cast< vector<Point>* >(getVector<PGpoint, PGpoint>(col, "%point"));
}

// =============== GETTERS - TIMESTAMP =========================================

time_t PGResultSet::getTimestamp(const int col)
{
    PGtimestamp pgts = getSingleValue<PGtimestamp, PGtimestamp>(col, "%timestamp");

    struct tm ts = { 0 };
    ts.tm_year  = pgts.date.year;
    ts.tm_mon   = pgts.date.mon;
    ts.tm_mday  = pgts.date.mday;
    ts.tm_hour  = pgts.time.hour;
    ts.tm_min   = pgts.time.min;
    ts.tm_sec   = pgts.time.sec;
    ts.tm_zone  = "GMT0";

    return timegm(&ts);
}

IntervalEvent *PGResultSet::getIntervalEvent(const int col)
{
    PGresult *evres     = NULL;
    IntervalEvent *event = NULL;

    do {
        // get event structure
        evres = getSingleValue<PGresult*, PGresult*>(col, "%public.vtevent");
        if (!evres) break;

        // get event members
        PGint4 ev_group_id = 0, ev_class_id = 0;
        PGbool ev_is_root = false;
        PGbox ev_region = { 0 };
        PGfloat8 ev_score = 0.0;
        PGbytea ev_data = { 0 };
        if (! PQgetf(evres, 0, "%int4 %int4 %bool %box %float8 %bytea",
                         0, &ev_group_id, 1, &ev_class_id, 2, &ev_is_root, 3, &ev_region, 4, &ev_score, 5, &ev_data)) {
            VTLOG_WARNING("Cannot get vtevent header");
            break;
        }

        // create event
        event = new IntervalEvent();
        if (!event) {
            VTLOG_WARNING("Failed to create IntervalEvent");
            break;
        }

        event->group_id = ev_group_id;
        event->class_id = ev_class_id;
        event->is_root = ev_is_root;
        memcpy(&event->region, &ev_region, sizeof (ev_region));
        event->score = ev_score;
        event->user_data_size = ev_data.len;
        event->SetUserData(ev_data.data, ev_data.len);
    } while (0);

    if (evres) PQclear(evres);

    return event;
}

ProcessState *PGResultSet::getProcessState(const int col)
{
    PGresult *psres     = NULL;
    ProcessState *pstate = NULL;

    do {
        // get process state structure
        psres = getSingleValue<PGresult*, PGresult*>(col, "%public.pstate");
        if (!psres) break;

        // get event members
        PGvarchar ps_status = NULL, ps_curritem = NULL, ps_lasterror = NULL;
        PGfloat4 ps_progress = 0;
        if (! PQgetf(psres, 0, "%public.pstatus %float4 %varchar %varchar",
                         0, &ps_status, 1, &ps_progress, 2, &ps_curritem, 3, &ps_lasterror)) {
            VTLOG_WARNING("Cannot get pstate header");
            break;
        }

        // create event
        pstate = new ProcessState();
        if (!pstate) {
            VTLOG_WARNING("Failed to create ProcessState");
            break;
        }

        if (ps_status)   pstate->status = pstate->toStatusValue(ps_status);
        if (ps_curritem) pstate->currentItem = ps_curritem;
        if (ps_lasterror)pstate->lastError = ps_lasterror;
        pstate->progress = ps_progress;

    } while (0);

    if (psres) PQclear(psres);

    return pstate;
}

// ========================= GETTERS - OTHER ==================================

void *PGResultSet::getBlob(const int col, int &size)
{
    PGbytea value = getSingleValue<PGbytea, PGbytea>(col, "%bytea");
    size = value.len;

    return (void *) value.data;
}

// =======================UNIVERSAL GETTER=====================================


#define GET_AND_SERIALIZE_VALUE(FUNC) \
{\
    ret = toString(FUNC(col));\
}
#define GET_AND_SERIALIZE_VALUE_ALLOC(TYPE,FUNC) \
{\
    TYPE *value = FUNC(col);\
    if (value) {\
        ret = toString(*value);\
        delete value;\
    }\
}
#define GET_AND_SERIALIZE_PTR(TYPE, FUNC) \
{\
    int size = 0;\
    TYPE *value = FUNC(col, size);\
    if (value) {\
        ret = toString(value, size, 0);\
    }\
}
#define GET_AND_SERIALIZE_ARRAY(TYPE, FUNC) \
{\
    int size = 0;\
    TYPE *values = FUNC(col, size);\
    if (values) {\
        ret = toString(values, size, 0);\
        vt_destructall(values);\
    }\
}
#define GET_AND_SERIALIZE_VALUE_AND_ARRAY(TYPE, FUNC) \
{\
    if (DBTYPE_HASFLAG(dbtype, DBTYPE_FLAG_ARRAY)) {\
        GET_AND_SERIALIZE_ARRAY(TYPE, FUNC ## A);\
    }\
    else {\
        GET_AND_SERIALIZE_VALUE(FUNC);\
    }\
}

string PGResultSet::getValue(const int col)
{
    string ret;

    DBTYPE dbtype = 0;
    if (_pdbtypes) {
        DBTYPES_MAP_IT it = _pdbtypes->find(PQftype(PGRES, col));
        if (it != _pdbtypes->end()) {
            dbtype = (*it).second.type;
        }
    }

    switch (DBTYPE_GETCATEGORY(dbtype))
    {
    case DBTYPE_STRING:
    {
        GET_AND_SERIALIZE_VALUE(getString);
        break;
    }
    case DBTYPE_INT:
    {
        GET_AND_SERIALIZE_VALUE_AND_ARRAY(long long, getInt8);
        break;
    }
    case DBTYPE_FLOAT:
    {
        GET_AND_SERIALIZE_VALUE_AND_ARRAY(double, getFloat8);
        break;
    }
    case DBTYPE_BOOLEAN:
    {
        GET_AND_SERIALIZE_VALUE(getBool);
        break;
    }
    case DBTYPE_BLOB:
    {
        GET_AND_SERIALIZE_PTR(void, getBlob);
        break;
    }
    case DBTYPE_TIMESTAMP:
    {
        GET_AND_SERIALIZE_VALUE(getTimestamp);
        break;
    }
    case DBTYPE_GEO_POINT:
    {
        GET_AND_SERIALIZE_VALUE_AND_ARRAY(Point, getPoint);
        break;
    }
    case DBTYPE_GEO_LSEG:
    {
        break;
    }
    case DBTYPE_GEO_PATH:
    {
        break;
    }
    case DBTYPE_GEO_BOX:
    {
        break;
    }
    case DBTYPE_GEO_POLYGON:
    {
        break;
    }
    case DBTYPE_GEO_LINE:
    {
        break;
    }
    case DBTYPE_GEO_CIRCLE:
    {
        break;
    }
    case DBTYPE_GEO_GEOMETRY:
    {
        // PostGIS geometry type
        break;
    }
    case DBTYPE_UD_SEQTYPE:
    {
        GET_AND_SERIALIZE_VALUE(getString);
        break;
    }
    case DBTYPE_UD_INOUTTYPE:
    {
        GET_AND_SERIALIZE_VALUE(getString);
        break;
    }
    case DBTYPE_UD_PSTATUS:
    {
        GET_AND_SERIALIZE_VALUE(getString);
        break;
    }
    case DBTYPE_UD_CVMAT:
    {
        GET_AND_SERIALIZE_VALUE_ALLOC(cv::Mat, getCvMat);
        break;
    }
    case DBTYPE_UD_EVENT:
    {
        GET_AND_SERIALIZE_VALUE_ALLOC(IntervalEvent, getIntervalEvent);
        break;
    }
    case DBTYPE_UD_PSTATE:
    {
        GET_AND_SERIALIZE_VALUE_ALLOC(ProcessState, getProcessState);
        break;
    }
    case DBTYPE_REF_TYPE:
    {
        break;
    }
    case DBTYPE_REF_CLASS:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}


}