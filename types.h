#ifndef TYPES_H
#define TYPES_H

#include <QMap>
#include <QColor>
#include <QString>
#include <QPalette>
#include <QApplication>
#include <QStyleFactory>

// Difficluty

enum class DifficultyLevel {
    Normal,
    Elite,
    Ultimate
};

struct Difficulty {
    DifficultyLevel level;
    QString name;

    Difficulty(DifficultyLevel lvl, const QString &nm) : level(lvl), name(nm) {}

    static QList<Difficulty> getAllDifficulties() {
        return {
            {DifficultyLevel::Normal, "Normal"},
            {DifficultyLevel::Elite, "Elite"},
            {DifficultyLevel::Ultimate, "Ultimate"}
        };
    }
};

// Quests

struct QuestInfo
{
    QString Chapter;
    QString QuestName;
};

struct QuestStatus {
    enum Status {
        NotCompleted,
        InProgress,
        Completed
    };

    Status status;

    QuestStatus(Status s = NotCompleted) : status(s) {}

    QString toString() const {
        switch (status) {
        case NotCompleted: return "Not completed";
        case InProgress: return "In Progress";
        case Completed: return "Completed";
        default: return "Unknown";
        }
    }

    QColor color() const {
        switch (status) {
        case NotCompleted: return QColor(244, 67, 54);
        case InProgress: return QColor(0, 176, 255);
        case Completed: return QColor(76, 175, 80);
        default: return QColor(0, 0, 0);
        }
    }
};

class QuestData {
public:
    void setStatus(const QString &chapter, const QString &quest, const QString &difficulty, QuestStatus::Status status) {
        data[chapter][quest][difficulty] = QuestStatus(status);
    }

    QuestStatus getStatus(const QString &chapter, const QString &quest, const QString &difficulty) const {
        return data.value(chapter).value(quest).value(difficulty, QuestStatus(QuestStatus::NotCompleted));
    }

    // Итераторы для удобного доступа к данным (если нужно)
    auto chapters() const { return data.keys(); }
    auto quests(const QString &chapter) const { return data.value(chapter).keys(); }
    auto difficulties(const QString &chapter, const QString &quest) const { return data.value(chapter).value(quest).keys(); }
    auto allData() const { return data; }

private:
    QMap<QString, QMap<QString, QMap<QString, QuestStatus>>> data;
};

// Themes

struct Theme {
    QString name;
    QPalette palette;
    QString debugColor;
    QString warningColor;
    QString criticalColor;
    QString fatalColor;
    QString infoColor;

    static Theme lightTheme() {
        Theme theme;
        theme.name = "Light Theme";

        // Define the light palette
        theme.palette.setColor(QPalette::Window, QColor(240, 240, 240));
        theme.palette.setColor(QPalette::WindowText, QColor(30, 30, 30)); // Slightly softer black
        theme.palette.setColor(QPalette::Base, QColor(255, 255, 255));
        theme.palette.setColor(QPalette::AlternateBase, QColor(225, 225, 225));
        theme.palette.setColor(QPalette::ToolTipBase, QColor(30, 30, 30)); // Slightly softer black
        theme.palette.setColor(QPalette::ToolTipText, QColor(240, 240, 240)); // Slightly off-white
        theme.palette.setColor(QPalette::Text, QColor(30, 30, 30)); // Slightly softer black
        theme.palette.setColor(QPalette::Button, QColor(240, 240, 240));
        theme.palette.setColor(QPalette::ButtonText, QColor(30, 30, 30)); // Slightly softer black
        theme.palette.setColor(QPalette::BrightText, Qt::red);

        theme.palette.setColor(QPalette::Highlight, QColor(76, 163, 224));
        theme.palette.setColor(QPalette::HighlightedText, QColor(240, 240, 240)); // Slightly off-white

        // Customize disabled state colors
        theme.palette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
        theme.palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
        theme.palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));

        // Define log message colors
        theme.debugColor = "#808080"; // Gray for debug messages
        theme.warningColor = "#FFA500"; // Orange for warnings
        theme.criticalColor = "#FF0000"; // Red for critical messages
        theme.fatalColor = "#8B0000"; // Dark red for fatal errors
        theme.infoColor = "#1E1E1E"; // Dark gray instead of pure black for info messages

        return theme;
    }

    static Theme darkTheme() {
        Theme theme;
        theme.name = "Dark Theme";

        // Define the dark palette
        theme.palette.setColor(QPalette::Window, QColor(37, 37, 38));
        theme.palette.setColor(QPalette::WindowText, QColor(220, 220, 220)); // Slightly off-white
        theme.palette.setColor(QPalette::Base, QColor(30, 30, 30));
        theme.palette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
        theme.palette.setColor(QPalette::ToolTipBase, QColor(220, 220, 220)); // Slightly off-white
        theme.palette.setColor(QPalette::ToolTipText, QColor(220, 220, 220)); // Slightly off-white
        theme.palette.setColor(QPalette::Text, QColor(220, 220, 220)); // Slightly off-white
        theme.palette.setColor(QPalette::Button, QColor(45, 45, 48));
        theme.palette.setColor(QPalette::ButtonText, QColor(220, 220, 220)); // Slightly off-white
        theme.palette.setColor(QPalette::BrightText, Qt::red);

        theme.palette.setColor(QPalette::Highlight, QColor(244, 67, 54));
        theme.palette.setColor(QPalette::HighlightedText, QColor(30, 30, 30)); // Darker gray instead of pure black

        // Customize disabled state colors
        theme.palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        theme.palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        theme.palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));

        // Define log message colors
        theme.debugColor = "#808080"; // Light gray for debug messages
        theme.warningColor = "#FFD700"; // Gold for warnings
        theme.criticalColor = "#FF6347"; // Tomato for critical messages
        theme.fatalColor = "#FF4500"; // Orange red for fatal errors
        theme.infoColor = "#DCDCDC"; // Light gray instead of pure white for info messages

        return theme;
    }

    static void applyTheme(const Theme& theme) {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setPalette(theme.palette);
    }

    static QList<Theme> availableThemes() {
        return {lightTheme(), darkTheme()};
    }

    static QStringList availableThemeNames() {
        QStringList names;
        for (const Theme& theme : availableThemes()) {
            names.append(theme.name);
        }
        return names;
    }

    static Theme getThemeByName(const QString& name) {
        for (const Theme& theme : availableThemes()) {
            if (theme.name == name) {
                return theme;
            }
        }
        qWarning() << "Unknown theme name:" << name << ", defaulting to Light Theme";
        return lightTheme();
    }
};


#endif // TYPES_H
