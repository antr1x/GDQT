#include "jsonparser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QException>

void JsonParser::read(const QString& filename)
{
    QFile file(filename);

    // Check if the file exists and open it in read-only mode.
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not open quests JSON file! Ensure that you have downloaded an existing quests data file or generated one. For details, refer to the Settings tab.";
        throw QException();
    }

    // Read all data from the file and close it after reading.
    QByteArray data = file.readAll();
    file.close();

    // Attempt to parse the JSON data; log and throw an error if parsing fails.
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "JSON parse error:" << parseError.errorString();
        throw QException();
    }

    // Ensure the parsed JSON is an object, as expected for the quests structure.
    if (!doc.isObject())
    {
        qWarning() << "JSON is not an object";
        throw QException();
    }

    // Process each key-value pair in the root JSON object.
    QJsonObject rootObj = doc.object();

    for (auto it = rootObj.begin(); it != rootObj.end(); ++it)
    {
        // Convert the key to lowercase to ensure consistent quest key formatting.
        QString key = it.key().toLower();

        // Extract quest details from the JSON object, populating a QuestInfo struct.
        QJsonObject questObj = it.value().toObject();
        QuestInfo info;
        info.Chapter = questObj.value("Chapter").toString();
        info.QuestName = questObj.value("QuestName").toString();

        // Insert the quest data into the map using the lowercase key.
        questData.insert(key, info);
    }
}
