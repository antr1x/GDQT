#ifndef GDD_PARSER_H
#define GDD_PARSER_H

#include <QFile>
#include <QDataStream>
#include <QString>
#include <QVector>
#include <QException>

class QuestsFile;

/**
 * @brief Template class extending QVector to read elements from a QuestsFile.
 *
 * @tparam T The type of elements stored in the vector.
 *
 * The Vector class provides a read method to populate itself by reading from a QuestsFile.
 * It reads the number of elements and then reads each element individually.
 */
template <typename T>
class Vector : public QVector<T>
{
public:
    /**
     * @brief Reads the vector data from the provided QuestsFile.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Class representing a string read from a QuestsFile.
 *
 * Extends QString to include a read method for populating itself from a QuestsFile.
 */
class String : public QString
{
public:
    /**
     * @brief Reads the string data from the provided QuestsFile.
     *
     * This function reads the length of the string and then reads each character individually.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Class representing a unique identifier (UID) consisting of 16 bytes.
 *
 * Used to uniquely identify quests, tasks, and other entities within the quests file.
 */
class UID
{
public:
    /// An array of 16 bytes representing the UID.
    quint8 id[16];

    /**
     * @brief Reads the UID data from the provided QuestsFile.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Class representing a list of tokens in the quests file.
 *
 * Contains a vector of strings that represent tokens used within quests.
 */
class TokenList
{
public:
    /// A vector containing the tokens.
    Vector<String> tokens;

    /**
     * @brief Reads the token list from the provided QuestsFile.
     *
     * Parses the token list block in the file and populates the tokens vector.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Class representing a task within a quest.
 *
 * Contains identifiers, state information, progress status, and objectives.
 */
class Task
{
public:
    /// Unique identifier for the task.
    UID id2;
    /// Another identifier for the task (possibly a numeric ID).
    quint32 id1;
    /// State of the task (e.g., completed, failed).
    quint32 state;
    /// Indicates if the task is in progress (1) or not (0).
    quint32 inProgress;
    /// A vector of objectives associated with the task.
    Vector<quint32> objectives;

    /**
     * @brief Reads the task data from the provided QuestsFile.
     *
     * Parses the task block and populates the task's properties.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Class representing a quest.
 *
 * Contains identifiers and a list of tasks that are part of the quest.
 */
class Quest
{
public:
    /// Identifier for the quest.
    quint32 id1;
    /// Unique identifier for the quest.
    UID id2;
    /// A vector of tasks that belong to the quest.
    Vector<Task> tasks;

    /**
     * @brief Reads the quest data from the provided QuestsFile.
     *
     * Parses the quest block and populates the quest's properties.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Class representing a list of quests.
 *
 * Contains a vector of all quests read from the quests file.
 */
class QuestList
{
public:
    /// A vector containing all the quests.
    Vector<Quest> quests;

    /**
     * @brief Reads the quest list from the provided QuestsFile.
     *
     * Parses the quest list block and populates the quests vector.
     *
     * @param gdd Pointer to the QuestsFile from which to read data.
     */
    void read(QuestsFile* gdd);
};

/**
 * @brief Struct representing a block in the quests file.
 *
 * Contains the length of the block and the end position within the file.
 */
struct Block
{
    /// Length of the block in bytes.
    quint32 len;
    /// End position of the block in the file.
    qint64 end;
};

/**
 * @brief Class for reading and parsing the quests file.
 *
 * Handles decryption, reading of headers, tokens, quests, and provides methods to read primitive types.
 */
class QuestsFile
{
private:
    /// Pointer to the QFile being read.
    QFile* file;
    /// QDataStream associated with the file.
    QDataStream* stream;
    /// Current decryption key.
    quint32 key;
    /// Decryption key table used for updating the key.
    quint32 table[256];

public:
    /// Unique identifier for the quests file.
    UID id;
    /// List of tokens used in the quests.
    TokenList tokens;
    /// List of quests contained in the file.
    QuestList quests;

    /**
     * @brief Reads and parses the quests file from the specified filename.
     *
     * Opens the file, initializes decryption, and reads the file content into structured data.
     *
     * @param filename The path to the quests file to read.
     */
    void read(const QString& filename);

private:
    /**
     * @brief Reads the initial decryption key from the file and initializes the key table.
     *
     * This function is essential for decrypting subsequent data in the file.
     */
    void readKey();

    /**
     * @brief Reads a 4-byte integer from the file without updating the key.
     *
     * Used internally when the key should not be updated after reading.
     *
     * @return The decrypted 4-byte integer.
     */
    quint32 nextInt();

    /**
     * @brief Updates the decryption key based on the data that was just read.
     *
     * Ensures that the key changes with each read operation for security.
     *
     * @param ptr Pointer to the data that was read.
     * @param len Length of the data in bytes.
     */
    void updateKey(void* ptr, unsigned len);

public:
    /**
     * @brief Reads a 4-byte integer from the file and updates the key.
     *
     * Decrypts the integer using the current key and then updates the key.
     *
     * @return The decrypted 4-byte integer.
     */
    quint32 readInt();

    /**
     * @brief Reads a single byte from the file and updates the key.
     *
     * Decrypts the byte using the current key and then updates the key.
     *
     * @return The decrypted byte.
     */
    quint8 readByte();

    /**
     * @brief Begins reading a block from the file.
     *
     * Reads the block type and length, and calculates the end position.
     *
     * @param b Pointer to a Block structure to store block information.
     * @return The block type identifier.
     */
    quint32 readBlockStart(Block* b);

    /**
     * @brief Ends reading a block and verifies its integrity.
     *
     * Checks that the current file position matches the expected end and reads the block checksum.
     *
     * @param b Pointer to the Block structure containing block information.
     */
    void readBlockEnd(Block* b);
};

#endif // GDD_PARSER_H
