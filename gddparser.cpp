#include "gddparser.h"
#include <QDebug>

template <typename T>
void Vector<T>::read(QuestsFile* gdd)
{
    quint32 n = gdd->readInt();
    this->resize(n);
    for (quint32 i = 0; i < n; i++)
    {
        (*this)[i].read(gdd);
    }
}

void String::read(QuestsFile* gdd)
{
    quint32 len = gdd->readInt();
    this->clear();
    for (quint32 i = 0; i < len; i++)
    {
        this->append(QChar(gdd->readByte()));
    }
}

void UID::read(QuestsFile* gdd)
{
    for (unsigned i = 0; i < 16; i++)
    {
        id[i] = gdd->readByte();
    }
}

void TokenList::read(QuestsFile* gdd)
{
    Block b;

    if (gdd->readBlockStart(&b) != 10)
        throw QException();

    if (gdd->readInt() != 2)
        throw QException();

    tokens.read(gdd);

    gdd->readBlockEnd(&b);
}

void QuestList::read(QuestsFile* gdd)
{
    Block b;

    if (gdd->readBlockStart(&b) != 11)
        throw QException();

    auto version_file = gdd->readInt();

    qDebug() << QString("QuestList Version: 0x%1").arg(version_file, 0, 16);

    if (version_file != 4)
        throw QException();

    quests.read(gdd);

    gdd->readBlockEnd(&b);
}

void Quest::read(QuestsFile* gdd)
{
    Block b;

    id1 = gdd->readInt();
    id2.read(gdd);

    if (gdd->readBlockStart(&b))
        throw QException();

    tasks.read(gdd);

    gdd->readBlockEnd(&b);
}

void Task::read(QuestsFile* gdd)
{
    Block b;

    id1 = gdd->readInt();
    id2.read(gdd);

    if (gdd->readBlockStart(&b) != 0)
        throw QException();

    state = gdd->readInt();
    inProgress = gdd->readByte();
    quint8 padding = gdd->readByte();

    quint32 n = (b.len - 6) / 4;

    objectives.resize(n);

    for (quint32 i = 0; i < n; i++)
    {
        objectives[i] = gdd->readInt();
    }

    gdd->readBlockEnd(&b);
}

void QuestsFile::read(const QString& filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open file:" << filename;
        throw QException();
    }

    qDebug() << "Opened file:" << filename;

    file = &f;
    stream = new QDataStream(file);
    stream->setByteOrder(QDataStream::LittleEndian);

    qint64 end = file->size();
    qDebug() << "File size:" << end << "bytes";

    readKey();
    qDebug() << "Read key successfully.";

    auto version_header = readInt();
    qDebug() << QString("Header version: 0x%1").arg(version_header, 0, 16);

    if (version_header != 0x58545351) {
        qCritical() << "Invalid header version. Expected: 0x58545351, Found:" << QString("0x%1").arg(version_header, 0, 16);
        throw QException();
    }

    auto file_version = readInt();
    qDebug() << QString("File Version: 0x%1").arg(file_version, 0, 16);

    if (file_version != 0) {
        qCritical() << "Invalid file version. Expected: 0, Found:" << file_version;
        throw QException();
    }

    id.read(this);
    tokens.read(this);
    quests.read(this);

    if (file->pos() != end) {
        qCritical() << "File read did not reach expected end position. Current position:" << file->pos() << ", Expected:" << end;
        throw QException();
    }

    delete stream;
    stream = nullptr;
    file = nullptr;
}

void QuestsFile::readKey()
{
    quint32 k;
    if (stream->readRawData(reinterpret_cast<char*>(&k), sizeof(k)) != sizeof(k))
        throw QException();

    k ^= 0x55555555;
    key = k;

    for (unsigned i = 0; i < 256; i++)
    {
        k = (k >> 1) | (k << 31);
        k *= 39916801;
        table[i] = k;
    }
}

quint32 QuestsFile::nextInt()
{
    quint32 ret;
    if (stream->readRawData(reinterpret_cast<char*>(&ret), sizeof(ret)) != sizeof(ret))
        throw QException();

    ret ^= key;
    return ret;
}

void QuestsFile::updateKey(void* ptr, unsigned len)
{
    quint8* p = reinterpret_cast<quint8*>(ptr);

    for (unsigned i = 0; i < len; i++)
    {
        key ^= table[p[i]];
    }
}

quint32 QuestsFile::readInt()
{
    quint32 val;
    if (stream->readRawData(reinterpret_cast<char*>(&val), sizeof(val)) != sizeof(val))
        throw QException();

    quint32 ret = val ^ key;
    updateKey(&val, sizeof(val));

    return ret;
}

quint8 QuestsFile::readByte()
{
    quint8 val;
    if (stream->readRawData(reinterpret_cast<char*>(&val), sizeof(val)) != sizeof(val))
        throw QException();

    quint8 ret = val ^ key;
    updateKey(&val, sizeof(val));

    return ret;
}

quint32 QuestsFile::readBlockStart(Block* b)
{
    quint32 ret = readInt();
    b->len = nextInt();
    b->end = file->pos() + b->len;

    return ret;
}

void QuestsFile::readBlockEnd(Block* b)
{
    qint64 current_pos = file->pos();
    if (current_pos != b->end)
    {
        qDebug() << "Block end position mismatch. Current position:" << current_pos << ", Expected:" << b->end;
        throw QException();
    }

    quint32 checksum = nextInt();
    if (checksum != 0)
    {
        qDebug() << "Checksum mismatch at block end. Expected checksum to be 0, but got:" << checksum;
        throw QException();
    }
}
