#include "utils.h"
#include "qst_parser.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QRegularExpression>
#include <QDirIterator>
#include <QDebug>

bool generateQuestJson(const QString &inputDirectoryPath)
{
    // Define the output file path where the JSON data will be saved
    QString outputFilePath = QDir::currentPath() + "/resources/quests.json";
    QMap<QString, QstFile> questData;
    QDir dir(inputDirectoryPath);

    // Check if the input directory exists
    if (!dir.exists()) {
        qWarning() << "Directory does not exist:" << inputDirectoryPath;
        return false;
    }

    // Initialize an iterator to traverse all .qst files in subdirectories
    // QDirIterator::Subdirectories flag ensures recursive search
    QDirIterator it(inputDirectoryPath, QStringList() << "*.qst", QDir::Files, QDirIterator::Subdirectories);
    QStringList questFiles;

    // Collect all .qst file paths into a list for processing
    while (it.hasNext()) {
        questFiles.append(it.next());
    }

    qDebug() << "Found" << questFiles.size() << ".qst files in directory:" << inputDirectoryPath;

    // If no .qst files are found, log a warning and exit
    if (questFiles.isEmpty()) {
        qWarning() << "No .qst files found in directory:" << inputDirectoryPath;
        return false;
    }

    // Parse each .qst file and extract quest data
    for (const QString &filePath : questFiles) {
        QstFile qstFile(filePath);
        if (qstFile.parse()) {
            // Obtain the quest hash and format it as a hexadecimal string with leading zeros
            // This serves as a unique identifier for each quest
            QString questIdStr = QString("0x%1").arg(qstFile.getQuestHash(), 8, 16, QChar('0'));

            // Store the parsed quest data in a map with the quest ID as the key
            questData[questIdStr] = qstFile;
            qDebug() << "Added quest:" << questIdStr << "Chapter:" << qstFile.getChapterName() << "Quest:" << qstFile.getQuestName();
        }
    }

    // If no quest data was parsed successfully, log a warning and exit
    if (questData.isEmpty()) {
        qWarning() << "No quest data was parsed. JSON will be empty.";
        return false;
    }

    QJsonObject jsonObject;

    // Convert the quest data into a JSON object
    for (auto it = questData.begin(); it != questData.end(); ++it) {
        QJsonObject questObj;
        // Add the chapter name and quest name to the JSON object
        questObj["Chapter"] = it.value().getChapterName();
        questObj["QuestName"] = it.value().getQuestName();

        // Use the quest ID as the key in the JSON object
        jsonObject[it.key()] = questObj;
    }

    // Open the output file for writing, truncating it if it already exists
    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Could not open output file for writing:" << outputFilePath;
        return false;
    }

    // Write the JSON data to the file with indentation for readability
    QJsonDocument jsonDoc(jsonObject);
    outputFile.write(jsonDoc.toJson(QJsonDocument::Indented));
    outputFile.close();

    qDebug() << "Quest data saved to" << outputFilePath << "successfully with" << questData.size() << "entries.";

    return true;
}

