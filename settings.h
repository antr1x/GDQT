#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>
#include <QVector>

class QuestTrackerWindow;

/**
 * @class Settings
 * @brief Manages application settings, including file paths, theme, and character selection.
 *
 * The Settings class provides methods to load and save application settings from/to a JSON file.
 * It also includes utility functions for browsing directories and updating file paths, which are
 * reflected in the main application window. Additionally, it can retrieve a list of available
 * characters based on the save directory.
 */
class Settings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a Settings object and initializes it with a settings file path.
     *
     * Loads the settings from the specified JSON file or uses default values if the file does not exist.
     *
     * @param window Pointer to the main QuestTrackerWindow to update UI components.
     * @param settingsFilePath Path to the JSON file where settings are stored (default is "Settings.json").
     */
    explicit Settings(QuestTrackerWindow *window, const QString &settingsFilePath = "Settings.json");

    /**
     * @brief Loads settings from the JSON file.
     *
     * If the file is missing or has an invalid format, default values are used, and UI components
     * are updated accordingly.
     *
     * @return True if settings were loaded successfully, otherwise false.
     */
    bool load();

    /**
     * @brief Saves current settings to the JSON file.
     *
     * @return True if settings were saved successfully, otherwise false.
     */
    bool save() const;

    // Setters for updating settings and reflecting changes in the UI
    void setSaveDirPath(const QString &path);
    void setQuestsFilePath(const QString &path);
    void setQstFilesDirPath(const QString &path);
    void setCharacterName(const QString &name);
    void setTheme(const QString &theme);

    // Getters for retrieving current settings
    QString getSaveDirPath() const;
    QString getQuestsFilePath() const;
    QString getQstFilesDirPath() const;
    QString getTheme() const;

    /**
     * @brief Retrieves a list of available characters from the save directory.
     *
     * Searches for character folders containing data for different difficulty levels.
     *
     * @return A list of character names found in the save directory.
     */
    QStringList getAvailableCharacters() const;

    /**
     * @brief Sets the default path for the quests file if none is specified.
     *
     * Checks if a default `quests.json` file exists in the resources folder and sets its path.
     */
    void checkAndSetDefaultQuestsFilePath();

public slots:
    /**
     * @brief Opens a dialog to select the save directory.
     *
     * Updates the save directory path if a new directory is selected by the user.
     */
    void browseSaveDir();

    /**
     * @brief Opens a dialog to select the quests JSON file.
     *
     * Updates the quests file path if a new file is selected by the user.
     */
    void browseJsonFile();

    /**
     * @brief Opens a dialog to select the directory containing QST files.
     *
     * Updates the QST files directory path if a new directory is selected by the user.
     */
    void browseQstFilesDir();

private:
    QString m_settingsFilePath;      ///< Path to the settings JSON file.
    QString m_saveDirPath;           ///< Directory path where game saves are stored.
    QString m_questsFilePath;        ///< Path to the quests JSON file.
    QString m_qstFilesDirPath;       ///< Directory path where QST files are stored.
    QString m_characterName;         ///< Selected character name for quest tracking.
    QString m_theme;                 ///< Currently selected theme name.
    QuestTrackerWindow *m_window;    ///< Pointer to the main application window for UI updates.
};

#endif // SETTINGS_H
