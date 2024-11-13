#include "qst_parser.h"

QstFile::QstFile() : questHash(0) {}
QstFile::QstFile(const QString &filePath) : filePath(filePath), questHash(0) {}

bool QstFile::parse()
{
    // Read the entire file into memory
    if (!readFile()) {
        return false;
    }

    // Extract the quest hash from the file data
    if (!extractQuestHash()) {
        return false;
    }

    // Extract the chapter name and quest name from the file data
    if (!extractLocalizationData()) {
        return false;
    }

    return true;
}

QString QstFile::getChapterName() const
{
    return chapterName;
}

QString QstFile::getQuestName() const
{
    return questName;
}

uint32_t QstFile::getQuestHash() const
{
    return questHash;
}

bool QstFile::readFile()
{
    QFile file(filePath);

    // Attempt to open the file in read-only mode
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << filePath;
        return false;
    }

    // Read all data from the file into the 'data' member variable
    data = file.readAll();
    file.close();

    return true;
}

bool QstFile::extractQuestHash()
{
    // Ensure the file has enough data to contain a quest hash
    if (data.size() < 16) {
        qWarning() << "File too small to contain valid quest data:" << filePath;
        return false;
    }

    // Read 4 bytes starting from the 12th byte as the quest hash
    // This assumes the quest hash is located at offset 12
    questHash = *reinterpret_cast<const uint32_t*>(data.constData() + 12);

    return true;
}

bool QstFile::extractLocalizationData()
{
    // **Lazy Reading Approach**
    // Instead of parsing the file according to its correct structure,
    // we search for a specific marker ('enUS') and use offsets from there.
    // This is considered "lazy reading" because it doesn't fully parse the file format.
    // In the future, this should be replaced with proper parsing of the QST file structure.

    // Search for the 'enUS' localization marker in the file data
    int position = data.indexOf("enUS");

    if (position == -1) {
        qWarning() << "Localization marker 'enUS' not found in file:" << filePath;
        return false;
    }

    // Offset to move past the 'enUS' marker (which is 4 bytes long)
    int offset = position + 4;

    // Skip any non-letter characters (usually null bytes or delimiters) to find the start of the chapter name
    while (offset < data.size() && !QChar::isLetter(data[offset])) {
        offset++;
    }

    // Check if we've reached the end of the data without finding valid text
    if (offset >= data.size()) {
        qWarning() << "Offset out of bounds for chapter name in file:" << filePath;
        return false;
    }

    // Find the null terminator that marks the end of the chapter name string
    int endIndex = data.indexOf('\0', offset);

    // Validate the indices to ensure we have a valid string
    if (endIndex == -1 || endIndex <= offset) {
        qWarning() << "Invalid chapter name in file:" << filePath;
        return false;
    }

    // Regular expression to remove non-printable characters from the extracted string
    static const QRegularExpression nonPrintableRegex("[^\x20-\x7E]");

    // Extract the chapter name from the data and clean it up
    chapterName = QString::fromUtf8(data.mid(offset, endIndex - offset)).trimmed();
    chapterName.remove(nonPrintableRegex);

    // Move the offset past the end of the chapter name string
    offset = endIndex + 1;

    // Again, skip any non-letter characters to find the start of the quest name
    while (offset < data.size() && !QChar::isLetter(data[offset])) {
        offset++;
    }

    // Check if we've reached the end of the data without finding valid text
    if (offset >= data.size()) {
        qWarning() << "Offset out of bounds for quest name in file:" << filePath;
        return false;
    }

    // Find the null terminator that marks the end of the quest name string
    endIndex = data.indexOf('\0', offset);

    // Validate the indices to ensure we have a valid string
    if (endIndex == -1 || endIndex <= offset) {
        qWarning() << "Invalid quest name in file:" << filePath;
        return false;
    }

    // Extract the quest name from the data and clean it up
    questName = QString::fromUtf8(data.mid(offset, endIndex - offset)).trimmed();
    questName.remove(nonPrintableRegex);

    return true;
}
