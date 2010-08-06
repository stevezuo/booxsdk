// Author: John

#ifndef CMS_UTILS_H_
#define CMS_UTILS_H_

#include <vector>
#include <algorithm>
#include <QtCore/QtCore>

namespace cms
{


const QString & dateFormat();

/// Retrieve database path name from content file path.
/// \param content_path The absolute content file path.
/// \param database_path The returned database path.
/// \return It returns false if it the database does not exist.
/// \Note: This function  is used to query content database.
/// When using thumbnail or sketch, you should call get
bool getDatabasePath(const QString & content_path,
                     QString & database_path);

/// This function returns the thumbnail database path for given directory.
/// It returns empty string if it can not create the needed directory.
/// This function does NOT create the thumbnail database.
QString getThumbDB(const QString & dir_path);


/// This function returns the sketch database path for given directory.
/// It returns empty string if the desired directory can not be created.
QString getSketchDB(const QString & file_path);

/// This function returns system internal sketch database path
/// for given note name. The name serves as a index to internal sketch database.
QString internalSketchDBPath(const QString & name);

/// Return the sketch database postfix.
QString sketchPostfix();

/// Return true if a document could support metadata.
bool couldContainMetadata(const QString &path);

template<typename T>
bool addToContainer(std::vector<T> & container, const T & data)
{
    typename std::vector<T>::iterator it;
    it = std::find(container.begin(), container.end(), data);
    if (it == container.end())
    {
        container.push_back(data);
        return true;
    }
    return false;
}

template<typename T>
bool removeFromContainer(std::vector<T> & container, const T & data)
{
    typename std::vector<T>::iterator it;
    it = std::find(container.begin(), container.end(), data);
    if (it != container.end())
    {
        container.erase(it);
        return true;
    }
    return false;
}

/// Scoped database class.
template<typename T> class ScopedDB
{
public:
    ScopedDB(T & t)
        : ref_(t)
    {
        ref_.open();
    }

    ~ScopedDB()
    {
        ref_.close();
    }
private:
    T & ref_;
};

}  // namespace cms

#endif  // CMS_UTILS_H_
