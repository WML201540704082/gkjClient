#include "mysm4.h"
#include "utf8.h"

#include <QDebug>
#include <QFile>

mySm4::mySm4(QObject *parent) : QObject(parent)
{
    iniKey();
}

void mySm4::iniKey()
{
    sm4_set_encrypt_key(&encrypt_key, user_key);
    sm4_set_decrypt_key(&decrypt_key, user_key);
}

bool mySm4::encrypt(unsigned char *data, unsigned long dataLen, QString savePath)
{
    // 为加密后的数据分配内存
    uint8_t *encrypted_data = (uint8_t *)malloc(dataLen + SM4_BLOCK_SIZE);  // 为加密数据预留空间
    if (encrypted_data == nullptr) 
    {
        qDebug() << "Memory allocation failed for encrypted data!";
        return false;
    }
    
    size_t encrypted_len = 0;
    // 对图像数据进行加密
    int ret = sm4_cbc_padding_encrypt(&encrypt_key, iv, data, dataLen, encrypted_data, &encrypted_len);
    if(ret != 1)
    {
        qDebug() << "Failed to encrypt image.";
        return false;
    }
    
    if (writeEncryptedDataToFile(encrypted_data, encrypted_len, savePath))
    {
        qDebug() << "Encrypted image saved to " << savePath;
    }
    else
    {
        qDebug() << "Failed to save encrypted image.";
        return false;
    }
    
    // 释放内存
    free(encrypted_data);
    
    return true;
}

bool mySm4::decrypt(const QString &filename, uint8_t *&encryptedData, size_t &encryptedLen, uint8_t *&decryptedData, size_t &decryptedLen)
{
    if(!readEncryptedDataFromFile(filename, encryptedData, encryptedLen))
    {
        qDebug() << "Failed to read encrypted data from file.";
        return false;
    }
    // 为解密后的数据分配内存
    decryptedData = (uint8_t *)malloc(encryptedLen);  // 解密后的数据大小不会超过加密数据的大小
    if (decryptedData == nullptr)
    {
        qDebug() << "Memory allocation failed for decrypted data!";
        free(encryptedData);
        return false;
    }
    
    int ret = sm4_cbc_padding_decrypt(&decrypt_key, iv, encryptedData, encryptedLen, decryptedData, &decryptedLen);
    if(ret != 1)
    {
        qDebug() << "Failed to decrypt data.";
        free(encryptedData);
        free(decryptedData);
        return false;
    }
    

}

bool mySm4::writeEncryptedDataToFile(const uint8_t *data, size_t len, const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open file for writing!";
        return false;
    }
    file.write(reinterpret_cast<const char*>(data), len);
    file.close();
    return true;
}

bool mySm4::readEncryptedDataFromFile(const QString &filename, uint8_t *&data, size_t &len)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file for reading!";
        return false;
    }

    len = file.size();
    data = (uint8_t *)malloc(len);
    if (data == nullptr) {
        qDebug() << "Memory allocation failed!";
        return false;
    }

    file.read(reinterpret_cast<char*>(data), len);
    file.close();
    return true;
}

bool mySm4::encryptStr(unsigned char *data, unsigned long dataLen, QString &strEncryptedData)
{
    // 为加密后的数据分配内存
    uint8_t *encryptedData = (uint8_t *)malloc(dataLen + SM4_BLOCK_SIZE);  // 为加密数据预留空间
    if (encryptedData == nullptr) 
    {
        qDebug() << "Memory allocation failed for encrypted data!";
        return false;
    }
    
    size_t encrypted_len = 0;
    // 对图像数据进行加密
    int ret = sm4_cbc_padding_encrypt(&encrypt_key, iv, data, dataLen, encryptedData, &encrypted_len);
    if(ret != 1)
    {
        qDebug() << "Failed to encrypt image.";
        return false;
    }
    
    QByteArray byteArray(reinterpret_cast<const char*>(encryptedData), encrypted_len);
    strEncryptedData = byteArray.toHex();
    
    // 释放分配的内存
    free(encryptedData);
    
    return true;
}

bool mySm4::decrptStr(unsigned char *&encryptedData, size_t &encryptedLen, QString &strDecryptedData)
{
    // 为解密后的数据分配内存
    uint8_t *decryptedData = (uint8_t *)malloc(encryptedLen);  // 解密后的数据大小不会超过加密数据的大小
    if (decryptedData == nullptr)
    {
        qDebug() << "Memory allocation failed for decrypted data!";
        return false;
    }
    
    size_t decryptedLen = 0;
    
    int ret = sm4_cbc_padding_decrypt(&decrypt_key, iv, encryptedData, encryptedLen, decryptedData, &decryptedLen);
    if(ret != 1)
    {
        qDebug() << "Failed to decrypt data.";
        free(decryptedData);
        return false;
    }
    
    QByteArray byteArray(reinterpret_cast<const char *>(decryptedData), decryptedLen);
    strDecryptedData = QString::fromUtf8(byteArray);
    
    free(decryptedData);
    return true;
}
