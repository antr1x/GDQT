#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QMap>

/**
 * @brief Generates a JSON file containing quest data from .qst files in the specified directory.
 *
 * This function recursively searches for all .qst files in the given directory,
 * parses each file to extract quest information, and saves the collected data
 * in a JSON file at the path `resources/quests.json`. Each quest is uniquely identified
 * by a hash, and the JSON data includes both the chapter name and quest name for each entry.
 *
 * @param inputDirectoryPath The directory to search for .qst files.
 * @return True if the JSON file was generated successfully; otherwise, false.
 */
bool generateQuestJson(const QString &inputDirectoryPath);

#endif // UTILS_H
