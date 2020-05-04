#include "cryptography.h"

#include <crypto++/aes.h>
#include <crypto++/filters.h>
#include <crypto++/modes.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>
#include <crypto++/md5.h>

#include <QMessageBox>

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

const QString Cryptography::aesEncrypt(const QString &input, const QString &key)
{
    std::string plain = input.toStdString();
    std::string strKey = key.toStdString();
    std::string cipherText;
    // Hex decode symmetric key
    CryptoPP::HexDecoder decoder;

    while (strKey.length() < CryptoPP::AES::DEFAULT_KEYLENGTH)
        strKey.push_back('0');

    decoder.Put((byte*)strKey.c_str(), 32 * 2);
    decoder.MessageEnd();

    CryptoPP::word64 size = decoder.MaxRetrievable();
    char *decodedKey = new char[size];

    decoder.Get((byte*)decodedKey, size);
    // Generate Cipher, Key, and CBC
    byte bKey[CryptoPP::AES::MAX_KEYLENGTH];
    byte iv[CryptoPP::AES::BLOCKSIZE];

    CryptoPP::StringSource(reinterpret_cast<const char*>(decodedKey), true,
                  new CryptoPP::HashFilter(*(new CryptoPP::SHA256), new CryptoPP::ArraySink(bKey, CryptoPP::AES::MAX_KEYLENGTH)));
    std::memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption Encryptor(bKey, sizeof(bKey), iv);
    CryptoPP::StringSource(plain, true, new CryptoPP::StreamTransformationFilter(Encryptor,
                  new CryptoPP::HexEncoder(new CryptoPP::StringSink(cipherText))));

    return QString::fromStdString(cipherText);
}

const QString Cryptography::aesDecrypt(const QString &input, const QString &key)
{
    std::string plain;
    std::string encrypted = input.toStdString();
    std::string strKey = key.toStdString();
    // Hex decode symmetric key:
    CryptoPP::HexDecoder decoder;

    while (strKey.length() < CryptoPP::AES::MAX_KEYLENGTH)
        strKey.push_back('0');

    decoder.Put((byte*)strKey.c_str(), 32 * 2);
    decoder.MessageEnd();

    CryptoPP::word64 size = decoder.MaxRetrievable();
    char *decodedKey = new char[size];

    decoder.Get((byte*)decodedKey, size);
    // Generate Cipher, Key, and CBC
    byte bKey[CryptoPP::AES::MAX_KEYLENGTH];
    byte iv[CryptoPP::AES::BLOCKSIZE];

    CryptoPP::StringSource(reinterpret_cast<const char*>(decodedKey), true,
                  new CryptoPP::HashFilter(*(new CryptoPP::SHA256),
                                           new CryptoPP::ArraySink(bKey, CryptoPP::AES::MAX_KEYLENGTH)));
    memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);

    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption Decryptor(bKey, sizeof(bKey), iv);
        CryptoPP::StringSource(encrypted, true,
                      new CryptoPP::HexDecoder(new CryptoPP::StreamTransformationFilter(Decryptor,
                                     new CryptoPP::StringSink(plain))));
    }
    catch (std::exception &e)
    {
        QMessageBox mb;

        mb.setText(e.what());
        mb.exec();
    }

    return QString::fromStdString(plain);
}
