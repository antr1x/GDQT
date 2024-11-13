#include "settings.h"
#include "questtrackerwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

Settings::Settings(QuestTrackerWindow *window, const QString &settingsFilePath)
    : m_settingsFilePath(settingsFilePath)
    , m_window(window)
{
    load();
}

bool Settings::load()
{
    QFile file(m_settingsFilePath);

    // Try to open the settings file for reading
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open settings file for reading:" << m_settingsFilePath;

        // Clear values if settings file does not exist
        m_saveDirPath.clear();
        m_questsFilePath.clear();
        m_qstFilesDirPath.clear();
        m_characterName.clear();
        m_theme = Theme::availableThemeNames().first(); // Set to default theme
    } else {
        QByteArray data = file.readAll();
        file.close();

        // Parse JSON from the file and validate format
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
            qWarning() << "Invalid JSON format in settings file:" << m_settingsFilePath;

            // Clear values due to invalid JSON format
            m_saveDirPath.clear();
            m_questsFilePath.clear();
            m_qstFilesDirPath.clear();
            m_characterName.clear();
            m_theme = Theme::availableThemeNames().first(); // Default theme
        } else {
            QJsonObject obj = doc.object();

            // Retrieve values from JSON and set class members
            m_saveDirPath = obj.value("saveDirPath").toString();
            m_questsFilePath = obj.value("questsFilePath").toString();
            m_qstFilesDirPath = obj.value("qstFilesDirPath").toString();
            m_characterName = obj.value("characterName").toString();
            m_theme = obj.value("theme").toString();

            // Validate theme against available themes
            if (!Theme::availableThemeNames().contains(m_theme)) {
                m_theme = Theme::availableThemeNames().first(); // Revert to default if invalid
            }
        }
    }

    // Check if the quests file exists; set default if necessary
    QFile questsFile(m_questsFilePath);
    if (m_questsFilePath.isEmpty() || !questsFile.exists()) {
        checkAndSetDefaultQuestsFilePath();
    }

    // Update the UI with loaded values
    m_window->updateSaveDirPath(m_saveDirPath);
    m_window->updateQuestsFilePath(m_questsFilePath);
    m_window->updateQstFilesDirPath(m_qstFilesDirPath);
    m_window->updateCharacterComboBox(getAvailableCharacters(), m_characterName);
    m_window->updateTheme(m_theme);

    return true;
}

bool Settings::save() const
{
    QFile file(m_settingsFilePath);
    // Try to open the settings file for writing
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open settings file for writing:" << m_settingsFilePath;
        return false;
    }

    // Save current settings values to JSON object
    QJsonObject obj;
    obj["saveDirPath"] = m_saveDirPath;
    obj["questsFilePath"] = m_questsFilePath;
    obj["qstFilesDirPath"] = m_qstFilesDirPath;
    obj["characterName"] = m_characterName;
    obj["theme"] = m_theme;

    QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

void Settings::setSaveDirPath(const QString &path)
{
    // Update save directory path and refresh character list in UI
    m_saveDirPath = path;
    m_window->updateSaveDirPath(path);
    m_window->updateCharacterComboBox(getAvailableCharacters(), m_characterName);
    save();
}

void Settings::setQuestsFilePath(const QString &path)
{
    // Update path for quests JSON file and refresh UI
    m_questsFilePath = path;
    m_window->updateQuestsFilePath(path);
    save();
}

void Settings::setQstFilesDirPath(const QString &path)
{
    // Update QST files directory path and save settings
    m_qstFilesDirPath = path;
    m_window->updateQstFilesDirPath(path);
    save();
}

void Settings::setCharacterName(const QString &name)
{
    // Update selected character name and save it
    m_characterName = name;
    save();
}

void Settings::setTheme(const QString &theme)
{
    // Validate theme before setting; update if valid, else log a warning
    if (Theme::availableThemeNames().contains(theme)) {
        m_theme = theme;
        m_window->updateTheme(theme);
        save();
    } else {
        qWarning() << "Attempted to set unknown theme:" << theme;
    }
}

QString Settings::getSaveDirPath() const
{
    return m_saveDirPath;
}

QString Settings::getQuestsFilePath() const
{
    return m_questsFilePath;
}

QString Settings::getQstFilesDirPath() const
{
    return m_qstFilesDirPath;
}

QString Settings::getTheme() const
{
    return m_theme;
}

QStringList Settings::getAvailableCharacters() const
{
    QStringList characterList;
    QDir saveDir(m_saveDirPath);

    // List available characters by checking subdirectories for expected structure
    if (saveDir.exists()) {
        const QStringList subDirs = saveDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const QString &dir : subDirs) {
            QDir characterDir(saveDir.filePath(dir + "/levels_world001.map"));

            // Append character to list if any difficulty folder exists
            if (characterDir.exists("Normal") || characterDir.exists("Elite") || characterDir.exists("Ultimate")) {
                characterList.append(dir);
            }
        }
    }

    return characterList;
}

void Settings::checkAndSetDefaultQuestsFilePath()
{
    // Set default path to quests.json in the resources directory if it exists
    QString defaultQuestsFilePath = QDir::currentPath() + "/resources/quests.json";
    QFile defaultFile(defaultQuestsFilePath);

    if (defaultFile.exists()) {
        setQuestsFilePath(defaultQuestsFilePath);
        qDebug() << "Default quests.json file found and set:" << defaultQuestsFilePath;
    } else {
        qWarning() << "The quests.json file is missing in the resources directory. "
                      "Please specify the file path or generate a new file.";

        // Clear the quests file path in the UI if default is not found
        m_questsFilePath = "";
        m_window->updateQuestsFilePath(m_questsFilePath);
    }
}

void Settings::browseSaveDir()
{
    // Open file dialog to select save directory, update path if chosen
    QString dir = QFileDialog::getExistingDirectory(m_window, "Select Save Directory", m_saveDirPath);

    if (!dir.isEmpty()) {
        setSaveDirPath(dir);
    }
}

void Settings::browseJsonFile()
{
    // Open file dialog to select quests JSON file, update path if chosen
    QString file = QFileDialog::getOpenFileName(m_window, "Select Quests JSON File", m_questsFilePath, "JSON Files (*.json);;All Files (*)");

    if (!file.isEmpty()) {
        setQuestsFilePath(file);
    }
}

void Settings::browseQstFilesDir()
{
    // Open file dialog to select QST files directory, update path if chosen
    QString dir = QFileDialog::getExistingDirectory(m_window, "Select QST Files Directory", m_qstFilesDirPath);

    if (!dir.isEmpty()) {
        setQstFilesDirPath(dir);
    }
}
