#ifndef QST_PARSER_H
#define QST_PARSER_H

#include <QFile>
#include <QByteArray>
#include <QString>
#include <QRegularExpression>
#include <QDebug>

/**
 * @class QstFile
 * @brief A class for parsing QST files from Grim Dawn to extract quest data.
 *
 * The QstFile class provides functionality to read and parse QST files,
 * extracting information such as the quest hash, chapter name, and quest name.
 * It uses a "lazy reading" approach by searching for specific markers within
 * the binary data to locate the necessary information. Instead of parsing the
 * entire file structure, it looks for the 'enUS' marker and offsets from there
 * to find the required data. This method should be improved in the future by
 * implementing a full parser for the QST file structure, similar to how GDD files
 * are handled.
 */
class QstFile
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the QstFile class with default values.
     */
    QstFile();

    /**
     * @brief Constructs a QstFile with a specified file path.
     *
     * @param filePath The path to the QST file to be parsed.
     */
    QstFile(const QString &filePath);

    /**
     * @brief Parses the QST file to extract quest data.
     *
     * This function orchestrates the parsing process, which includes reading the file,
     * extracting the quest hash, and obtaining localization data such as the chapter
     * and quest names.
     *
     * @return True if parsing was successful; otherwise false.
     */
    bool parse();

    /**
     * @brief Gets the name of the chapter.
     *
     * @return The chapter name extracted from the QST file.
     */
    QString getChapterName() const;

    /**
     * @brief Gets the name of the quest.
     *
     * @return The quest name extracted from the QST file.
     */
    QString getQuestName() const;

    /**
     * @brief Gets the quest hash.
     *
     * The quest hash serves as a unique identifier for the quest.
     *
     * @return The quest hash as a 32-bit unsigned integer.
     */
    uint32_t getQuestHash() const;

private:
    /// Path to the QST file.
    QString filePath;

    /// Extracted chapter name from the QST file.
    QString chapterName;

    /// Extracted quest name from the QST file.
    QString questName;

    /// Quest hash used as a unique identifier.
    uint32_t questHash;

    /// Raw binary data read from the QST file.
    QByteArray data;

    /**
     * @brief Reads the QST file into memory.
     *
     * This function opens the file and reads all its contents into the 'data' member variable.
     *
     * @return True if the file was read successfully; otherwise false.
     */
    bool readFile();

    /**
     * @brief Extracts the quest hash from the file data.
     *
     * The quest hash is typically located at a specific offset within the file.
     * This function reads the 4 bytes at that offset and interprets them as a 32-bit unsigned integer.
     *
     * @return True if the quest hash was extracted successfully; otherwise false.
     */
    bool extractQuestHash();

    /**
     * @brief Extracts localization data (chapter and quest names) from the file data.
     *
     * **Lazy Reading Approach:**
     * Instead of parsing the file according to its correct structure, we search for a specific
     * marker ('enUS') and use offsets from there. This is considered "lazy reading" because
     * it doesn't fully parse the file format. In the future, this should be replaced with
     * proper parsing of the QST file structure as is done with GDD files.
     *
     * We use offsets and byte skips to navigate through the raw binary data. After finding
     * the 'enUS' marker, we skip non-letter characters to locate the chapter and quest names.
     * These skips are necessary because there may be extra bytes between meaningful data.
     *
     * @return True if the localization data was extracted successfully; otherwise false.
     */
    bool extractLocalizationData();
};

#endif // QST_PARSER_H
