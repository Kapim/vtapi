#pragma once

#include <ctime>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "../common/vtapi_types.h"
#include "../common/vtapi_tkey.h"
#include "../data/vtapi_intervalevent.h"
#include "../data/vtapi_processstate.h"

namespace vtapi {


/**
 * @brief Class provides interface to the result set object
 *
 * New result set is inserted via newResult method. Iterating over rows is handled
 * by incrementing pos attribute via step() or setPosition() methods. Field values
 * can be obtained via various getX methods.
 */
class ResultSet
{
public:
    /**
     * Constructor
     * @param dbtypes preloaded map of database types
     */
    explicit ResultSet(DBTYPES_MAP *dbtypes)
        : _pdbtypes(dbtypes), _pos(-1), _res(NULL) {}

    /**
     * Virtual destructor
     */
    virtual ~ResultSet() { }

    /**
     * Destroy old result object and replace it with new one
     * @param res new result object
     */
    virtual void newResult(void *res) = 0;

    /**
     * Gets current row position within result set
     * @return row position
     */
    int getPosition()
    { return _pos; }

    /**
     * Sets row position within result set
     * @param pos row position
     */
    void setPosition(const int pos)
    { _pos = pos; }

    /**
     * Increments row position within result set
     */
    void incPosition()
    { _pos++; }

    /**
     * Gets number of rows within result set
     * @return number of rows
     */
    virtual int countRows() = 0;

    /**
     * Gets number of columns within result set
     * @return number of columns
     */
    virtual int countCols() = 0;

    /**
     * Checks whether result set object is valid
     * @return success
     */
    virtual bool isOk() = 0;

    /**
     * Clears result set object
     */
    virtual void clear() = 0;

   /**
     * Get key of a single table column
     * @param col Column index
     * @return Column key
     */
    virtual TKey getKey(int col) = 0;

    /**
     * Get a list of all possible columns as TKeys
     * @return list of keys
     */
    virtual TKeys *getKeys() = 0;

    /**
     * Gets type of given column
     * @param col column index
     * @return type name
     */
    virtual std::string getKeyType(const int col) = 0;

    /**
     * Gets index of given column name
     * @param key column name
     * @return column index
     */
    virtual int getKeyIndex(const std::string& key) = 0;

    // =============== GETTERS (Select) ========================================

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col column index
     * @return string representation of field value
     */
    virtual std::string getValue(const int col) = 0;

    // =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===============
    /**
     * Get single character specified by a column key
     * @param key column key
     * @return character
     */
    char getChar(const std::string& key)
    { return this->getChar(this->getKeyIndex(key)); }

    /**
     * Get single character specified by column index
     * @param col column index
     * @return character
     */
    virtual char getChar(const int col) = 0;

    /**
     * Get a character array specified by a column key
     * @param key column key
     * @return character array
     */
    std::string getString(const std::string& key)
    { return this->getString(this->getKeyIndex(key)); }

    /**
     * Get a string value specified by an index of a column
     * @param col column index
     * @return string value
     */
    virtual std::string getString(const int col) = 0;

    // =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==============
    /**
     * Get a boolean value specified by a column key
     * @param key column key
     * @return integer value
     */
    bool getBool(const std::string& key)
    { return this->getBool(this->getKeyIndex(key)); }

    /**
     * Get a boolean value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    virtual bool getBool(const int col) = 0;

    /**
     * Get an integer value specified by a column key
     * @param key column key
     * @return integer value
     */
    int getInt(const std::string& key)
    { return this->getInt(this->getKeyIndex(key)); }

    /**
     * Get an integer value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    virtual int getInt(const int col) = 0;

    /**
     * Get long integer value specified by a column key
     * @param key column key
     * @return long integer value
     */
    long long getInt8(const std::string& key)
    { return this->getInt8(this->getKeyIndex(key)); }

    /**
     * Get long integer value specified by an index of a column
     * @param col index of column
     * @return long integer value
     */
    virtual long long getInt8(const int col) = 0;

    /**
     * Get an array of integer values specified by a column key
     * @param key column key
     * @param size size of the array of integer values
     * @return array of integer values
     */
    int *getIntA(const std::string& key, int& size)
    { return this->getIntA(this->getKeyIndex(key), size); }

    /**
     * Get an array of integer values specified by an index of a column
     * @param col index of column
     * @param size size of the array of integer values
     * @return array of integer values
     */
    virtual int *getIntA(const int col, int& size) = 0;

    /**
     * Get a vector of integer values specified by a column key
     * @param key column key
     * @return vector of integer values
     */
    std::vector<int> *getIntV(const std::string& key)
    { return this->getIntV(this->getKeyIndex(key)); }

    /**
     * Get a vector of integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<int> *getIntV(const int col) = 0;

    /**
     * Get an array of long integer values specified by a column key
     * @param key column key
     * @param size size of the array of integer values
     * @return array of integer values
     */
    long long *getInt8A(const std::string& key, int& size)
    { return this->getInt8A(this->getKeyIndex(key), size); }

    /**
     * Get an array of long integer values specified by an index of a column
     * @param col index of column
     * @param size size of the array of integer values
     * @return array of integer values
     */
    virtual long long *getInt8A(const int col, int& size) = 0;

    /**
     * Get a vector of long integer values specified by a column key
     * @param key column key
     * @return vector of integer values
     */
    std::vector<long long> *getInt8V(const std::string& key)
    { return this->getInt8V(this->getKeyIndex(key)); }

    /**
     * Get a vector of long integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<long long> *getInt8V(const int col) = 0;

    // =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ==================
    /**
     * Get a float value specified by a column key
     * @param key column key
     * @return float value
     */
    float getFloat(const std::string& key)
    { return this->getFloat(this->getKeyIndex(key)); }

    /**
     * Get a float value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    virtual float getFloat(const int col) = 0;

    /**
     * Get a double value specified by a column key
     * @param key column key
     * @return double value
     */
    double getFloat8(const std::string& key)
    { return this->getFloat8(this->getKeyIndex(key)); }

    /**
     * Get a double value specified by an index of a column
     * @param col index of column
     * @return double value
     */
    virtual double getFloat8(const int col) = 0;

    /**
     * Get an array of float values specified by a column key
     * @param key column key
     * @param size size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const std::string& key, int& size)
    { return this->getFloatA(this->getKeyIndex(key), size); }

    /**
     * Get array of float values specified by index of column
     * @param col index of column
     * @param size size of the array of float values
     * @return array of float values
     */
    virtual float *getFloatA(const int col, int& size) = 0;

    /**
     * Get a vector of float values specified by the column index
     * @param key column key
     * @return vector of float values
     */
    std::vector<float> *getFloatV(const std::string& key)
    { return this->getFloatV(this->getKeyIndex(key)); }

    /**
     * Get a vector of float values specified by column key
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<float> *getFloatV(const int col) = 0;

    /**
     * Get an array of double values specified by a column key
     * @param key column key
     * @param size size of the array of float values
     * @return array of float values
     */
    double* getFloat8A(const std::string& key, int& size)
    { return this->getFloat8A(this->getKeyIndex(key), size); }

    /**
     * Get array of double values specified by index of column
     * @param col index of column
     * @param size size of the array of float values
     * @return array of float values
     */
    virtual double *getFloat8A(const int col, int& size) = 0;

    /**
     * Get a vector of double values specified by the column index
     * @param key column key
     * @return vector of float values
     */
    std::vector<double> *getFloat8V(const std::string& key)

    { return this->getFloat8V(this->getKeyIndex(key)); }

    /**
     * Get a vector of double values specified by column key
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<double> *getFloat8V(const int col) = 0;

    // =============== GETTERS - TIMESTAMP =====================================
    /**
     * Get timestamp specified by column key
     * @param key column key
     * @return Timestamp info
     */
    time_t getTimestamp(const std::string& key)
    { return this->getTimestamp(this->getKeyIndex(key)); }

    /**
     * Get timestamp specified by the column index
     * @param col column index
     * @return Timestamp info
     */
    virtual time_t getTimestamp(const int col) = 0;

    // =============== GETTERS - OpenCV MATRICES ===============================
    /**
     * Get OpenCv matrix (cv::Mat) specified by the column key
     * @param key column key
     * @return CvMat structure
     */
    cv::Mat *getCvMat(const std::string& key)
    { return this->getCvMat(this->getKeyIndex(key)); }

    /**
     * Get OpenCv matrix (cvMat) specified by the column index
     * @param col column index
     * @return CvMat structure
     */
    virtual cv::Mat *getCvMat(const int col) = 0;

// =============== GETTERS - GEOMETRIC TYPES ===============================
    /**
     * Get 2D point specified by the column key
     * @param key column key
     * @return 2D Point
     */
    Point getPoint(const std::string& key)
    { return this->getPoint(this->getKeyIndex(key)); }

    /**
     * Get 2D point specified by the column index
     * @param col column index
     * @return 2D Point
     */
    virtual Point getPoint(const int col) = 0;

    /**
     * Get array of 2D points specified by the column index
     * @param key column index
     * @return vector of 2D Points
     */
    Point *getPointA(const std::string& key, int& size)
    { return this->getPointA(this->getKeyIndex(key), size); }

    /**
     * Get array of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    virtual Point *getPointA(const int col, int& size) = 0;

    /**
     * Get vector of 2D points specified by the column index
     * @param key column index
     * @return vector of 2D Points
     */
    std::vector<Point>*  getPointV(const std::string& key)
    { return this->getPointV(this->getKeyIndex(key)); }

    /**
     * Get vector of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    virtual std::vector<Point>*  getPointV(const int col) = 0;

    // =============== GETTERS - CUSTOM COMPOSITE TYPES ========================
    /**
     * Get interval event by column key
     * @param key column key
     * @return interval event class
     */
    IntervalEvent *getIntervalEvent(const std::string& key)
    { return this->getIntervalEvent(this->getKeyIndex(key)); }

    /**
     * Get interval event by column index
     * @param col column index
     * @return interval event class
     */
    virtual IntervalEvent *getIntervalEvent(const int col) = 0;

    /**
     * Gets process state by a column key
     * @param key   column key
     * @return process state class
     */
    ProcessState *getProcessState(const std::string& key)
    { return this->getProcessState(this->getKeyIndex(key)); }

    /**
     * Gets process state by an index of a column
     * @param col   index of the column
     * @return process state class
     */
    virtual ProcessState *getProcessState(const int col) = 0;

    // =============== GETTERS - OTHER =========================================

    /**
     * Gets binary data by a column key
     * @param key   column key
     * @param size size of output data
     * @return allocated data
     */
    void *getBlob(const std::string& key, int &size)
    { return this->getBlob(this->getKeyIndex(key), size); }

    /**
     * Gets binary data by a column index
     * @param col   column index
     * @param size size of output data
     * @return allocated data
     */
    virtual void *getBlob(const int col, int &size) = 0;

protected:
    DBTYPES_MAP     *_pdbtypes;       /**< map of database types definitions */
    int             _pos;            /**< position within resultset */
    void            *_res;           /**< result object */

};


} // namespace vtapi