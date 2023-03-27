//// UBXWaveAnalyze.cpp : 实现文件
////

//#include "stable.h"
//#include "UBXMotionEditor.h"

//#include "UBXWaveAnalyze.h"


//// CUBXWaveAnalyze

//#include <MMSystem.h>
//#define ID_RIFF	mmioFOURCC('R', 'I', 'F', 'F')
//#define ID_WAVE	mmioFOURCC('W', 'A', 'V', 'E')
//#define ID_fmt	mmioFOURCC('f', 'm', 't', ' ')
//#define ID_data	mmioFOURCC('d', 'a', 't', 'a')

//CUBXWaveAnalyze::CUBXWaveAnalyze(void)
//{
//    m_bFileOpened = false;
//    m_dwChannels = 0;
//    m_nSamples = 0;
//    m_nSampleRate = 0;
//    m_pDm = m_pDl = m_pDr = NULL;
//    m_pArc = NULL;
//}


//CUBXWaveAnalyze::~CUBXWaveAnalyze(void)
//{

//}
//bool CUBXWaveAnalyze::Serialize()
//{
//    if (m_pArc->IsStoring())
//    {
//        // TODO: add storing code here 存储
//    }
//    else
//    {
//        //m_strWavePath=m_pArc->GetFile()->GetFilePath();

//        bool format = false, data = false;
//        WORD bytes;
//        DWORD i, pos=12, fileSize = m_pArc->GetFile()->GetLength();
//        float df; // unitm_pArcy quantization step
//        FOURCC id;
//        DWORD fileLen, chkLen, sampleRate, avgBytesRate;
//        WORD formatTag, bitsPerSample, extSize, blockAlign;
//        // read RIFF header
//        *m_pArc >> id;
//        if (id != ID_RIFF)
//        {
//            MessageBox(NULL, QObject::tr("Not RIFF format!"), QObject::tr("Error"), MB_OK);
//            //throw(new CFileException(CFileException::generic));
//            return false;
//        }
//        *m_pArc >> fileLen;
//        if (fileLen != fileSize - 8)
//        {
//            MessageBox(NULL, QObject::tr("Wrong file size!"), QObject::tr("Error"), MB_OK);
//            //throw(new CFileException(CFileException::generic));
//            return false;
//        }
//        // read wave id
//        *m_pArc >> id;
//        if (id != ID_WAVE)
//        {
//            MessageBox(NULL, QObject::tr("Not WAVE format!"), QObject::tr("Error"), MB_OK);
//            //throw(new CFileException(CFileException::generic));
//            return false;
//        }
//        // read chunks
//        while (!format || !data || pos < fileSize)
//        {
//            *m_pArc >> id;
//            *m_pArc >> chkLen;
//            if (id == ID_fmt)
//            { // read format chunk
//                *m_pArc >> formatTag;
//                if (formatTag != 1)
//                {
//                    MessageBox(NULL, QObject::tr("Not PCM format!"), QObject::tr("Error"), MB_OK);
//                    //throw(new CFileException(CFileException::generic));
//                    return false;
//                }
//                *m_pArc >> m_dwChannels;
//                *m_pArc >> sampleRate;
//                m_nSampleRate = sampleRate;
//                *m_pArc >> avgBytesRate;
//                *m_pArc >> blockAlign;
//                *m_pArc >> bitsPerSample;
//                if (chkLen == 18)
//                {
//                    *m_pArc >> extSize;

//                    if (extSize > 0)
//                    {
//                        char *buf = new char[extSize];
//                        m_pArc->Read(buf, (uint)extSize);
//                        delete[] buf;
//                        buf = NULL;
//                    }
//                }
//                else
//                    extSize = 0;
//                pos += 8 + chkLen + extSize;
//                format = true;
//            }
//            else if (id == ID_data)
//            { // read data chunk
//                if (!format)
//                {
//                    MessageBox(NULL, QObject::tr("No format chunk before data chunk!"), QObject::tr("Error"), MB_OK);
//                    //throw(new CFileException(CFileException::generic));
//                    return false;
//                }
//                // validate and calculate pm_pArcameters
//                if (bitsPerSample <= 0) {
//                    MessageBox(NULL, QObject::tr("Wrong bits per sample!"), QObject::tr("Error"), MB_OK);
//                    //throw(new CFileException(CFileException::generic));
//                    return false;
//                }
//                bytes = (WORD)((bitsPerSample + 7) / 8);
//                if (bytes > 2)
//                {
//                    MessageBox(NULL, QObject::tr("Too lm_pArcge bits per sample!"), QObject::tr("Error"), MB_OK);
//                    //throw(new CFileException(CFileException::generic));
//                    return false;
//                }
//                m_nSamples = chkLen / (m_dwChannels * bytes);
//                if (m_nSamples <= 0)
//                {
//                    MessageBox(NULL, QObject::tr("The number of the sample is 0!"), QObject::tr("Error"), MB_OK);
//                    //throw(new CFileException(CFileException::generic));
//                    return false;
//                }
//                //timeLen = (float)m_nSamples / sampleRate;

//                // calculate unitm_pArcy quantization step
//                df = 1.0f / (1l << (bitsPerSample - 1));

//                // read in sample data
//                if (m_dwChannels == 2)
//                { // stereo
//                    // allocate memory for float sample data
//                    m_pDl = new float[m_nSamples];
//                    m_pDr = new float[m_nSamples];
//                    // allocate temporm_pArcy memory and read sample data
//                    BYTE (*b2)[2];
//                    short (*s2)[2];
//                    if (bytes == 1)
//                    {
//                        b2 = new BYTE[m_nSamples][2];
//                        m_pArc->Read(b2, m_nSamples*2);
//                    }
//                    else
//                    {
//                        s2 = new short[m_nSamples][2];
//                        m_pArc->Read(s2, m_nSamples*4);
//                    }
//                    // convert sample to unitm_pArcy float data
//                    for (i = 0; i < m_nSamples; i ++ )
//                    {
//                        m_pDl[i] = ((bytes == 1) ? b2[i][0] - 128 : s2[i][0]) * df;
//                        m_pDr[i] = ((bytes == 1) ? b2[i][1] - 128 : s2[i][1]) * df;
//                    }
//                    if (bytes == 1)
//                    {
//                        delete[] b2;
//                        b2 = NULL;
//                    }
//                    else
//                    {
//                        delete[] s2;
//                        s2 = NULL;
//                    }
//                }
//                else { // mono
//                    // allocate memory for float sample data
//                    m_pDm = new float[m_nSamples];
//                    // allocate temporm_pArcy memory and read sample data
//                    BYTE *b1;
//                    short *s1;
//                    if (bytes == 1)
//                    {
//                        b1 = new BYTE[m_nSamples];
//                        m_pArc->Read(b1, m_nSamples);
//                    }
//                    else
//                    {
//                        s1 = new short[m_nSamples];
//                        m_pArc->Read(s1, m_nSamples*2);
//                    }
//                    // convert sample to unitm_pArcy float data
//                    for (i = 0; i < m_nSamples; i ++ )
//                        m_pDm[i] = ((bytes == 1) ? b1[i] - 128 : s1[i]) * df;
//                    if (bytes == 1)
//                        delete[] b1;
//                    else
//                        delete[] s1;
//                }
//                pos += 8 + chkLen;
//                data = true;
//                break;
//            }
//            else
//            { //read other chunk
//                char* buf = new char[chkLen];
//                m_pArc->Read(buf, chkLen);
//                pos += 8 + chkLen;
//                delete[] buf;
//            }
//        }
//        //m_pArc.Close();
//        if (!format || !data) {
//            MessageBox(NULL, QObject::tr("Wrong file data!"), QObject::tr("Error"), MB_OK);
//            //throw(new CFileException(CFileException::generic));
//            return false;
//        }
//        m_bFileOpened = true;
//    }
//    return true;
//}

//bool CUBXWaveAnalyze::Connect(QString strWavePath)
//{
//    CFile file;
//    if (file.Open(strWavePath,CFile::modeRead))
//    {
//        int nResult = false;
//        CArchive *par = new CArchive(&file,CArchive::load/*|CArchive::store*/,100,NULL);
//        m_pArc = par;
//        DisConnect();
//        if(Serialize())
//        {
//            nResult = true;
//        }
//        if (m_pArc)
//        {
//            delete m_pArc;
//            m_pArc = NULL;
//        }
//        file.Close();
//        return nResult;
//    }
//    return false;
//}
//void CUBXWaveAnalyze::Release()
//{
//// 	if (m_pArc)
//// 	{
//// 		m_pArc->Close();
//// 		delete m_pArc;
//// 		m_pArc = NULL;
//// 	}
//    DisConnect();
//}
//void CUBXWaveAnalyze::DisConnect()
//{
//    if (m_pDl)
//    {
//        delete []m_pDl;
//        m_pDl = NULL;
//    }
//    if(m_pDm)
//    {
//        delete []m_pDm;
//        m_pDm = NULL;
//    }
//    if(m_pDr)
//    {
//        delete []m_pDr;
//        m_pDr = NULL;
//    }
//// 	if (m_pArc)
//// 	{
//// 		m_pArc->Flush();
//// 		m_pArc->Close();
//// 		delete m_pArc;
//// 		m_pArc = NULL;
//// 	}
//}

//// CUBXWaveAnalyze 成员函数
