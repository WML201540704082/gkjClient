#ifndef MYSM4_H
#define MYSM4_H

#include <QObject>
#include "sm4/sm4.h"
#include "sm4/hex.h"
#include "sm4/rand.h"
#include "sm4/error.h"

class mySm4 : public QObject
{
    Q_OBJECT
public:
    explicit mySm4(QObject *parent = nullptr);
    
    ///
    /// \brief iniKey 初始化加解密秘钥
    ///
    void iniKey();
    
    ///
    /// \brief encrypt 加密图像数据函数
    /// \param data
    /// \param dataLen
    /// \param savePath
    /// \return 
    /// 
    bool encrypt(unsigned char* data, unsigned long dataLen, QString savePath);
    
    ///
    /// \brief decrypt 从文件中读取加密数据并解密
    /// \param filename
    /// \param encryptedData
    /// \param encryptedLen
    /// \param decryptedData
    /// \param decryptedLen
    /// \return 
    ///
    bool decrypt(const QString &filename, uint8_t *&encryptedData, size_t &encryptedLen, uint8_t *&decryptedData, size_t &decryptedLen);
    
    ///
    /// \brief writeEncryptedDataToFile 加密数据写入文件
    /// \param data
    /// \param len
    /// \param filename
    /// \return 
    ///
    bool writeEncryptedDataToFile(const uint8_t *data, size_t len, const QString &filename);
    
    ///
    /// \brief readEncryptedDataFromFile
    /// \param filename
    /// \param data
    /// \param len
    /// \return 
    ///
    bool readEncryptedDataFromFile(const QString &filename, uint8_t *&data, size_t &len) ;
    
    ///
    /// \brief encryptStr 加密系统用户名密码
    /// \param data
    /// \param dataLen
    /// \param strEncryptedData
    /// \return 
    ///
    bool encryptStr(unsigned char* data, unsigned long dataLen, QString &strEncryptedData);
    
    ///
    /// \brief decrptStr 解密密码
    /// \param encryptedData
    /// \param encryptedLen
    /// \param strDecryptedData
    /// \return 
    ///
    bool decrptStr(unsigned char *&encryptedData, size_t &encryptedLen, QString &strDecryptedData);
    
    ///
    /// \brief decrptStr2 解密系统用户名或密码
    /// \param encryptedData
    /// \param encryptedLen
    /// \param strDecryptedData
    /// \return 
    ///
    bool decrptStr2(unsigned char *&encryptedData, size_t &encryptedLen, QString &strDecryptedData);
signals:
    
private:
    const uint8_t user_key[16] = {0x35, 0x13, 0x41, 0x69, 0x09, 0xA1, 0xAD, 0x8E,
                                  0x27, 0x85, 0x57, 0x76, 0x98, 0xBA, 0xDC, 0xFE};
    const uint8_t iv[16] = {0xAD, 0x01, 0xC2, 0xB3, 0x64, 0x21, 0xCC, 0x77,
                            0xFF, 0x12, 0x0A, 0x0D, 0xEC, 0x7B, 0x0E, 0x0F};
    
    SM4_KEY encrypt_key;
    SM4_KEY decrypt_key;
};

#endif // MYSM4_H
