#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include <QString>

class Cryptography
{
public:
    static const QString xorEncrypt(const QString& input, const QString& key);
    static const QString xorDecrypt(const QString& input, const QString& key);

private:
    Cryptography();
};

#endif // CRYPTOGRAPHY_H
