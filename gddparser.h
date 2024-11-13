#ifndef GDDPARSER_H
#define GDDPARSER_H

#include <QFile>
#include <QDataStream>
#include <QString>
#include <QVector>
#include <QException>

class QuestsFile;

template <typename T>
class Vector : public QVector<T>
{
public:
    void read(QuestsFile*);
};

class String : public QString
{
public:
    void read(QuestsFile*);
};

class UID
{
public:
    quint8 id[16];

    void read(QuestsFile*);
};

class TokenList
{
public:
    Vector<String> tokens;

    void read(QuestsFile*);
};

class Task
{
public:
    UID id2;
    quint32 id1;
    quint32 state;
    quint32 inProgress;
    Vector<quint32> objectives;

    void read(QuestsFile*);
};

class Quest
{
public:
    quint32 id1;
    UID id2;
    Vector<Task> tasks;

    void read(QuestsFile*);
};

class QuestList
{
public:
    Vector<Quest> quests;

    void read(QuestsFile*);
};

struct Block
{
    quint32 len;
    qint64 end;
};

class QuestsFile
{
private:
    QFile* file;
    QDataStream* stream;
    quint32 key;
    quint32 table[256];

public:
    UID id;
    TokenList tokens;
    QuestList quests;

    void read(const QString&);

private:
    void readKey();
    quint32 nextInt();
    void updateKey(void* ptr, unsigned len);

public:
    quint32 readInt();
    quint8 readByte();
    quint32 readBlockStart(Block* b);
    void readBlockEnd(Block* b);
};

#endif // GDDPARSER_H
