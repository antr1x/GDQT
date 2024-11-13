#include "questtrackerwindow.h"
#include "./ui_questtrackerwindow.h"
#include "settings.h"
#include "gdd_parser.h"
#include "jsonparser.h"
#include "utils.h"
#include "version.h"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QMutex>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QApplication>

// Static member initialization
QTextEdit* QuestTrackerWindow::textEditLogInstance = nullptr;

QuestTrackerWindow::QuestTrackerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QuestTrackerWindow)
{
    ui->setupUi(this);
    ui->tabQestsTracker->setCurrentIndex(0);

    // Set window title with the application version
    QString title = QString("Grim Dawn Quests Tracker v%1.%2").arg(VERSION_MAJOR).arg(VERSION_MINOR);
    this->setWindowTitle(title);

    // Initialize settings and logging setup
    initializeSettings();
    initializeLogging();

    // Set up the filter proxy model to enable case-insensitive search across all columns
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1); // Apply filter to all columns

    // Assign the proxy model to the table view
    ui->tableViewQuestsList->setModel(proxyModel);
}

QuestTrackerWindow::~QuestTrackerWindow()
{
    delete m_settings;
    delete ui;
}

void QuestTrackerWindow::generateQuestJsonFile()
{
    QString directoryPath = ui->lineEditQstFilesPath->text();

    // Check if the directory path is specified
    if (directoryPath.isEmpty()) {
        qWarning() << "Please specify the directory with .qst files.";
        return;
    }

    // Attempt to generate the quests JSON file from the specified directory
    if (generateQuestJson(directoryPath)) {
        qInfo() << "The quests.json has been generated in the resources folder.";
    } else {
        qCritical() << "Failed to generate quests.json. Please check the directory and try again.";
    }
}

void QuestTrackerWindow::populateTableView(const QuestData &questData)
{
    // Create a new model with headers for quest details and difficulty statuses
    QStandardItemModel *tableModel = new QStandardItemModel(this);
    tableModel->setHorizontalHeaderLabels({"Chapter", "Quest", "Normal", "Elite", "Ultimate"});

    // Iterate over each chapter and its quests, creating table rows
    for (const QString &chapter : questData.chapters()) {
        for (const QString &quest : questData.quests(chapter)) {
            // Prepare row items with chapter and quest information
            QList<QStandardItem *> rowItems = {new QStandardItem(chapter), new QStandardItem(quest)};

            // Retrieve quest status for each difficulty level and set colors
            for (const Difficulty &difficulty : Difficulty::getAllDifficulties()) {
                QuestStatus questStatus = questData.getStatus(chapter, quest, difficulty.name);

                QStandardItem *item = new QStandardItem(questStatus.toString());
                item->setForeground(questStatus.color());  // Set text color based on status
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

                rowItems.append(item);
            }

            // Add the completed row to the model
            tableModel->appendRow(rowItems);
        }
    }

    // Assign the model to the proxy and enable sorting on the table view
    proxyModel->setSourceModel(tableModel);
    ui->tableViewQuestsList->setSortingEnabled(true);

    // Adjust column sizes; resize name columns to content, others to stretch
    for (int i = 0; i < tableModel->columnCount(); ++i) {
        ui->tableViewQuestsList->horizontalHeader()->setSectionResizeMode(i, i < 2 ? QHeaderView::ResizeToContents : QHeaderView::Stretch);
    }

    // Sort the table by the chapter column in ascending order
    ui->tableViewQuestsList->sortByColumn(0, Qt::AscendingOrder);
}

void QuestTrackerWindow::filterTable(const QString &text)
{
    // Apply case-insensitive filtering to the table view using a regular expression
    QRegularExpression regex(text, QRegularExpression::CaseInsensitiveOption);
    proxyModel->setFilterRegularExpression(regex);
}

void QuestTrackerWindow::refreshData()
{
    // Ensure a valid character is selected from the combo box
    int selectedIndex = ui->comboBoxCharacter->currentIndex();
    if (selectedIndex < 0 || selectedIndex >= m_originalCharacterNames.size()) {
        qDebug() << "No valid character selected.";
        return;
    }

    QString characterFolder = m_originalCharacterNames[selectedIndex];
    QString gddFilePath = m_settings->getSaveDirPath() + "/" + characterFolder + "/levels_world001.map/";

    QuestData questData;
    QuestsFile gddParser;
    JsonParser jsonParser;

    try {
        // Parse quests JSON file and load data
        jsonParser.read(m_settings->getQuestsFilePath());

        // Process each difficulty level and update quest data based on task states
        for (const Difficulty &difficulty : Difficulty::getAllDifficulties()) {
            QFile gddFile(QString("%1/%2/quests.gdd").arg(gddFilePath, difficulty.name));

            if (gddFile.exists()) {
                gddParser.read(gddFile.fileName());

                for (const Quest &quest : gddParser.quests.quests) {
                    QString questIdStr = QString("0x%1").arg(quest.id1, 8, 16, QChar('0')).toLower();
                    QuestInfo questInfo = jsonParser.questData.value(questIdStr);

                    // Skip processing if quest info is incomplete or matches a bounty quest
                    if (!questInfo.Chapter.isEmpty() && !questInfo.QuestName.isEmpty() && !questInfo.QuestName.contains("Bounty:")) {
                        QuestStatus::Status questStatus;

                        // Determine quest status based on task completion states
                        if (std::all_of(quest.tasks.cbegin(), quest.tasks.cend(), [](const Task &task) { return task.state == 3; })) {
                            questStatus = QuestStatus::Completed;
                        } else if (std::none_of(quest.tasks.cbegin(), quest.tasks.cend(), [](const Task &task) { return task.state == 3 || task.state == 2; })) {
                            questStatus = QuestStatus::NotCompleted;
                        } else {
                            questStatus = QuestStatus::InProgress;
                        }

                        // Update the quest data model with the quest status
                        questData.setStatus(questInfo.Chapter, questInfo.QuestName, difficulty.name, questStatus);
                    }
                }
            }
        }

        // Populate the table view with the updated quest data
        populateTableView(questData);
    } catch (QException &) {
        qDebug() << "An error occurred during parsing.";
    }
}

void QuestTrackerWindow::initializeSettings()
{
    // Load available themes and populate the theme combo box
    ui->comboBoxTheme->clear();
    ui->comboBoxTheme->addItems(Theme::availableThemeNames());

    // Create settings object and load stored configurations
    m_settings = new Settings(this);
    m_settings->load();

    // Connect UI buttons to corresponding functions for browsing directories
    connect(ui->buttonBrowseSaves, &QPushButton::clicked, m_settings, &Settings::browseSaveDir);
    connect(ui->buttonBrowseJson, &QPushButton::clicked, m_settings, &Settings::browseJsonFile);
    connect(ui->buttonBrowseQst, &QPushButton::clicked, m_settings, &Settings::browseQstFilesDir);

    // Connect data refresh and JSON generation buttons to their respective functions
    connect(ui->buttonRefreshData, &QPushButton::clicked, this, &QuestTrackerWindow::refreshData);
    connect(ui->buttonGenerateJson, &QPushButton::clicked, this, &QuestTrackerWindow::generateQuestJsonFile);

    // Connect character selection and filter input to corresponding slots
    connect(ui->comboBoxCharacter, &QComboBox::currentIndexChanged, this, &QuestTrackerWindow::refreshData);
    connect(ui->lineEditQuestsFilter, &QLineEdit::textChanged, this, &QuestTrackerWindow::filterTable);

    // Connect theme selection to settings for applying the chosen theme
    connect(ui->comboBoxTheme, &QComboBox::currentTextChanged, m_settings, &Settings::setTheme);
}

void QuestTrackerWindow::initializeLogging()
{
    // Set the log instance to display messages in the textEditLog widget
    textEditLogInstance = ui->textEditLog;
    qInstallMessageHandler(customMessageHandler);
}

void QuestTrackerWindow::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    // Determine the theme and appropriate color for log message based on message type
    Theme currentTheme = Theme::getThemeByName(qApp->palette().color(QPalette::Window).lightness() > 127 ? "Light Theme" : "Dark Theme");

    QString color;
    QString message = msg;

    // Remove any enclosing double quotes from the message
    if (message.startsWith("\"") && message.endsWith("\"")) {
        message = message.mid(1, message.length() - 2);
    }

    // Select color based on message type
    switch (type) {
    case QtDebugMsg:
        color = currentTheme.debugColor;
        break;
    case QtWarningMsg:
        color = currentTheme.warningColor;
        break;
    case QtCriticalMsg:
        color = currentTheme.criticalColor;
        break;
    case QtFatalMsg:
        color = currentTheme.fatalColor;
        abort();
    case QtInfoMsg:
        color = currentTheme.infoColor;
        break;
    }

    // Format the message with HTML color styling for display in the log
    QString formattedMessage = QString("<span style=\"color:%1;\">%2</span>").arg(color, message.toHtmlEscaped());

    if (textEditLogInstance) {
        textEditLogInstance->append(formattedMessage);
        textEditLogInstance->ensureCursorVisible();
    }
}

void QuestTrackerWindow::updateSaveDirPath(const QString &path)
{
    // Update the save directory path in the UI
    ui->lineEditSavesPath->setText(path);
}

void QuestTrackerWindow::updateQuestsFilePath(const QString &path)
{
    // Update the quests file path in the UI
    ui->lineEditJsonFilePath->setText(path);
}

void QuestTrackerWindow::updateCharacterComboBox(const QStringList &characters, const QString &selectedCharacter)
{
    // Refresh the character combo box with new character data
    ui->comboBoxCharacter->clear();
    m_originalCharacterNames.clear();

    for (const QString &character : characters) {
        QString displayName = character;
        if (displayName.startsWith("_")) displayName.remove(0, 1);

        ui->comboBoxCharacter->addItem(displayName);
        m_originalCharacterNames.append(character);
    }

    // Set the combo box index to the selected character if available
    int index = m_originalCharacterNames.indexOf(selectedCharacter);
    if (index != -1) {
        ui->comboBoxCharacter->setCurrentIndex(index);
    } else if (!m_originalCharacterNames.isEmpty()) {
        ui->comboBoxCharacter->setCurrentIndex(0);
        m_settings->setCharacterName(m_originalCharacterNames[0]);
    }
}

void QuestTrackerWindow::updateQstFilesDirPath(const QString &path)
{
    // Update the QST files directory path in the UI
    ui->lineEditQstFilesPath->setText(path);
}

void QuestTrackerWindow::updateTheme(const QString &themeName)
{
    // Temporarily block signals to avoid triggering extra events during theme change
    bool oldState = ui->comboBoxTheme->blockSignals(true);
    int index = ui->comboBoxTheme->findText(themeName);
    if (index != -1)
    {
        ui->comboBoxTheme->setCurrentIndex(index);
    }
    else
    {
        // Default to the first theme if the specified theme is not found
        ui->comboBoxTheme->setCurrentIndex(0);
    }
    ui->comboBoxTheme->blockSignals(oldState);

    // Apply the new theme settings to the application
    Theme theme = Theme::getThemeByName(themeName);
    Theme::applyTheme(theme);
}
