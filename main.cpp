#include "questtrackerwindow.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QuestTrackerWindow w;

    // The font path points to a Grim Dawn-themed font, inspired by www.grimtools.com where a similar font was used.
    QString fontPath = ":/fonts/LinBiolinum_R.ttf";

    // Custom font size for the application
    int fontSize = 12;

    // Attempt to load the custom font from resources
    int fontId = QFontDatabase::addApplicationFont(fontPath);

    if (fontId == -1) {
        qWarning() << "Failed to load font from resources:" << fontPath;
    } else {
        // Retrieve the font family name and set it as the application's default font
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont customFont(fontFamily, fontSize);
        a.setFont(customFont);
    }

    // Initialize data by refreshing saved quest data
    w.refreshData();

    // Display the main window
    w.show();

    return a.exec();
}
