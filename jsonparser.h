#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QMap>
#include <QString>
#include "types.h"

/**
 * @class JsonParser
 * @brief A class responsible for reading and parsing quest data from a JSON file.
 *
 * JsonParser loads quest data from a specified JSON file and stores it in a map
 * for further use. Each quest entry includes details such as the chapter name
 * and quest name, indexed by a lowercase key.
 */
class JsonParser
{
public:
    /**
     * @brief Holds parsed quest data, indexed by lowercase keys.
     *
     * Each entry in the map represents a quest, with the key as the quest's
     * name in lowercase and the value as a QuestInfo struct containing
     * chapter and quest names.
     */
    QMap<QString, QuestInfo> questData;

    /**
     * @brief Reads and parses the quest data from a JSON file.
     *
     * This function opens the specified JSON file, reads its contents,
     * and populates the questData map with the extracted quest information.
     * If the file is missing or the JSON is invalid, an exception is thrown.
     *
     * @param filename The path to the JSON file to be parsed.
     */
    void read(const QString& filename);
};

#endif // JSONPARSER_H
