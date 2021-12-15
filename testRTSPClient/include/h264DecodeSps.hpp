#pragma once
#include <cmath>
#include <iostream>
UINT Ue(BYTE* pBuff, UINT nLen, UINT& nStartBit)
{
    // count 0 bit
    UINT nZeroNum = 0;
    while (nStartBit < nLen * 8)
    {
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            break;
        }
        nZeroNum++;
        nStartBit++;
    }
    nStartBit++;


    // count
    DWORD dwRet = 0;
    for (UINT i = 0; i < nZeroNum; i++)
    {
        dwRet <<= 1;
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            dwRet += 1;
        }
        nStartBit++;
    }
    return (1 << nZeroNum) - 1 + dwRet;
}


int Se(BYTE* pBuff, UINT nLen, UINT& nStartBit)
{
    int UeVal = Ue(pBuff, nLen, nStartBit);
    double k = UeVal;
    int nValue = ceil(k / 2);
    if (UeVal % 2 == 0)
        nValue = -nValue;
    return nValue;
}


DWORD u(UINT BitCount, BYTE* buf, UINT& nStartBit)
{
    DWORD dwRet = 0;
    for (UINT i = 0; i < BitCount; i++)
    {
        dwRet <<= 1;
        if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            dwRet += 1;
        }
        nStartBit++;
    }
    return dwRet;
}

void de_emulation_prevention(BYTE* buf, unsigned int* buf_size)
{
    int i = 0, j = 0;
    BYTE* tmp_ptr = NULL;
    unsigned int tmp_buf_size = 0;
    int val = 0;

    tmp_ptr = buf;
    tmp_buf_size = *buf_size;
    for (i = 0; i < (tmp_buf_size - 2); i++)
    {
        //check for 0x000003
        val = (tmp_ptr[i] ^ 0x00) + (tmp_ptr[i + 1] ^ 0x00) + (tmp_ptr[i + 2] ^ 0x03);
        if (val == 0)
        {
            //kick out 0x03
            for (j = i + 2; j < tmp_buf_size - 1; j++)
                tmp_ptr[j] = tmp_ptr[j + 1];

            //and so we should devrease bufsize
            (*buf_size)--;
        }
    }
}

bool h264_idr_frame(BYTE* buf, unsigned int nLen)
{
    UINT StartBit = 0;
    de_emulation_prevention(buf, &nLen);

    int forbidden_zero_bit = u(1, buf, StartBit);
    int nal_ref_idc = u(2, buf, StartBit);
    int nal_unit_type = u(5, buf, StartBit);
    if (nal_unit_type == 5) {
        return true;
    }
    return false;
}