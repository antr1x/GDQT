#ifndef QUESTTRACKERWINDOW_H
#define QUESTTRACKERWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSortFilterProxyModel>
#include "types.h"

// Forward declaration
class Settings;

QT_BEGIN_NAMESPACE
namespace Ui {
class QuestTrackerWindow;
}
QT_END_NAMESPACE

/**
 * @class QuestTrackerWindow
 * @brief Main window class for managing and displaying quest tracking data in Grim Dawn.
 *
 * The QuestTrackerWindow class provides the main interface for users to view and interact
 * with quest data, including filtering, refreshing, and loading quests from external sources.
 * It also integrates logging functionality and applies theme settings to enhance user experience.
 */
class QuestTrackerWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the QuestTrackerWindow.
     *
     * Initializes the UI components, settings, and logging configuration.
     *
     * @param parent The parent widget of the window.
     */
    explicit QuestTrackerWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     *
     * Cleans up allocated resources, including settings and UI.
     */
    ~QuestTrackerWindow();

    /**
     * @brief Sets the quests text in the UI's text area.
     *
     * This method allows setting a custom text display, typically used for detailed
     * quest information or log updates.
     *
     * @param text The text to display in the quests text area.
     */
    void setQuestsText(const QString &text);

    /**
     * @brief Refreshes quest data based on the current character and difficulty settings.
     *
     * This method updates the displayed quest data by reading the current settings and files.
     */
    void refreshData();

    /**
     * @brief Generates a JSON file containing quest data.
     *
     * Reads .qst files from a specified directory and outputs a JSON file with structured quest data.
     */
    void generateQuestJsonFile();

    /**
     * @brief Populates the quest table view with quest data.
     *
     * Fills the UI's quest table with detailed information, including quest name,
     * chapter, and status for each difficulty level.
     *
     * @param questData The data model containing quest information to be displayed.
     */
    void populateTableView(const QuestData &questData);

    /**
     * @brief Filters the quest table based on the provided text.
     *
     * Applies a text filter to the quest table view to help users quickly locate specific quests.
     *
     * @param text The text to filter quests by.
     */
    void filterTable(const QString &text);

    // Update UI Methods

    /**
     * @brief Updates the save directory path displayed in the UI.
     *
     * @param path The new save directory path.
     */
    void updateSaveDirPath(const QString &path);

    /**
     * @brief Updates the quests JSON file path in the UI.
     *
     * @param path The path to the JSON file with quest data.
     */
    void updateQuestsFilePath(const QString &path);

    /**
     * @brief Updates the character selection combo box.
     *
     * Refreshes the character list in the UI and sets the selected character.
     *
     * @param characters A list of character names.
     * @param selectedCharacter The character to set as currently selected.
     */
    void updateCharacterComboBox(const QStringList &characters, const QString &selectedCharacter);

    /**
     * @brief Updates the QST files directory path in the UI.
     *
     * @param path The new path to the QST files directory.
     */
    void updateQstFilesDirPath(const QString &path);

    /**
     * @brief Updates the application theme based on user selection.
     *
     * Applies the chosen theme by adjusting colors and styles across the UI.
     *
     * @param theme The name of the selected theme.
     */
    void updateTheme(const QString &theme);

    // Static Methods

    /**
     * @brief Custom message handler for logging messages in the application.
     *
     * Formats log messages with appropriate colors based on message type
     * and appends them to the logging text area.
     *
     * @param type The type of message (debug, warning, critical, etc.).
     * @param context Provides information about the context of the message.
     * @param msg The message content.
     */
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    // Initialization Methods

    /**
     * @brief Initializes application settings, connecting UI elements to the settings manager.
     *
     * Loads saved settings, populates theme and directory options, and sets up user preferences.
     */
    void initializeSettings();

    /**
     * @brief Sets up logging functionality for the application.
     *
     * Configures the log display in the UI to capture and show log messages.
     */
    void initializeLogging();

    // Member Variables
    Ui::QuestTrackerWindow *ui;                ///< The UI form class generated by Qt Designer.
    QSortFilterProxyModel *proxyModel;         ///< Model for filtering quest table data.
    QStringList m_originalCharacterNames;      ///< List of original character names for selection.
    Settings *m_settings;                      ///< Pointer to the settings manager.

    // Static Members
    static QTextEdit *textEditLogInstance;     ///< Static instance of log text edit for displaying logs.
};

#endif // QUESTTRACKERWINDOW_H
