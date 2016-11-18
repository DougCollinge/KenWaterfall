#ifndef FRAMEINFO_HPP
#define FRAMEINFO_HPP
#include <QtCore/QObject>
#include <QtCore/QByteArray>

class Protocol;

class FrameInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_bytID             READ getBytID WRITE setM_bytID)
    Q_PROPERTY(QString m_name              READ getName)              // read only
    Q_PROPERTY(qint32  m_samplesToComplete READ getSamplesToComplete) // read only

public:
    explicit FrameInfo(QByteArray bytFrameType,
                      bool    *blnOdd,
                      qint32  *intNumCar,
                      QString *strMod,
                      qint32  *intBaud,
                      qint32  *intDataLen,
                      qint32  *intRSLen,
                      QByteArray *bytQualThres,
                      QString *strType);

    virtual ~FrameInfo();

    QByteArray bytValidFrameTypes;

    // propterty sets and gets   //
    QString getName()              { return m_name; }
    QString getBytID();
    qint32  getSamplesToComplete();
    QByteArray getDataModes(qint32);
    //                           //

    bool    getFrameInfo(quint8   bytFrameType,
                         bool    *blnOdd,
                         qint32  *intNumCar,
                         QString *strMod,
                         qint32  *intBaud,
                         qint32  *intDataLen,
                         qint32  *intRSLen,
                         QString *strType);


    bool isValidFrameType(quint8);
    bool isShortControlFrame(quint8);

    quint8 getFrameCode(QString strFrameName);     //!< Function to look up the byte value from the frame string name
    quint8 ComputeTypeParity(quint8 bytFrameType); //!< A function to compute the parity symbol used in the frame type encoding

private:
    // property vars   //
    quint8  m_bytID;
    qint32  m_samplesToComplete;
    QString m_name;
    //                 //

    QStringList strFrameType[255];

    Protocol  *objProtocol;
};

#endif // FRAMEINFO_HPP
