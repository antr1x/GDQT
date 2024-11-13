#include "gdd_parser.h"
#include <QDebug>

template <typename T>
void Vector<T>::read(QuestsFile* gdd)
{
    // Read the number of elements in the vector from the file
    quint32 n = gdd->readInt();

    // Resize the vector to hold 'n' elements
    this->resize(n);

    // Iterate over each element and read its data from the file
    for (quint32 i = 0; i < n; i++)
    {
        // Recursively call the read function of each element
        (*this)[i].read(gdd);
    }
}

void String::read(QuestsFile* gdd)
{
    // Read the length of the string from the file
    quint32 len = gdd->readInt();

    // Clear any existing content in the string
    this->clear();

    // Read 'len' bytes from the file and append them to the string
    for (quint32 i = 0; i < len; i++)
    {
        // Read a byte and convert it to a QChar before appending
        this->append(QChar(gdd->readByte()));
    }
}

void UID::read(QuestsFile* gdd)
{
    // A UID consists of 16 bytes; read each byte from the file
    for (unsigned i = 0; i < 16; i++)
    {
        id[i] = gdd->readByte();
    }
}

void TokenList::read(QuestsFile* gdd)
{
    Block b;

    // Begin reading a block and check if the block type is 10 (TokenList)
    if (gdd->readBlockStart(&b) != 10)
        throw QException(); // Throw an exception if the block type doesn't match

    // Read the version number of the TokenList and verify it
    if (gdd->readInt() != 2)
        throw QException(); // Expected version is 2

    // Read the tokens vector from the file
    tokens.read(gdd);

    // Ensure we've reached the end of the block without extra data
    gdd->readBlockEnd(&b);
}

void QuestList::read(QuestsFile* gdd)
{
    Block b;

    // Begin reading a block and check if the block type is 11 (QuestList)
    if (gdd->readBlockStart(&b) != 11)
        throw QException(); // Throw an exception if the block type doesn't match

    // Read the version number of the QuestList
    auto version_file = gdd->readInt();

    qDebug() << QString("QuestList Version: 0x%1").arg(version_file, 0, 16);

    // Verify that the version number matches the expected value (4)
    if (version_file != 4)
        throw QException(); // Throw an exception if the version doesn't match

    // Read the list of quests from the file
    quests.read(gdd);

    // Ensure we've reached the end of the block without extra data
    gdd->readBlockEnd(&b);
}

void Quest::read(QuestsFile* gdd)
{
    Block b;

    // Read the first identifier (id1) for the quest
    id1 = gdd->readInt();

    // Read the second identifier (id2) which is a UID
    id2.read(gdd);

    // Begin reading a block; the block type is not checked here (could be variable)
    if (gdd->readBlockStart(&b))
        throw QException(); // Throw an exception if block start fails

    // Read the list of tasks associated with the quest
    tasks.read(gdd);

    // Ensure we've reached the end of the block without extra data
    gdd->readBlockEnd(&b);
}

void Task::read(QuestsFile* gdd)
{
    Block b;

    // Read the first identifier (id1) for the task
    id1 = gdd->readInt();

    // Read the second identifier (id2) which is a UID
    id2.read(gdd);

    // Begin reading a block and check if the block type is 0 (Task data)
    if (gdd->readBlockStart(&b) != 0)
        throw QException(); // Throw an exception if the block type doesn't match

    // Read the state of the task (e.g., completed, failed)
    state = gdd->readInt();

    // Read a byte indicating if the task is in progress (1) or not (0)
    inProgress = gdd->readByte();

    // Read a padding byte (unused but must be read to maintain correct position)
    quint8 padding = gdd->readByte();

    // Calculate the number of objectives based on the remaining block length
    // Each objective is a 4-byte integer, and we've already read 6 bytes (state, inProgress, padding)
    quint32 n = (b.len - 6) / 4;

    // Resize the objectives vector to hold 'n' objectives
    objectives.resize(n);

    // Read each objective's data from the file
    for (quint32 i = 0; i < n; i++)
    {
        objectives[i] = gdd->readInt();
    }

    // Ensure we've reached the end of the block without extra data
    gdd->readBlockEnd(&b);
}

void QuestsFile::read(const QString& filename)
{
    QFile f(filename);
    // Attempt to open the file in read-only mode
    if (!f.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open file:" << filename;
        throw QException();
    }

    qDebug() << "Opened file:" << filename;

    // Initialize the file and data stream
    file = &f;
    stream = new QDataStream(file);

    // Set the byte order to Little Endian as per file format specification
    stream->setByteOrder(QDataStream::LittleEndian);

    // Get the size of the file to determine the end position
    qint64 end = file->size();
    qDebug() << "File size:" << end << "bytes";

    // Read the encryption key used for subsequent data decryption
    readKey();
    qDebug() << "Read key successfully.";

    // Read and verify the header version identifier
    auto version_header = readInt();
    qDebug() << QString("Header version: 0x%1").arg(version_header, 0, 16);

    // The header version should match the expected magic number 'QSTX' (0x58545351)
    if (version_header != 0x58545351) {
        qCritical() << "Invalid header version. Expected: 0x58545351, Found:" << QString("0x%1").arg(version_header, 0, 16);
        throw QException();
    }

    // Read and verify the file version number
    auto file_version = readInt();
    qDebug() << QString("File Version: 0x%1").arg(file_version, 0, 16);

    // The expected file version is 0; throw an exception if it doesn't match
    if (file_version != 0) {
        qCritical() << "Invalid file version. Expected: 0, Found:" << file_version;
        throw QException();
    }

    // Read the unique identifier for the file
    id.read(this);

    // Read the list of tokens used in the quests
    tokens.read(this);

    // Read the list of quests from the file
    quests.read(this);

    // Verify that we've reached the end of the file
    if (file->pos() != end) {
        qCritical() << "File read did not reach expected end position. Current position:" << file->pos() << ", Expected:" << end;
        throw QException();
    }

    // Clean up by deleting the stream and resetting pointers
    delete stream;
    stream = nullptr;
    file = nullptr;
}

void QuestsFile::readKey()
{
    quint32 k;

    // Read the initial key value from the file (4 bytes)
    if (stream->readRawData(reinterpret_cast<char*>(&k), sizeof(k)) != sizeof(k))
        throw QException(); // Throw an exception if reading fails

    // XOR the key with a constant value to get the actual key
    k ^= 0x55555555;
    key = k;

    // Initialize the key table used for decrypting subsequent data
    for (unsigned i = 0; i < 256; i++)
    {
        // Rotate the key right by 1 bit and multiply by a constant
        k = (k >> 1) | (k << 31);
        k *= 39916801;
        // Store the transformed key in the table
        table[i] = k;
    }
}

quint32 QuestsFile::nextInt()
{
    quint32 ret;

    // Read the next 4 bytes from the file
    if (stream->readRawData(reinterpret_cast<char*>(&ret), sizeof(ret)) != sizeof(ret))
        throw QException(); // Throw an exception if reading fails

    // Decrypt the value using the current key
    ret ^= key;

    return ret;
}

void QuestsFile::updateKey(void* ptr, unsigned len)
{
    quint8* p = reinterpret_cast<quint8*>(ptr);

    // Update the decryption key based on the data that was just read
    // This ensures that the key changes with each read operation
    for (unsigned i = 0; i < len; i++)
    {
        key ^= table[p[i]];
    }
}

quint32 QuestsFile::readInt()
{
    quint32 val;

    // Read the next 4 bytes from the file
    if (stream->readRawData(reinterpret_cast<char*>(&val), sizeof(val)) != sizeof(val))
        throw QException(); // Throw an exception if reading fails

    // Decrypt the value using the current key
    quint32 ret = val ^ key;

    // Update the key with the raw value before decryption
    updateKey(&val, sizeof(val));

    return ret;
}

quint8 QuestsFile::readByte()
{
    quint8 val;

    // Read the next byte from the file
    if (stream->readRawData(reinterpret_cast<char*>(&val), sizeof(val)) != sizeof(val))
        throw QException(); // Throw an exception if reading fails

    // Decrypt the value using the current key
    quint8 ret = val ^ key;

    // Update the key with the raw value before decryption
    updateKey(&val, sizeof(val));

    return ret;
}

quint32 QuestsFile::readBlockStart(Block* b)
{
    // Read the block type identifier
    quint32 ret = readInt();

    // Read the length of the block
    b->len = nextInt();

    // Calculate the position where the block ends in the file
    b->end = file->pos() + b->len;

    return ret;
}

void QuestsFile::readBlockEnd(Block* b)
{
    // Get the current position in the file
    qint64 current_pos = file->pos();

    // Verify that we've reached the expected end position of the block
    if (current_pos != b->end)
    {
        qDebug() << "Block end position mismatch. Current position:" << current_pos << ", Expected:" << b->end;
        throw QException();
    }

    // Read the block checksum (should be 0)
    quint32 checksum = nextInt();

    if (checksum != 0)
    {
        qDebug() << "Checksum mismatch at block end. Expected checksum to be 0, but got:" << checksum;
        throw QException();
    }
}
