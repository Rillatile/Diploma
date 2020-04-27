#include "cryptography.h"

Cryptography::Cryptography()
{

}

const QString Cryptography::xorEncrypt(const QString &input, const QString& key)
{
    int i, j;
    QString output = "";

    // Перевод строк в битовые массивы с использованием локальных таблиц
    QByteArray baInput = input.toLocal8Bit();
    QByteArray baKey = key.toLocal8Bit();

    // Кодирование информации
    for (i = 0; i < baInput.size(); i++)
    {
        for (j = 0; j < baKey.size(); j++)
        {
            // XOR - кодировка символа
            baInput[i] = baInput[i] ^ (baKey[j] + (i * j));
        }

        //Преобразование числа в шестнадцатеричную систему
        output += QString("%1").arg((int)((unsigned char)baInput[i]), 2, 16, QChar('0'));
    }

    //Возврат кодированной строки
    return output;
}

const QString Cryptography::xorDecrypt(const QString &input, const QString& key)
{
    int i, j;

    // Декодировка строки из 16-ричной системы в битовый массив
    QByteArray baInput = QByteArray::fromHex(input.toLocal8Bit());
    // Перевод строки пароля в битовый массив
    QByteArray baKey = key.toLocal8Bit();

    // Декодирование информации
    for (i = 0; i < baInput.size(); i++)
    {
        for (j = 0; j < baKey.size(); j++)
        {
            // XOR - кодировка символа
            baInput[i] = baInput[i] ^ (baKey[j] + (i * j));
        }
    }

    //Возврат кодированной строки
    return QString::fromLocal8Bit(baInput.data());
}
